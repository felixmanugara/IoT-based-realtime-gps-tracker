// inisiasi pin pada ESP32
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22
#define LED                  13

#define BLYNK_PRINT Serial    
#define BLYNK_HEARTBEAT 30
#define TINY_GSM_MODEM_SIM800

#include <TinyGPS++.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>

#include <Wire.h>
#include "settings.h"

// set serial monitor untuk modul gsm dan gps
#define SerialMon Serial

// komunikasi serial untuk modul gsm
#define SerialAT Serial1

// variabel untuk menyimpan data GPS
double latitude;
double longitude;
//float speed;
//int satellites;
//String direction;


const char apn[]  = "indosatgprs";
const char user[] = "";
const char pass[] = "";

// ini adalah token authentifikasi untuk aplikasi blynk
const char auth[] = "tTRE5J-DSIWucdJyRYgqvQhEolfhHbRf";

// membuat instance modem untuk modul GSM
TinyGsm modem(SerialAT);

// membuat instance untuk modul GPS
TinyGPSPlus gps;
WidgetMap myMap(V0);

void setup()
{
  // set baud rate untuk serial monitor
  Serial.begin(9600);
  delay(10);

  // kode ini berguna agar ESP32 dapat bekerja dengan daya baterai
  Wire.begin(I2C_SDA, I2C_SCL);
  bool   isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // pengaturan baud rate untuk modul GSM dan komunikasi UART
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // ini digunakan untuk restart modul GSM
  SerialMon.println("Inisialsasi GSM modul..");
  modem.restart();

  String modulInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modulInfo);

  

   SerialMon.print("Menunggu jaringan...");
  if (!modem.waitForNetwork(240000L)) {
    SerialMon.println(" gagal");
    delay(10000);
    return;
  }
  SerialMon.println(" berhasil");

  if (modem.isNetworkConnected()) {
    digitalWrite(LED, HIGH);
    SerialMon.println("Jaringan terhubung");
  } 
 
  SerialMon.print("Menghubungkan ke APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" gagal");
    delay(10000);
    return;
  }
  SerialMon.println(" berhasil");

  Blynk.begin(auth, modem, apn, user, pass);
}

void loop()
{
  while (Serial.available() > 0)
  {
    if (gps.encode(Serial.read()))
       gpsData();
       
  }

  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("GPS tidak terdeteksi"));
  }
     
  Blynk.run();
}

void gpsData() 
{
  if (gps.location.isValid())
  {
    unsigned int index = 1;
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    float altitude = gps.altitude.meters();
    
    Serial.print("latitude: ");
    Serial.println(latitude,5);
    Serial.print("longitude: ");
    Serial.println(longitude,5);
    Serial.print("altitude: ");
    Serial.println(altitude);

    BLYNK_WRITE(V1);
      table.addRow("Latitude", latitude);
    /*
    Blynk.virtualWrite(V1, double(latitude),5);
    Blynk.virtualWrite(V2, double(longitude),5);
    myMap.location(index, latitude, longitude, "Lokasi Terkini"); */
  }
}

/*
void dateData()
{
  if (gps.date.isValid())
  {
    Serial.print("Date: ");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
} */
