/*
  Ejemplo de "eco" en comunicacion serial.
  Cuando recibe algo lo envia de vuelta.
*/

void setup()
{
  // Inicia comunicacion serial a 9600 bauds:
  Serial.begin(9600);
}

void loop()
{
  // Si recibimos algo:
  if(Serial.available())
  {
    // Lee caracter recibido:
    char a = Serial.read();
    
    // Envia el caracter mas un fin de linea:
    Serial.println(a);  
    
    //Serial.print(a);  // Envia el caracter solo
  }
}
