#define LED 5
#define BUTTON 6  // Boton externo en configuracion Pull up
int estado_led = LOW;
int estado_boton_actual = LOW;
int estado_boton_anterior = HIGH;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(LED, estado_led);
}

void loop()
{
  estado_boton_actual = digitalRead(BUTTON);
  if(estado_boton_actual == LOW && estado_boton_anterior == HIGH)
  {
    estado_led = !estado_led;
    digitalWrite(LED, estado_led);
    estado_boton_anterior = LOW;
  }
  else if(estado_boton_actual == HIGH)
  {
    estado_boton_anterior = HIGH;
  }

}
