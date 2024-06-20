#define BLYNK_TEMPLATE_ID "TMPL6_e9Dlw-d"
#define BLYNK_TEMPLATE_NAME "Pemantauan Lingkungan Belajar Sehat"
#define BLYNK_AUTH_TOKEN "FCFrohv0cauxhk-hjppClKQGY4nfxtUn"


#define BLYNK_PRINT Serial
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>


// Definisikan pin dan jenis sensor
#define DHTPIN 33
#define DHTTYPE DHT22


#define LEDPIN 14
#define LDRPIN 32
#define TRIGPIN 26
#define ECHOPIN 25
#define BUZZERPIN 27  // Definisikan pin untuk buzzer


// Inisialisasi sensor dan LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Pengaturan WiFi
const char *ssid = "Infinix NOTE 30 Pro"; // nama wifi
const char *pass = "11222333344444"; // password wifi


void setup() {
  // Inisialisasi komunikasi serial
  Serial.begin(115200);
  delay(10);


  // Inisialisasi LCD
  lcd.begin();
  lcd.backlight();


  // Tampilkan pesan selamat datang
  lcd.setCursor(0, 0);
  lcd.print("Program Belajar");
  lcd.setCursor(0, 1);
  lcd.print("Sehat & Nyaman");
  delay(3000);  // Tampilkan pesan selama 3 detik


  // Inisialisasi DHT22
  dht.begin();
  // Inisialisasi pin LED sebagai output
  pinMode(LEDPIN, OUTPUT);
  // Inisialisasi pin sensor jarak
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  // Inisialisasi pin LDR sebagai input
  pinMode(LDRPIN, INPUT);
  // Inisialisasi pin buzzer sebagai output
  pinMode(BUZZERPIN, OUTPUT);
  // Inisialisasi koneksi WiFi
  Serial.println("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);


  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(1000);
    Serial.print(".");
    retryCount++;
  }


  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi terhubung");
  } else {
    Serial.println("");
    Serial.println("Gagal terhubung ke WiFi");
  }


  // Inisialisasi Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}


void loop() {
  Blynk.run();
  // Baca data dari DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Baca data dari LDR
  int ldrValue = analogRead(LDRPIN);
  // Baca data dari sensor jarak ultrasonik
  long duration, distance;
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10); // durasi delay 10 mikrodetik
  digitalWrite(TRIGPIN, LOW);
  duration = pulseIn(ECHOPIN, HIGH);
  distance = (duration / 2) / 29.1;
  // Definisikan kondisi abnormal
  bool abnormalCondition = false;
  String abnormalMessage = "";


  if (t > 35) {
    abnormalCondition = true;
    abnormalMessage = "Suhu Tinggi!";
    Serial.println("Abnormal: Suhu Tinggi!");
  } else if (h > 90) {
    abnormalCondition = true;
    abnormalMessage = "Kelembaban Tinggi!";
    Serial.println("Abnormal: Kelembaban Tinggi!");
  } else if (distance < 15) {
    abnormalCondition = true;
    abnormalMessage = "Objek Terlalu Dekat!";
    Serial.println("Abnormal: Objek Terlalu Dekat!");
  } else if (ldrValue > 3000) {  // ambang batas kondisi terang
    abnormalCondition = true;
    abnormalMessage = "Terlalu Redup!";
    Serial.println("Abnormal: Terlalu Redup!");
  } else if (ldrValue < 0) {  // ambang batas kondisi gelap
    abnormalCondition = true;
    abnormalMessage = "Terlalu Terang!";
    Serial.println("Abnormal: Terlalu Terang!");
  }
  // Tampilkan data pada LCD
  lcd.clear();
  if (abnormalCondition) {
    lcd.setCursor(0, 0);
    lcd.print("Abnormal:");
    lcd.setCursor(0, 1);
    lcd.print(abnormalMessage);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Suhu: ");
    lcd.print(t);
    lcd.print(" C");
   
    lcd.setCursor(0, 1);
    lcd.print("Cahaya: ");
    lcd.print(ldrValue);
    lcd.print(" D: ");
    lcd.print(distance);
    lcd.print("cm");
  }
  // Kontrol LED dan buzzer berdasarkan kondisi abnormal
  if (abnormalCondition) {
    digitalWrite(LEDPIN, HIGH);  // Nyalakan LED abnormal
    digitalWrite(BUZZERPIN, HIGH);  // Nyalakan buzzer abnormal
  } else {
    digitalWrite(LEDPIN, LOW);   // Matikan LED kondisi normal
    digitalWrite(BUZZERPIN, LOW);   // Matikan buzzer kondisi normal
  }
  // Kirim data ke Blynk
  Blynk.virtualWrite(V0, t); // Suhu
  Blynk.virtualWrite(V1, ldrValue); // Nilai LDR
  Blynk.virtualWrite(V2, distance); // Jarak
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print(" derajat Celcius, Kelembaban: ");
  Serial.print(h);
  Serial.print("%. Cahaya: ");
  Serial.print(ldrValue);
  Serial.print(", Jarak: ");
  Serial.print(distance);
  Serial.println(" cm. Dikirim ke Blynk.");
  delay(1000);
}
