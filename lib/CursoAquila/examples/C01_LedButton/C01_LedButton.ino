// Enciende el LED (Activo en BAJO) al presionar el boton (en Pull Up)
#define LED 13       // Se define que el LED esta en el pin 13 (Interno)
#define BUTTON 33    // Se define que el boton esta en el pin 33 (Interno)

void setup() // Se ejecuta una sola vez
{
  pinMode(LED, OUTPUT);    // Configura el pin del LED como salida
  pinMode(BUTTON, INPUT);  // Configura el pin del boton como entrada
}

void loop() // Se ejecuta infinitamente
{
  if(digitalRead(BUTTON) == LOW)  // Revisa si el boton esta presionado
  {
    digitalWrite(LED, LOW);  // Enciende el LED
  }
  else  // Si no esta presionado
  {
    digitalWrite(LED, HIGH);  // Apaga el LED
  }
}
