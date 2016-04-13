#define LED 13
#define POT A7

int valor = 0;

void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(POT, INPUT);
}

void loop() 
{
  valor = analogRead(POT);
  valor = map(valor, 0, 1023, 0, 255);
  analogWrite(LED, valor);
}
