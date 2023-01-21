/*#############################  SLAVE_SICAKLIK_PID#################### */
#include <Wire.h>
#include <DallasTemperature.h>
#include <RBDdimmer.h>
#include <OneWire.h>
#define outputPin 11 
#define zerocross 2
unsigned long lastTime1;
const byte viyol=9;
String sicaklikString;
  char sicaklikChar[5];
const byte nozul=7;
const byte fan220=10;
const byte fan24=8;
OneWire oneWire(6);
DallasTemperature DS18B20(&oneWire);
DeviceAddress DS18B20adres;
bool isActive=false,ok=false,calis;
float C;
int say = 0,say2;
unsigned long haha,hahaha;
const float  kp=40 ;         //Mine was 2.5
const float  ki=0.8;        //Mine was 0.06
const float  kd=3;         //Mine was 0.8
static float PID_p, PID_i, PID_d, PID_total;      //We will store here the real temp 
float  Setpoint = 36.6;    //In degrees C
const float  SetpointDiff = 30;
const float  refresh_rate = 100;                   //PID loop time in ms
float now_pid_error, prev_pid_error;
unsigned long elapsedTime, now_time, prev_time,b,x,a;
dimmerLamp dimmer(outputPin);
void(* resetFunc) (void) = 0;
void setup() {
  Serial.begin(9600);
  Wire.begin(0x91);
  Wire.onRequest(wireSend);
  Wire.onReceive(wireGet);

  dimmer.begin(NORMAL_MODE, ON);
  DS18B20.begin();
  DS18B20.getAddress(DS18B20adres, 0);
  
 /* for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (DS18B20adres[i] < 0x10) Serial.print("0");
    Serial.print(DS18B20adres[i], HEX);
    if (i < 7) Serial.print(", ");
  }*/
  DS18B20.setResolution(DS18B20adres, 9);
  pinMode(viyol,OUTPUT);
  pinMode(nozul,OUTPUT);
  pinMode(fan220,OUTPUT);
  pinMode(fan24,OUTPUT);
  digitalWrite(fan24,HIGH);
  digitalWrite(nozul,HIGH);
}

void loop() {
if(isActive){
  PID_control();
  }
  else delay(10
  );
  
}
void PID_control(void){
  elapsedTime = millis() - prev_time;   
  if(elapsedTime > refresh_rate){
    DS18B20.requestTemperatures();
      C=DS18B20.getTempCByIndex(0);
    now_pid_error = Setpoint - C;
    PID_p = kp * now_pid_error;
    PID_d = kd*((now_pid_error - prev_pid_error)/refresh_rate);
    if(-3 < now_pid_error && now_pid_error < 3){
      PID_i = PID_i + (ki * now_pid_error);}
    else{PID_i = 0;}

    PID_total = PID_p + PID_i + PID_d;  
    PID_total = map(PID_total, 0, 150,0 , 100  );
    
    
    if(PID_total <= 0){PID_total = 0;}
    if(PID_total >= 100) {PID_total =100; } 
   //Serial.print(PID_total);
   // Serial.print(C);       
  
  
    sicaklikString="";
    sicaklikString += C;
    sicaklikString.toCharArray(sicaklikChar,5);
    
    prev_time = millis();                       
    prev_pid_error = now_pid_error;
  }
}
void wireGet(int veri)
{
  char getChar;
    getChar = Wire.read();
  if(getChar == 'C')
   {String sicaklik;
    while(Wire.available()){
      char c = Wire.read();
    sicaklik += c;
      }
      Setpoint=sicaklik.toFloat();
     digitalWrite(fan220,HIGH);
     isActive=true;
    }
  else if(getChar == 'D')
   {
    digitalWrite(viyol,LOW);
    digitalWrite(nozul,HIGH);
    digitalWrite(fan220,LOW);
    digitalWrite(fan24,HIGH);
    isActive=false;
    resetFunc();
    }
    else if(getChar=='Y'){
      digitalWrite(viyol,LOW);
    }
    else if(getChar=='N'){
      digitalWrite(nozul,LOW);
      digitalWrite(fan24,LOW);
    }
    else if(getChar=='M'){
      digitalWrite(nozul,HIGH);
      digitalWrite(fan24,HIGH);
    }
   else if(getChar == 'V'){
    digitalWrite(viyol,HIGH);
    }
    else if(getChar=='P'){
      String sicaklik;
    while(Wire.available()){
      char c = Wire.read();
    sicaklik += c;
      }
      Setpoint=sicaklik.toFloat();
      isActive=true;
      }
      
}
 
void wireSend()
{
  Wire.write(sicaklikChar); 
}
void yakamk(){
      dimmer.setPower(PID_total);
  
  }
