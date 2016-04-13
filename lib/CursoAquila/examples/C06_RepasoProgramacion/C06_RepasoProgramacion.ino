// Vamos a Cambiar el color del LED presionando el boton
// LEDR 0, LEDG 1, LEDB 2
#define LEDR 13
#define LEDG 14
#define LEDB 15
#define BUTTON 33

int ledActual = 0;

bool estadoAnteriorBtn = true;

bool botonPresionado()
{
  bool estadoActual = digitalRead(BUTTON);
  
  if(estadoActual == LOW && estadoAnteriorBtn == HIGH)
  {
    estadoAnteriorBtn = LOW;
    return true;
  }
  else if(estadoActual == HIGH) 
  {
    estadoAnteriorBtn = HIGH;
  }
  
  return false;
}

void encenderLedN(int n)
{
  switch(n)
  {
    case 0:  // Encender LEDR
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
    break;
    case 1:  // Encender LEDG
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);
    break;
    case 2:  // Encender LEDB
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);
    break;
    default:
    break;
  }
}

void setup()
{
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop()
{ 
  if(botonPresionado())
  {
    ledActual = ledActual + 1;
    if(ledActual >= 3)
    {
      ledActual = 0;
    }
  }
  
  encenderLedN(ledActual);
}
