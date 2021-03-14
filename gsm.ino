#define SIM800L_IP5306_VERSION_20200811

#define MODEM_RST       5
#define MODEM_PWKEY     4
#define MODEM_POWER_ON  23
#define MODEM_TX        27
#define MODEM_RX        26
#define I2C_SDA         21
#define I2C_SCL         22

#define BLYNK_PRINT Serial
#define BLYNK_HEARTBEAT 30
#define TINY_GSM_MODEM_SIM800

#include <TinyGPS++.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>

#include <Wire.h>
#include "settings.h"

#define SerialMon Serial
#define SerialAT Serial1
#define LED 13


const char apn[]  = "indosatgprs";
const char user[] = "";
const char pass[] = "";

const char auth[] = "tTRE5J-DSIWucdJyRYgqvQhEolfhHbRf";

TinyGsm modem(SerialAT);
TinyGPSPlus gps;

WidgetMap myMap(V0);


void setup() {
  SerialMon.begin(115200);
  delay(10);

  Wire.begin(I2C_SDA, I2C_SCL);
  bool   isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  SerialMon.println("inisialisasi Modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);

  SerialMon.print("menunggu jaringan...");
  if (!modem.waitForNetwork(240000L)) {
    SerialMon.println(" gagal");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("menghubungkan ke Jaringan: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" gagal menghubungkan ke jaringan");
    digitalWrite(LED, LOW);
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Terhubung ke Jaringan");
    SerialMon.print(apn);
    digitalWrite(LED, HIGH);
  }


  Blynk.begin(auth, modem, apn, user, pass);
  
}

void loop() {
  while (Serial.available() > 0)
    if (gps.encode(Serial.read())) {
      gpsInfo();
    }
  Blynk.run();
  //batteryInfo();
  

}

void gpsInfo() {
  if (gps.location.isValid() ) {

  int index = 1;
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();
  float speed =gps.speed.kmph();
  myMap.location(index, latitude, longitude, "GPS_Location");
  Blynk.virtualWrite(V1, String(latitude));
  Blynk.virtualWrite(V2, String(longitude));
  Blynk.virtualWrite(V3, speed);
}
}
/*
void batteryInfo() {
  int BateraiLevel = analogRead(35) * 2 / 1135;
  Blynk.virtualWrite(V1, BateraiLevel); */
  
