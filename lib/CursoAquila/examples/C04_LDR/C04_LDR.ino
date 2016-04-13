#define LED 13
#define LDR A7

int valor = 0;

void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
}

void loop() 
{
  valor = analogRead(LDR);
  valor = map(valor, 300, 1023, 0, 255);
  analogWrite(LED, valor);
}
