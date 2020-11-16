//a very simple dekatron spinner - control the rate of spin with i_delay in the void_setup section - lower number = faster spin.
//spin direction should be clockwise - reverse the spin by changing D_count++; to D_count--; in void loop()

int D_count = 0;
int i_delay = 100;
int Guide1 = 5;       // Guide 1 - G1 pin of 2-guide Dekatron
int Guide2 = 6;       // Guide 2 - G2 pin of 2-guide Dekatron
int Index  = 7;       // Index   - NDX input pin. High when glow at K0

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

// setup() runs once, at reset, to initialize system
void setup() {
  pinMode(Guide1, OUTPUT);
  pinMode(Guide2, OUTPUT);
  pinMode(Index, INPUT);

  D_count = 0;
  i_delay = 100;
  G_step(D_count);
}

// the loop function runs over and over again forever
void loop() {
  D_count++;
  delay(i_delay);            // wait per i_rate
  if (D_count > 2) D_count = 0;
  if (D_count < 0) D_count = 2;
  G_step(D_count);          // Step Dekatron
}
