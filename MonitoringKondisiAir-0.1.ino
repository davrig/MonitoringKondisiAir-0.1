/////////////////////////////////////////////////////////////////////////////////////////////////////
///  Project : Monitoring Kualitas Air
///  Created : Desember 2016
///  Name  : David Rigan
///  Email : dr@kreator.id
///  Web   : kreator.id
///  Refrence : https://www.hackster.io/eani/water-quality-monitoring-and-notification-system-f85d23
/////////////////////////////////////////////////////////////////////////////////////////////////////


#include <OneWire.h> //library untuk DS18B20
//#include <DallasTemperature.h> //library untuk DS18B20
#include <SoftwareSerial.h> //library untuk GSM modul
#include <NewPing.h> //library untuk HC-SR04

#define SensorPin A0         //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00          //deviation compensate
unsigned long int avgValue;  //Store the average value of the sensor feedback

#define TRIGGER_PIN   9 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN      10 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

#define ONE_WIRE_BUS 6

SoftwareSerial mySerial(7, 8); //PIN GSM Module

OneWire ds(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 

#include <LiquidCrystal.h> // include the library code:
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sensorPin = A0;
int levredled = 22;
int levgreenled = 23;
int levblueled = 24;
int phredled = 25;
int phgreenled = 26;
int phblueled = 27;
int tempredled = 28;
int tempgreenled = 29;
int tempblueled = 30; 
int buzzer = 31;

long duration, cm;
float phValue;
float temperatureC;

//SETUP FIRST !!!!!
float tingbidang = 26.50;
float tmphigh = 50.00;
float tmplow = 10.00;
float phlow = 6.90;
float phhigh = 7.30;
float levwaterlow = 5.00;
float levwaterhigh = 15.00;

void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  pingTimer = millis(); // Start now.
  //sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  lcd.begin(16, 2);// set up the LCD's number of columns and rows:
  lcd.clear();

  pinMode(phblueled, OUTPUT);
  pinMode(phredled, OUTPUT);
  pinMode(phgreenled, OUTPUT);
  pinMode(tempblueled, OUTPUT);
  pinMode(tempredled, OUTPUT);
  pinMode(tempgreenled, OUTPUT);
  pinMode(levblueled, OUTPUT);
  pinMode(levredled, OUTPUT);
  pinMode(levgreenled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}

void loop() {
  //sensors.requestTemperatures(); // Send the command to get temperature
  //Serial.println(sensors.getTempCByIndex(0));
    int reading = analogRead(sensorPin);  

  // converting that reading to voltage, 
  float voltage = reading * 5.0;
  voltage /= 1024.0; 

  // now print out the temperature
  //float temperatureC = (voltage - 0.5) * 100 ;
  
  PH();
  digitalWrite(phblueled, LOW);
  digitalWrite(phredled, LOW);
  digitalWrite(phgreenled, LOW);
  temperature();
  digitalWrite(tempblueled, LOW);
  digitalWrite(tempredled, LOW);
  digitalWrite(tempgreenled, LOW);
  Water_level();
  digitalWrite(levblueled, LOW);
  digitalWrite(levredled, LOW);
  digitalWrite(levgreenled, LOW);
  send_sms();
  digitalWrite(phgreenled, LOW);
  digitalWrite(tempgreenled, LOW);
  digitalWrite(levgreenled, LOW); 
  delay(8000);
}

/////////////////// FUNGSI ////////////////////////////

void PH(){
  Serial.println(" ");
  lcd.clear( );
  digitalWrite(phblueled, LOW);
  digitalWrite(phredled, LOW);
  digitalWrite(phredled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("MEMBACA SENSOR");
  lcd.setCursor(1,1);
  lcd.print("DARI SENSOR PH");
  Serial.println("Membaca dari Sensor PH...");
  
  PHblink();
  
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue+Offset;                      //convert the millivolt into pH value
  
  //Serial.print("pH:"); 
  //Serial.print(phValue,2);
  //Serial.println(" ");

  if(phValue >= phhigh){
    lcd.clear();
    digitalWrite(phblueled, LOW);
    digitalWrite(phredled, LOW);
    digitalWrite(phredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
    lcd.print("Nilai pH:");
    lcd.setCursor(10,0);
    lcd.print(phValue);
    lcd.setCursor(0,1);
    Serial.print("Nilai pH: ");
    Serial.println(phValue); 
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("KEBASAAN TINGGI");
    Serial.println("Tingkat BASA Air Tinggi");
    delay(3000);
   }

  if(phValue >= phlow && phValue <= phhigh){
    digitalWrite(phblueled, LOW);
    digitalWrite(phredled,LOW);
    digitalWrite(phgreenled, HIGH);
    digitalWrite(buzzer, LOW);
    lcd.clear();
    lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
    lcd.print("Nilai pH:");
    lcd.setCursor(10,0);
    lcd.print(phValue);
    lcd.setCursor(0,1);
    Serial.print("Nilai pH: ");
    Serial.println(phValue); 
    lcd.setCursor(1,1);//set cursor (colum by row) indexing from 0
    lcd.print("Air Normal");
    Serial.println("Air Normal");
  }

  if(phValue < phlow){
    lcd.clear();
    digitalWrite(phblueled, LOW);
    digitalWrite(phredled, LOW);
    digitalWrite(phredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
    lcd.print("Nilai pH:");
    lcd.setCursor(10,0);
    lcd.print(phValue);
    lcd.setCursor(0,1);
    Serial.print("Nilai pH: ");
    Serial.println(phValue); 
    lcd.setCursor(2,1);//set cursor (colum by row) indexing from 0
    lcd.print("KEASAMAN TINGGI");
    Serial.println("Keasaman Tinggi");
    delay(3000);
   }
   
  delay(8000);
}

void temperature(){
  Serial.println(" ");
  digitalWrite(tempblueled, LOW);
  digitalWrite(tempgreenled, LOW);
  digitalWrite(tempredled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.clear( );
  lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
  lcd.print("MEMBACA SENSOR");
  lcd.setCursor(2,1);
  lcd.print("TEMPERATURE");
  Serial.println("Membaca dari sensor Temperature..");
  TEMPblink();
  temp_check_surr();
  //delay(4000);
  //temp_check_water();
}

void temp_check_surr(){
  digitalWrite(tempblueled, LOW);
  digitalWrite(tempgreenled, LOW);
  digitalWrite(tempredled, LOW);
  digitalWrite(buzzer, LOW);
  
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  //Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  ///Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  temperatureC = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  
  if(temperatureC > tmphigh){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    Serial.print("Temperature Air: ");
    Serial.print(temperatureC); 
    Serial.println(" *C");
    lcd.setCursor(3,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR PANAS");
    Serial.println("Temperature Air Panas.");
    delay(3000);
   }

  if(temperatureC >= tmplow && temperatureC <= tmphigh){
    digitalWrite(tempblueled, HIGH);
    digitalWrite(tempgreenled,LOW);
    digitalWrite(tempredled, LOW);
    digitalWrite(buzzer, LOW);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Temperature Air: ");
    Serial.print(temperatureC); 
    Serial.println(" *C");
    lcd.setCursor(2,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR NORMAL");
    Serial.println("Temperature Air Normal.");
  }

  if(temperatureC < tmplow){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Temperature Air: ");
    Serial.print(temperatureC); 
    Serial.println(" *C");
    lcd.setCursor(3,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR DINGIN");
    Serial.println("Temperature Air Dingin.");
    delay(3000);
   }
  delay(8000);
}
  /*
  int reading = analogRead(sensorPin);  

  // converting that reading to voltage, 
  float voltage = reading * 5.0;
  voltage /= 1024.0; 

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  lcd.clear();
  Serial.print("Surrounding Temperature: ");
  Serial.println(temperatureC);

  if(temperatureC > 50){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("SUR TEMP:");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC); 
    Serial.println(" degree C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("SURR TEMP HIGH");
    Serial.println("Surrounding Temperature high");
    delay(3000);
   }

  if(temperatureC >= 10 && temperatureC <= 50){
    digitalWrite(tempblueled, HIGH);
    digitalWrite(tempgreenled,LOW);
    digitalWrite(tempredled, LOW);
    digitalWrite(buzzer, LOW);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("SUR TEMP:");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC); 
    Serial.println(" degree C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("SURR TEMP NORMAL");
    Serial.println("Surrounding Temperature normal");
  }

  if(temperatureC < 10){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("SUR TEMP:");
    lcd.setCursor(9,0);
    lcd.print(temperatureC);
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC); 
    Serial.println(" degree C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("SURR TEMP LOW");
    Serial.println("Surrounding Temperature low");
    delay(3000);
   }
  delay(8000);


void temp_check_water(){
  lcd.clear();
  digitalWrite(tempblueled, LOW);
  digitalWrite(tempgreenled, LOW);
  digitalWrite(tempredled, LOW);
  digitalWrite(buzzer, LOW);
  //sensors.requestTemperatures(); // Send the command to get temperature
  Serial.print("Temperature Air: ");
  Serial.println(sensors.getTempCByIndex(0));

  if(sensors.getTempCByIndex(0) > 40){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR:");
    lcd.setCursor(9,0);
    lcd.print(sensors.getTempCByIndex(0));
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Temperature Air: ");
    Serial.print(sensors.getTempCByIndex(0)); 
    Serial.println(" C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR TINGGI");
    Serial.println("Temperature air tinggi");
    delay(3000);
  }

  if(sensors.getTempCByIndex(0) >= 15 && sensors.getTempCByIndex(0) <= 40){
    digitalWrite(tempblueled, HIGH);
    digitalWrite(tempgreenled,LOW);
    digitalWrite(tempredled, LOW);
    digitalWrite(buzzer, LOW);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR:");
    lcd.setCursor(9,0);
    lcd.print(sensors.getTempCByIndex(0));
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Temperature Air: ");
    Serial.print(sensors.getTempCByIndex(0)); 
    Serial.println(" C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR NORMAL");
    Serial.println("Temperature Air normal");
  }

  if(sensors.getTempCByIndex(0) < 15){
    digitalWrite(tempblueled, LOW);
    digitalWrite(tempgreenled, LOW);
    digitalWrite(tempredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR:");
    lcd.setCursor(9,0);
    lcd.print(sensors.getTempCByIndex(0));
    lcd.setCursor(14,0);
    lcd.print("*C");
    lcd.setCursor(0,1);
    Serial.print("Temperature Air: ");
    Serial.print(sensors.getTempCByIndex(0)); 
    Serial.println(" C");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP AIR RENDAH");
    Serial.println("Temperature Air Rendah");
    delay(3000);
  }
  delay(8000);
}*/

void Water_level() {
  Serial.println(" ");
  digitalWrite(levblueled, LOW);
  digitalWrite(levgreenled, LOW);
  digitalWrite(levredled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.clear( );
  lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
  lcd.print("MEMBACA SENSOR");
  lcd.setCursor(1,1);
  lcd.print("KETINGGIAN AIR");
  Serial.println("Mengambil data ketinggian air");
  LEVblink();
  level_check();
  delay(5000);
}

void level_check(){
  digitalWrite(levblueled, LOW);
  digitalWrite(levgreenled, LOW);
  digitalWrite(levredled, LOW);
  digitalWrite(buzzer, LOW);
  
    // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
//delay(1000);  
}
}

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
   if(sonar.ping_result / US_ROUNDTRIP_CM > levwaterhigh){
    digitalWrite(levblueled, LOW);
    digitalWrite(levgreenled, LOW);
    digitalWrite(levredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.clear( );
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TINGGI AIR:");
    lcd.setCursor(12,0);
    lcd.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM));
    lcd.setCursor(14,0);
    lcd.print("cm");
    Serial.print("Tinggi Air: ");
    Serial.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM)); 
    Serial.println("cm");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR RENDAH");
    Serial.println("Tinggi Air RENDAH");
    delay(3000);
  }

  if(sonar.ping_result / US_ROUNDTRIP_CM >= levwaterlow && sonar.ping_result / US_ROUNDTRIP_CM <= levwaterhigh){
    digitalWrite(levblueled, LOW);
    digitalWrite(levgreenled, HIGH);
    digitalWrite(levredled, LOW);
    digitalWrite(buzzer, LOW);
    lcd.clear( );
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TINGGI AIR:");
    lcd.setCursor(12,0);
    lcd.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM));
    lcd.setCursor(14,0);
    lcd.print("cm");
    lcd.setCursor(0,1);
    Serial.print("TINGGI AIR: ");
    Serial.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM)); 
    Serial.println("cm");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR NORMAL");
    Serial.println("Tinggi Air NORMAL");
  }
  
  if(sonar.ping_result / US_ROUNDTRIP_CM < levwaterlow){
    digitalWrite(levblueled, LOW);
    digitalWrite(levgreenled, LOW);
    digitalWrite(levredled, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.clear( );
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("TINGGI AIR:");
    lcd.setCursor(12,0);
    lcd.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM));
    lcd.setCursor(14,0);
    lcd.print("cm");
    lcd.setCursor(0,1);
    Serial.print("TINGGI AIR: ");
    Serial.print(tingbidang - (sonar.ping_result / US_ROUNDTRIP_CM)); 
    Serial.println("cm");
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("AIR TINGGI");
    Serial.println("Tinggi Air TINGGI");
    delay(3000);
    }
  }
 }

 /////////Fungsi SMS///////////
 
void send_sms(){
  lcd.clear();
  Serial.println();
  lcd.setCursor(1,0);//set cursor (colum by row) indexing from 0
  lcd.print("MENUNGGU UNTUK");
  Serial.print("Menunggu Untuk ");
  lcd.setCursor(2,1);
  lcd.print("MENGIRIM SMS");
  Serial.print("Mengirim SMS.. ");
  Serial.println("\r");
  String temp; 
  String lev;
  String phm;
  
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue+Offset;                      //convert the millivolt into pH value
  /*
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  temperatureC = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  */
    if(temperatureC > tmphigh){
    temp = String("KONDISI AIR PANAS");
    }
    if(temperatureC >= tmplow && temperatureC <= tmphigh){
    temp = String("KONDISI AIR NORMAL");
    }
    if(temperatureC < tmplow){
     temp = String("KONDISI AIR DINGIN"); 
    }
       
    if(sonar.ping_result / US_ROUNDTRIP_CM > levwaterhigh){
    lev = String("KONDISI AIR RENDAH");
    }
    if(sonar.ping_result / US_ROUNDTRIP_CM >= levwaterlow && sonar.ping_result / US_ROUNDTRIP_CM <= levwaterhigh){
    lev = String("KONDISI AIR NORMAL");
    }
    if(sonar.ping_result / US_ROUNDTRIP_CM < levwaterlow){
    lev = String("KONDISI AIR TINGGI"); 
    }
    
    if(phValue >= phhigh){
    phm = String("KONDISI AIR BASA");
    }
    if(phValue >= phlow && phValue <= phhigh){
    phm = String("KONDISI AIR NORMAL");
    }
    if(phValue < phlow){
     phm = String("KONDISI AIR ASAM"); 
    }
    
  mySerial.begin(19200);  //Default serial port setting for the GPRS modem is 19200bps 8-N-1
  mySerial.print("\r");
  digitalWrite(buzzer, LOW);
  digitalWrite(phblueled, LOW);
  digitalWrite(phgreenled, LOW);
  digitalWrite(phredled, LOW);
  delay(1000);                    //wait for a second while the modem sends an "OK"
  mySerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(1000);
  mySerial.print("AT+CMGS=\"+628989664944\"\r");    //Start accepting the text for the message
  delay(1000);
  mySerial.print(temp);
  mySerial.print(" \r");
  mySerial.print("Temperature= ");   //The text for the message
  mySerial.print(temperatureC);
  mySerial.print("*C\r");
  mySerial.println("\r");
  mySerial.print(lev);
  mySerial.print(" \r");
  mySerial.print("Tinggi Air= ");   //The text for the message
  mySerial.print(sonar.ping_result / US_ROUNDTRIP_CM);
  mySerial.print("cm\r");
  mySerial.println("\r");
  mySerial.print(phm);
  mySerial.print(" \r");
  mySerial.print("pH Air= ");   //The text for the message
  mySerial.print(phValue);
  //mySerial.println("\r");
  
  digitalWrite(phgreenled, HIGH);
  digitalWrite(tempgreenled, HIGH);
  digitalWrite(levgreenled, HIGH);
  delay(3000);
  lcd.clear();
  lcd.setCursor(2,0);//set cursor (colum by row) indexing from 0
  lcd.print("SMS TERKIRIM");
  Serial.print("Sms Terkirim...");
  lcd.setCursor(4,1);
  lcd.print("BERHASIL");
  Serial.print("BERHASIL");
  Serial.println();
  mySerial.write(0x1A);  //Equivalent to sending Ctrl+Z 
}

/////////blink lampu/////////

void PHblink() {
  digitalWrite(phblueled, HIGH); 
  delay(1000);               
  digitalWrite(phblueled, LOW);
  delay(1000);              
  digitalWrite(phblueled, HIGH);  
  delay(1000); 
  digitalWrite(phblueled, LOW); 
  delay(1000); 
  digitalWrite(phblueled, HIGH); 
  delay(1000); 
  digitalWrite(phblueled, LOW);  
  delay(1000);
   digitalWrite(phblueled, HIGH); 
  delay(1000); 
  digitalWrite(phblueled, LOW);  
  delay(1000); 
}

void TEMPblink() {
  digitalWrite(tempblueled, HIGH); 
  delay(1000);               
  digitalWrite(tempblueled, LOW);
  delay(1000);              
  digitalWrite(tempblueled, HIGH);  
  delay(1000); 
  digitalWrite(tempblueled, LOW); 
  delay(1000); 
  digitalWrite(tempblueled, HIGH); 
  delay(1000); 
  digitalWrite(tempblueled, LOW);  
  delay(1000);
   digitalWrite(tempblueled, HIGH); 
  delay(1000); 
  digitalWrite(tempblueled, LOW);  
  delay(1000); 
}

 void LEVblink() {
  digitalWrite(levblueled, HIGH);
  delay(1000);               
  digitalWrite(levblueled, LOW);
  delay(1000);              
  digitalWrite(levblueled, HIGH);
  delay(1000); 
  digitalWrite(levblueled, LOW);
  delay(1000); 
  digitalWrite(levblueled, HIGH);
  delay(1000); 
  digitalWrite(levblueled, LOW);
  delay(1000);
  digitalWrite(levblueled, HIGH);
  delay(1000); 
  digitalWrite(levblueled, LOW);
  delay(1000); 
}
