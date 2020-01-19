#include <Keypad.h>
#include <EEPROM.h>
#include <DHT.h>
#include <LiquidCrystal.h>
#include <DS1307RTC.h> 
#include <Wire.h>
#include <Time.h>
#include <Servo.h>

//setting & global variable
LiquidCrystal lcd(A0,A1,A2,A3,A4,A5,A6);
tmElements_t tm;
#define DHTPIN 13
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
int pDioda,i;
int suhu, kelembabpan;
int suhu2, kelembabpan2; 
int pinBuzzer = 8;
int pinLed = 9;
int pinLed2 = 10;
int pinFan = 11;
int pos = 0;
int pinButton1 = A8, stateButton1 = 0;
int pinButton2 = A9, stateButton2 = 0;
int pinButton3 = A10, stateButton3 = 0;
int pinButton4 = A11, stateButton4 = 0;
int alamatSuhu = 0, alamatKelembabpan = 1, alamatSaveSuhu = 3;
int alamatJadwal1 = 5, alamatJadwal2 = 6, alamatJadwal3 = 7;

//keypad
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[rows] = {6, 1, 2, 4}; //connect to the row pinouts of the keypad
byte colPins[cols] = {5, 7, 3}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

//variable keypad
boolean inputMode=true, var1=true;
int inputSuhu=0;
char kolomSuhu[5];

void setup() {
  // put your setup code here, to run once: 
  lcd.begin(20,4);
  dht.begin();
  //Serial.begin(9600);
  myservo.attach(A8);
  pinMode(A7,INPUT);
  pinMode(pinBuzzer,OUTPUT);
  pinMode(pinLed,OUTPUT);
  pinMode(pinLed2,OUTPUT);
  pinMode(pinFan,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(pinLed, 255);
  analogWrite(pinFan, 255);
  char key;
  key = bacaKey();
  if(key){
    if(key == '*'){ // reset key '#'
      inputSuhu=0;
      inputMode=var1=true;
      lcd.setCursor(0,0);
      lcd.print("     INTEL ATOM     ");
      lcd.setCursor(0,1);
      bacaRTC();
      bacaSensor();
      tampilSensor();
      cekJadwalPutar();
      cekTetas();
    }
    
    //enter key '*'
    if((key == '#') && (key != '*')){
      if((inputMode == true) && (var1 == false)){
        lcd.clear();
        inputMode = false;
      }
      else if((inputMode == false)){
        lcd.setCursor(0,0);
        lcd.print("Input Suhu : ");
        sscanf(kolomSuhu, "%d",&inputSuhu);
        saveInput();
        lcd.print(bacaInputUser());
      }
    }
    
    //input suhu (1-9)
    if((key != '#') && (key != '*')){
      if((var1 == true)){
        kolomSuhu[0] = key;
        var1 = false;
      } else if(var1==false){
        kolomSuhu[1] = key;
      }
    }
  }
  kontrolSuhu(suhu);
  kontrolKelembabpan(kelembabpan);  
}

void bacaRTC(){
  if(RTC.read(tm)){
    cetak2tm(tm.Hour);
    lcd.print(":");
    cetak2tm(tm.Minute);
    lcd.print(":");
    cetak2tm(tm.Second);
    lcd.print(", ");
    cetak2tm(tm.Day);
    lcd.print(":");
    cetak2tm(tm.Month);
    lcd.print(":");
    lcd.print(tmYearToCalendar(tm.Year));
    }else {
      if (RTC.chipPresent()) {
        lcd.setCursor(0,1);
        lcd.print("RTC ada, tapi error!");
      } else {
        lcd.setCursor(0,1);
        lcd.print(" RTC Tidak terbaca!");
      }
      //delay(1000);
    }
    //delay(1000);
}

void cetak2tm(int nomor) {
   if (nomor >= 0 && nomor < 10) {
     lcd.write('0');
   }
   lcd.print(nomor);
}

void bacaSensor(){
  suhu = dht.readTemperature(); 
  kelembabpan = dht.readHumidity();
  EEPROM.write(alamatSuhu, suhu);
  EEPROM.write(alamatKelembabpan, kelembabpan);
}

void tampilSensor(){
  suhu2 = EEPROM.read(alamatSuhu);
  kelembabpan2 = EEPROM.read(alamatKelembabpan);
  lcd.setCursor(0,2);
  lcd.print("Suhu        : ");
  lcd.print(suhu2);
  lcd.print(" 'C");
  lcd.setCursor(0,3);
  lcd.print("Kelembabpan : ");
  lcd.print(kelembabpan2);
  lcd.print(" %");
}

void saveInput(){
  EEPROM.write(alamatSaveSuhu, inputSuhu);
}

int bacaInputUser(){
  return EEPROM.read(alamatSaveSuhu);
}

char bacaKey(){
  char key = keypad.getKey();
  if (key){
      return key;
  }
}

void kontrolSuhu(int suhu){
  if(suhu == bacaInputUser()){
    analogWrite(pinLed, 125);
  }
  else if(suhu > bacaInputUser()){
    analogWrite(pinLed, 0); 
  }
  else if(suhu < bacaInputUser()){
    analogWrite(pinLed, 255); 
  }
}

void kontrolKelembabpan(int kelembabpan){
  if(kelembabpan == 70){
      analogWrite(pinFan, 127); 
    }
    else if(kelembabpan > 70){
      analogWrite(pinFan, 0); 
    }
    else if(kelembabpan < 70){
      analogWrite(pinFan, 225); 
    }
}

void simpanJadwal(int jadwalKe, int jadwal){
  if(jadwalKe == 1){
    EEPROM.write(alamatJadwal1, jadwal);
  }
  else if(jadwalKe == 2){
    EEPROM.write(alamatJadwal2, jadwal);
  }
  else if(jadwalKe == 3){
    EEPROM.write(alamatJadwal3, jadwal);
  }
}

void tones(){
  digitalWrite(pinBuzzer, HIGH);
  delay(200);
  digitalWrite(pinBuzzer, LOW);
  delay(200);
}

void cekTetas(){
  pDioda = analogRead(A7);
  //Serial.println(pDioda);
  if(pDioda <= 50){
    for(i=0;i<5;i++) tones();
  }
}

void cekJadwalPutar(){
  if(RTC.read(tm)){
    if((tm.Second == 1) || (tm.Second == 48)){
      putarTelurPlus();
    }
    if((tm.Second == 20) || (tm.Second == 55)){
      putarTelurMin();
    }
  }
}

void putarTelurPlus(){
  for(pos = 0; pos < 180; pos += 1){
  // pada posisi 1 derajat
  // memberitahu servo untuk pergi ke posisi  'pos'
  myservo.write(pos);
  // tunggu 15ms untuk pencapaian  posisi servo    
  delay(15);                  
 }
}

void putarTelurMin(){
  for(pos = 180; pos>=1; pos-=1){
  // memberitahu servo untuk pergi ke posisi  'pos'                                
  myservo.write(pos);                 
  // tunggu 15ms untuk pencapaian  posisi servo    
  delay(15);                        
 }
}

