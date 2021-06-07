// inisialisasi
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

// library yang digunakan 
#include <LiquidCrystal.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "esp32-hal-cpu.h"

// lcd pin re,e,d4,d5,d6,d7
LiquidCrystal lcd(23,22,21,19,18,5);
// custom character hexaedcimal
byte head[8]={0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,};
byte nozzle[8]={0x00,0x00,0x00,0x1F,0x1F,0x00,0x00,0x00,};
byte chain1[8]={0x07,0x08,0x10,0x10,0x10,0x10,0x08,0x07,};
byte chain2[8]={0x1F,0x00,0x0A,0x04,0x04,0x0A,0x00,0x1F,};
byte chain3[8]={0x1C,0x02,0x01,0x01,0x01,0x01,0x02,0x1C,};

// pin motor kiri
int motor1pin1 = 26;
int motor1pin2 = 27;
int motor1pwm = 14;
int motor1;
int map1motor1;
int map2motor1;
// pin motor 2 kanan
int motor2pin1 = 17;
int motor2pin2 = 16;
int motor2pwm = 4;
int motor2;
int map1motor2;
int map2motor2;

int sagemode;
int levelAngry;
int starter;

// virtual pin motor 
BLYNK_WRITE(V1){
  motor1 = param.asInt();
}
BLYNK_WRITE(V2){
  motor2 = param.asInt();
}

// virtual pin sagemode
BLYNK_WRITE(V0){
  sagemode = param.asInt();
}
// virtual pin starter
BLYNK_WRITE(V3){
  starter = param.asInt();
}
BLYNK_WRITE(V5){
  levelAngry = param.asInt();
}

const int freq = 30000;
const int resolution = 10;
const int Mchannel1 = 0;
const int Mchannel2 = 1;
int awalMotor = 700;

// kode authentikasi blynk
char auth[] = "d1qSs5mwRGMzR12E7VtgNwizxXI5NMlU";

void setup() {
  // put your setup code here, to run once:
  // inisialisasi kecepatan komunikasi serial
  Serial.begin(9600);
  //inisialisasi lcd
  lcd.begin(16,2);
  
  // set cpu speed
  setCpuFrequencyMhz(240);

  // set output motor pin
  pinMode(motor1pin1,OUTPUT);
  pinMode(motor1pin2,OUTPUT);
  pinMode(motor2pin1,OUTPUT);
  pinMode(motor2pin2,OUTPUT);

  pinMode(motor1pwm, OUTPUT);
  pinMode(motor2pwm, OUTPUT);
  // setup pwm
  ledcSetup(Mchannel1, freq, resolution);
  ledcSetup(Mchannel2, freq, resolution);
  // pin pwm diisi data channel
  ledcAttachPin(motor1pwm, Mchannel1);
  ledcAttachPin(motor2pwm, Mchannel2);

  // debug console
  Serial.println("Menunggu Terkoneksi...");
  Blynk.setDeviceName("Undercariage");
  Blynk.begin(auth);
  // membuat character dari array
  lcd.createChar(3,head);
  lcd.createChar(4,nozzle);
  lcd.createChar(5,chain1);
  lcd.createChar(6,chain2);
  lcd.createChar(7,chain3);
  
  lcd.setCursor(4,0);
  lcd.print("POWER ON");
  lcd.setCursor(2,1);
  lcd.print("READING DATA");
  delay(1500);
  lcd.clear();
  
  // memanggil fungsi tank
  tank();
  delay(700);
  // geser display saat ini
  for (int posisi = 0; posisi < 10; posisi++){
    lcd.scrollDisplayRight();
    delay(500);
  }
  delay(700);
  lcd.clear();
  // memanggil fungsi cekCPU
  cekCPU();
  // memanggil fungsi tesmotor
  tesmotor();
}

/*fungsi untuk cek jenis kontroller
  dan CPU speed*/
void cekCPU() {
  lcd.setCursor(1,0);
  lcd.print("Use BLYNK ctrl");
  lcd.setCursor(0,1);
  lcd.print("CPU speed ");
  lcd.print(getCpuFrequencyMhz());
  lcd.print("Mhz");
}

// fungsi untuk character tank
void tank() {
  lcd.setCursor(1,0);
  lcd.write(3);
  lcd.write(3);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
  lcd.setCursor(0,1);
  lcd.write(5);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(6);
  lcd.write(7);  
}

// fungsi untuk tes motor
void tesmotor() {
  digitalWrite(motor1pin1,HIGH);
  digitalWrite(motor1pin2,LOW);
  ledcWrite(Mchannel1, awalMotor);
  digitalWrite(motor2pin1,HIGH);
  digitalWrite(motor2pin2,LOW);
  ledcWrite(Mchannel2, awalMotor);
  delay(300);
  digitalWrite(motor1pin1,LOW);
  digitalWrite(motor1pin2,LOW);
  digitalWrite(motor2pin1,LOW);
  digitalWrite(motor2pin2,LOW);
  delay(1000);
  digitalWrite(motor1pin1,LOW);
  digitalWrite(motor1pin2,HIGH);
  ledcWrite(Mchannel1, awalMotor);
  digitalWrite(motor2pin1,LOW);
  digitalWrite(motor2pin2,HIGH);
  ledcWrite(Mchannel2, awalMotor);
  delay(300);
  digitalWrite(motor1pin1,LOW);
  digitalWrite(motor1pin2,LOW);
  digitalWrite(motor2pin1,LOW);
  digitalWrite(motor2pin2,LOW);
}

// fungsi untuk control motor
void motor1Control(){
  // klik tombol starter
  if(starter == 1){
    // sagemode enable
    if(sagemode == 1){
      // mapping value motor 1
      map1motor1 = map(motor1,1024,2048,0,levelAngry);
      map2motor1 = map(motor1,1024,0,0,levelAngry);
      
      // motor 1 maju
      if(motor1 > 1024) {
        digitalWrite(motor1pin1,HIGH);
        digitalWrite(motor1pin2,LOW);
        ledcWrite(Mchannel1, map1motor1);
      }
      // motor 1 mundur
      else if(motor1 < 1024){
        digitalWrite(motor1pin1,LOW);
        digitalWrite(motor1pin2,HIGH);
        ledcWrite(Mchannel1, map2motor1);
      }
      // jika analog ditengah
      else {
        digitalWrite(motor1pin1,LOW);
        digitalWrite(motor1pin2,LOW);
      }
    }
    // sagemode disable
    else if(sagemode == 0){
      // mapping value motor 1
      map1motor1 = map(motor1,1024,2048,0,576);
      map2motor1 = map(motor1,1024,0,0,576);

      // motor 1 maju
      if(motor1 > 1024) {
        digitalWrite(motor1pin1,HIGH);
        digitalWrite(motor1pin2,LOW);
        ledcWrite(Mchannel1, map1motor1);
      }
      // motor 1 mundur
      else if(motor1 < 1024){
        digitalWrite(motor1pin1,LOW);
        digitalWrite(motor1pin2,HIGH);
        ledcWrite(Mchannel1, map2motor1);
      }
      // jika analog ditengah
      else {
        digitalWrite(motor1pin1,LOW);
        digitalWrite(motor1pin2,LOW);
      }
    }
  }
  // starter mati
  else {
    digitalWrite(motor1pin1,LOW);
    digitalWrite(motor1pin2,LOW);
  }
}
void motor2Control(){
  // klik tombol starter
  if(starter == 1){
    // sagemode enable
    if(sagemode == 1){
      // mapping value motor 2
      map1motor2 = map(motor2,1024,2048,0,levelAngry);
      map2motor2 = map(motor2,1024,0,0,levelAngry);

      // motor 2 maju
      if(motor2 > 1024){
        digitalWrite(motor2pin1,HIGH);
        digitalWrite(motor2pin2,LOW);
        ledcWrite(Mchannel2,map1motor2);
      }
      // motor 2 mundur
      else if(motor2 < 1024){
        digitalWrite(motor2pin1,LOW);
        digitalWrite(motor2pin2,HIGH);
        ledcWrite(Mchannel2, map2motor2);
      }
      // jika analog ditengah
      else {
        digitalWrite(motor2pin1,LOW);
        digitalWrite(motor2pin2,LOW);
      }
    }
    // sagemode disable
    else if(sagemode == 0){
      // mapping value motor 2
      map1motor2 = map(motor2,1024,2048,0,576);
      map2motor2 = map(motor2,1024,0,0,576);
      // motor 2 maju
      if(motor2 > 1024){
        digitalWrite(motor2pin1,HIGH);
        digitalWrite(motor2pin2,LOW);
        ledcWrite(Mchannel2, map1motor2);
      }
      // motor 2 mundur
      else if(motor2 < 1024){
        digitalWrite(motor2pin1,LOW);
        digitalWrite(motor2pin2,HIGH);
        ledcWrite(Mchannel2, map2motor2);
      }
      // jika analog ditengah
      else {
        digitalWrite(motor2pin1,LOW);
        digitalWrite(motor2pin2,LOW);
      }
    }
  }
  // starter mati
  else {
    digitalWrite(motor2pin1,LOW);
    digitalWrite(motor2pin2,LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  // memanggil fungsi motorcontrol
  motor1Control();
  motor2Control();
}
