/* ARDUINO/ESP8266 and DEKATRON Broadband Monitor 2020 
 *  A project utilising SNMP queries via Wi-Fi to a router
 *  to determine in/outOctets and determining bandwidth saturation
 *  which is displayed on a Dekatron by changing rate of spin
 *  
 *  Ig you use this you need to CHANGE your WiFi info, router IP, OIDs,
 *  community settings, hardcoded max speeds and you should be good to go.
 *  
 *  OIDs
 *  last value in the (.xxxx here) OID is the interface number - this 
 *  will be different depending on your router. Use Paessler
 *  SNMP tester 3.0 to pull a list of interfaces to pick from and
 *  test uptime/community settings are working OK.
 *    
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>    // For Posting data to web logging service
#include <millisDelay.h>          // https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html#using
#include <Arduino_SNMP_Manager.h> // https://github.com/shortbloke/Arduino_SNMP_Manager

//************************************
//* Your WiFi info                   *
//************************************
const char *ssid = "xxxx";
const char *password = "xxxx";
//************************************

//************************************
//* SNMP Device Info                 *
//************************************
IPAddress router(192, 168, 1, xxxx);
const char *community = "xxxx";                          // SNMP Community String
const int snmpVersion = 1;                                 // SNMP Version 1 = 0, SNMP Version 2 = 1
char *oidInOctets = ".1.3.6.1.2.1.2.2.1.10.xxxx";             // Counter32 ifInOctets.4
char *oidOutOctets = ".1.3.6.1.2.1.2.2.1.16.xxxx";            // Counter32 ifOutOctets.4
char *oidUptime = ".1.3.6.1.2.1.1.3.0";                    // TimeTicks Uptime
//************************************

//************************************
//* Settings                         *
//************************************
const int pollInterval = 3000;    // delay in milliseconds (15000 = 15 seconds)
//************************************

//************************************
//* Dekatron Settings                *
//************************************
bool reversedirection = false;
int D_count = 0;
int Guide1 = D3;       // Guide 1 - G1 pin of 2-guide Dekatron you may need to change to suit
int Guide2 = D5;       // Guide 2 - G2 pin of 2-guide Dekatron you may need to change to suit
int Index  = D2;       // Index   - NDX input pin. High when glow at K0 - may change but currently grounded and not used
int spinnerRate = 20;
//************************************

//************************************
//* Initialise                       *
//************************************
// Variables
unsigned int downSpeed = xxxxx;
unsigned int upSpeed = xxxxx;
unsigned int inOctets = 0;
unsigned int outOctets = 0;
int uptime = 0;
int lastUptime = 0;

float bandwidthInUtilPct = 0;
float bandwidthOutUtilPct = 0;
float bandwidthDifference = 0;
unsigned int lastInOctets = 0;
unsigned int lastOutOctets = 0;

//Delay timer non blocking
unsigned long previousMillis = 0;   //store interval since last time used non blocking delay

// SNMP Objects
WiFiUDP udp;                                           // UDP object used to send and recieve packets
SNMPManager snmp = SNMPManager(community);             // Starts an SMMPManager to listen to replies to get-requests
SNMPGet snmpRequest = SNMPGet(community, snmpVersion); // Starts an SMMPGet instance to send requests

// Blank callback pointer for each OID
ValueCallback *callbackDownSpeed;
ValueCallback *callbackUpSpeed;
ValueCallback *callbackInOctets;
ValueCallback *callbackOutOctets;
ValueCallback *callbackUptime;

// millisDelay timer objects
millisDelay fastPollDelay;
millisDelay pollDelay;
bool isFastPolling = false;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.softAPdisconnect(true); // Disable broadcast of local AP
  Serial.println("");
  // Wait for connection
  int dp = 8;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    // Scroll the decimal point scrolls to left whilst connecting
    if (dp == 1)
    {
      dp = 8;
    }
    else
    {
      dp = dp >> 1;
    }
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.print(" with IP address: ");
  Serial.println(WiFi.localIP());

  snmp.setUDP(&udp); // give snmp a pointer to the UDP object
  snmp.begin();      // start the SNMP Manager

  // Get callbacks from creating a handler for each of the OID
  callbackInOctets = snmp.addCounter32Handler(router, oidInOctets, &inOctets);
  callbackOutOctets = snmp.addCounter32Handler(router, oidOutOctets, &outOctets);
  callbackUptime = snmp.addTimestampHandler(router, oidUptime, &uptime);

  pinMode(Guide1, OUTPUT);
  pinMode(Guide2, OUTPUT);
  pinMode(Index, INPUT);
  
  D_count = 0;
  G_step(D_count);
}

void loop()
{
  snmp.loop();
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= pollInterval) {
          previousMillis = currentMillis;       
          getSNMP();
          calculateBandwidths();
          calculatebandwidthDifference();
    } 

    stepDekatron();
}

void calculateBandwidths()
{
  int deltaTime = 0;
  if (uptime < lastUptime)
  {
    Serial.println("Uptime less than lastUptime. Skip calculation.");
  }
  else if (uptime > 0 && lastUptime > 0)
  {
    deltaTime = (uptime - lastUptime) / 100;
    if ((deltaTime < (pollInterval / 3000)))
    {
      Serial.println("Fast Poll: Implausible sample period. Skipping.");
    }
    else
    {
      if (inOctets >= lastInOctets)
      {
        bandwidthInUtilPct = ((float)((inOctets - lastInOctets) * 8) / (float)(downSpeed * deltaTime) * 100);
      }
      else if (lastInOctets > inOctets)
      {
        Serial.println("inOctets Counter wrapped");
        bandwidthInUtilPct = (((float)((4294967295 - lastInOctets) + inOctets) * 8) / (float)(downSpeed * deltaTime) * 100);
      }
      if (outOctets >= lastOutOctets)
      {
        bandwidthOutUtilPct = ((float)((outOctets - lastOutOctets) * 8) / (float)(upSpeed * deltaTime) * 100);
      }
      else if (lastOutOctets > outOctets)
      {
        Serial.println("outOctets Counter wrapped");
        bandwidthOutUtilPct = (((float)((4294967295 - lastOutOctets) + outOctets) * 8) / (float)(upSpeed * deltaTime) * 100);
      }
Serial.print('\n');
Serial.print('\n');
Serial.print((String)"In %: "+bandwidthInUtilPct);
Serial.print((String)" - Out %: "+bandwidthOutUtilPct);
    }
  }
  // Update last samples
  lastUptime = uptime;
  lastInOctets = inOctets;
  lastOutOctets = outOctets;
}

void calculatebandwidthDifference()
{
  bandwidthDifference = fabs((float)bandwidthInUtilPct - bandwidthOutUtilPct);
  if (bandwidthInUtilPct > bandwidthOutUtilPct)
    {
     reversedirection = false;    //keep direction not reversed
    }
   else if (bandwidthOutUtilPct > bandwidthInUtilPct && bandwidthOutUtilPct > 1)
   {
    reversedirection = true;    //if more upload than download, reverse spinner
   } 
   else
   {
    reversedirection = false;    //if calculation fails or zero default to false
   }
Serial.print('\n');
Serial.print((String)"Difference %: "+bandwidthDifference);
Serial.print('\n');
  if (reversedirection)
  {
    Serial.print("Reversed!");
  }
  else
  {
    Serial.print("Normal Direction!");
  }

  spinnerRate = map(int(bandwidthDifference), 0, 90, 70000, 4000); // Map % util to spinner rate in millis
  spinnerRate = constrain(spinnerRate, 4000, 70000);
Serial.print('\n');
Serial.println((String)"Spinner Rate uS:"+spinnerRate);
}

void stepDekatron()
{
  if (reversedirection)
  {
    D_count--;              // increment D_count for clockwise motion
  }
  else
  {
    D_count++;              // decrement D_count for counter-clockwise motion
  }
  delayMicroseconds(spinnerRate);                // wait for a 33mS
  if (D_count > 2) D_count = 0;
  if (D_count < 0) D_count = 2;
  G_step(D_count);          // Step Dekatron  
}

void G_step(int CINT) // Dekatron Stepper
{
  if (CINT > 2) CINT = 0;
  if (CINT == 0)
  {
    digitalWrite(Guide1, LOW);
    digitalWrite(Guide2, LOW);
  }
  if (CINT == 1)
  {
    digitalWrite(Guide1, HIGH);
    digitalWrite(Guide2, LOW);
  }
  if (CINT == 2)
  {
    digitalWrite(Guide1, LOW);
    digitalWrite(Guide2, HIGH);
  }
}

void getSNMP()
{
  // Build a SNMP get-request, add multiple OID to a single request
  snmpRequest.addOIDPointer(callbackDownSpeed);
  snmpRequest.addOIDPointer(callbackUpSpeed);
  snmpRequest.addOIDPointer(callbackInOctets);
  snmpRequest.addOIDPointer(callbackOutOctets);
  snmpRequest.addOIDPointer(callbackUptime);

  snmpRequest.setIP(WiFi.localIP());
  snmpRequest.setUDP(&udp);
  snmpRequest.setRequestID(rand() % 5555);
  snmpRequest.sendTo(router);

  snmpRequest.clearOIDList();
}
