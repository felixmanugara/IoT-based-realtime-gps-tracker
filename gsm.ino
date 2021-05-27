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

void SMSData();
void buzzer_State();

// kode di bawah merupakan button untuk sms
BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    SMSData();
  }
}

// kode di bawah merupakan button untuk Buzzer
BLYNK_WRITE(V6)
{
  int buzzValue = param.asInt();
  if(buzzValue == 1) {
    buzzer_State();
  }

}


// variabel untuk menyimpan data GPS
double latitude;
double longitude;
float altitude;
float kecepatan;
int satellite;
String arah;

// variable untuk fungsi SMS



const char apn[]  = "indosatgprs";
const char user[] = "";
const char pass[] = "";

// ini adalah token autentifikasi untuk aplikasi blynk
const char auth[] = "tTRE5J-DSIWucdJyRYgqvQhEolfhHbRf";

int BUZZER = 2;
BlynkTimer timer;

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
  pinMode(BUZZER, OUTPUT);
  
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
   
  if (!modem.waitForNetwork(240000L)) 
  {
    SerialMon.println(" gagal");
    delay(10000);
    return;
  }
  
  SerialMon.println(" berhasil");

  if (modem.isNetworkConnected()) 
  {
    SerialMon.println("Jaringan terhubung");
  } 
 
  SerialMon.print("Menghubungkan ke APN: ");
  SerialMon.print(apn);
  
  if (!modem.gprsConnect(apn, user, pass)) 
  {
    SerialMon.println(" koneksi ke GPRS gagal");
    delay(10000);
    return;
  }
  SerialMon.println(" berhasil");

  Blynk.begin(auth, modem, apn, user, pass);
  timer.setInterval(5000L, GPSInfo);

  if (Blynk.connected()) {
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);
  }


}
 
void GPSInfo() 
{
  if (gps.charsProcessed() < 10)
   {
      Serial.println("GPS tidak terdeteksi");
      Blynk.virtualWrite(V4, "Lokasi tidak Terdeteksi");
   }
}

void loop()
{
  
  while (Serial.available() > 0)
  {
    if (gps.encode(Serial.read()))
       GPSData();
        
  }

  Blynk.run();
  timer.run(); 
  
}

void GPSData() 
{
  if (gps.location.isValid())
  {
    unsigned int index = 1;
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    //altitude = gps.altitude.meters();
    kecepatan = gps.speed.kmph();
    arah = TinyGPSPlus::cardinal(gps.course.value());
    //satellite = gps.satellites.value();

    Blynk.virtualWrite(V1, double(latitude),",");
    Blynk.virtualWrite(V2, double(longitude),",");
    Blynk.virtualWrite(V3, kecepatan);
    Blynk.virtualWrite(V4, arah);
    
    myMap.location(index, latitude, longitude, "Lokasi Terkini"); 
  }
}

void SMSData() 
{
  String no_hp = "+6281219918587";
  String sms = "Latitude" + String(latitude) + " Longitude" + String(longitude);
  modem.sendSMS(no_hp, sms);
  //Serial.println("berhasil mengirim sms");
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
}

void buzzer_State() {
  digitalWrite(BUZZER, HIGH);
  delay(1000);
  digitalWrite(BUZZER, LOW);
}
