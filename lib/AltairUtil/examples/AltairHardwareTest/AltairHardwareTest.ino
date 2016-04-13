#define LEDR 13
#define LEDG 14
#define LEDB 15
#define BUTTON 33

// Checar que el led de ON (Verde) este encendido.
// Checar que el boton RESET funcione
// Checar que el boton 33 funcione

// Falta checar la comunicacion y chip MAC

// Pines del header ICSP son probados a la hora de programacion del bootloader
// Pines del header SERIAL son probados al subir este sketch
// Pines AREF y 19 no son probados

void checkPins()
{
  // Clearing all pins
  for(int i = 0; i < 28; i++)
  {
    if(i == 13 || i == 14 || i == 15) i = 16;
    digitalWrite(i, LOW);
  }
  delay(100);
  // Setting all pins:
  for(int i = 0; i < 28; i++)
  {
    if(i == 19) i++;
    if(i == 13 || i == 14 || i == 15)
    {
      digitalWrite(i, LOW);
      delay(200);
      digitalWrite(i, HIGH);
    }
    else
    {
      digitalWrite(i, HIGH);
      delay(200);
      digitalWrite(i, LOW);
    }
    
  }
}

void checkRGBLED()
{
  // Apagando LED RGB:
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  
  digitalWrite(LEDR, LOW);
  delay(500);
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  delay(500);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
  delay(500);
  digitalWrite(LEDB, HIGH);
}

void setup()
{
  // Configurando pines del 0 al 27
  for(int i = 0; i < 28; i++)
  {
    pinMode(i, OUTPUT);
  }
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  
  // configuracion del boton:
  pinMode(BUTTON, INPUT);
  
  Serial.begin(9600);

}

bool sToggle = false;

void loop()
{
  if(!digitalRead(BUTTON))
  {
    //checkRGBLED();
    checkPins();
    Serial.println("hola");
  }
  if(Serial.available())
  {
    Serial.println(Serial.read());
    sToggle != sToggle;
    digitalWrite(LEDB, sToggle);
  }

}
