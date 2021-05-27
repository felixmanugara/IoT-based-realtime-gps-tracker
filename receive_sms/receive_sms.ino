void setup() {
  ReceiveMode();

}

void loop() {
  updateSerial();

}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    SerialAT.write(Serial.read());
  }
  while (SerialAT.available())
  {
    Serial.write(SerialAT.read());
  }
}

void ReceiveMode()
{ 
  //Serial.print("AT");
  SerialMon.println("AT"); // check AT command
  updateSerial();
  
  //Serial.print("AT+CMGF=1");
  SerialMon.println("AT+CMGF=1"); // AT command untuk mengatur sim800 ke mode text
  updateSerial();

  //Serial.print("AT+CNMI=2,2,0,0,0");
  SerialMon.println("AT+CNMI=2,2,0,0,0");
  updateSerial();
}

boolean SMSRequest()
{
  if (SerialAT.available() > 0)
  {
    incomingsms = Serial1.read();
    if (incomingsms == 'G') 
    {
      delay(10);
      SerialMon.print(incomingsms);
      incomingsms = Serial1.read();
      if (incomingsms == 'P') 
      {
        delay(10);
        SerialMon.print(incomingsms);
        incomingsms = Serial1.read();
        if (incomingsms = 'S')
        {
          delay(10);
          SerialMon.print(incomingsms);
          SerialMon.print("..permintaan diterima \n");
          return true;
        }
      }
    }
  }
return false;
}
