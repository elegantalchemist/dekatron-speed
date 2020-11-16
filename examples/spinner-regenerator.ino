//regenerator for dekatron spinners. Set the pinSpeed from 1-200ish, and it auto reverse direction every 20 seconds
//this sketch ignores any input from k0
//this sketch is aimed at running a dekatron back and forth for long periods of time which can help with sticky pins
//or just looking cool.

int D_count = 0;
int Guide1 = 3;       // Guide 1 - G1 pin of 2-guide Dekatron
int Guide2 = 5;       // Guide 2 - G2 pin of 2-guide Dekatron
int Index  = 2;       // Index   - NDX input pin. High when glow at K0
int Ndx = 0;          // K0 index indicator variable
int pinSpeed = 10;     // pinSpeed - speed of next step in milliseconds - low value = fast, high = slow, try 5-40ish
boolean Dir = false;  // Direction Flag, true for clockwise
long previousMillis = 0; //non blocking delay timer
long interval = 20000;   //time to reverse direction of spin based on

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
    digitalWrite(Guide2, HIGH);
    delayMicroseconds(15);
    digitalWrite(Guide1, LOW);

  }
}

// setup() runs once, at reset, to initialize system
void setup() {
  pinMode(Guide1, OUTPUT);
  pinMode(Guide2, OUTPUT);
  pinMode(Index, INPUT);

  D_count = 0;
  G_step(D_count);
}

// the loop function runs over and over again forever
void loop() {
    unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) 
  {
    //reverse the direction boolean and restart counter
    Dir = !Dir; 
    previousMillis = currentMillis;   
  }
  if (Dir)
  {
    D_count++;              // increment D_count for clockwise motion
  }
  else
  {
    D_count--;              // decrement D_count for counter-clockwise motion
  }
  
  delay(pinSpeed);            // wait per i_rate
  if (D_count > 2) D_count = 0;
  if (D_count < 0) D_count = 2;
  G_step(D_count);          // Step Dekatron
}
