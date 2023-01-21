#include <string.h>
#include "RTClib.h"
#include <OneWire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"
#define DHTPIN 3
#define FIREBASE_HOST "kulc-b073f-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "36rwK78tu7AKedcje0ndyqOs9l8g6fhvPKWDQI7n"
//#define WIFI_SSID "Redmi 9C"
//#define WIFI_PASSWORD "1234567890"
DHT dht(DHTPIN, DHT22);

RTC_DS1307 rtc;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
DateTime baslangicZamani;
DateTime bitisZamani;
byte say,donusSayac;
int issaniye;
String gSicaklik;
const byte artirButon=14;
const byte azaltButon=0;
const byte okButon=2;
const byte geriButon=12;
const byte kapaButon=13;
unsigned long eskiZaman,yeniZaman, lastTime1,lastTime2,lastTime3;
bool girdi,geriGirdi,geriCikti,bitti=true,isViyol,donuyor,calis_set,calis_set2,nozul,isCounted,a;
uint8_t art_azal=0;
uint8_t menuGez;
uint8_t zamanla;
float h;
char gunchar[20],aychar[5],yilchar[5],saatchar[5],dakikachar[5],igun[5],isaat[5],idk[5],isn[50],tarihchar[20];;
uint8_t sayac;
uint8_t testDegeri;
uint8_t anaMenu_var;
uint8_t baslatMenuGez;
int gun=15,ay=6,yil=2020,saat=12,dakika=30;
struct turler{
char isim[20];
};
struct calis{
float sicaklik;
uint8_t nem;
uint8_t gun;
uint8_t kacdk;
uint8_t cikis_gun;
uint8_t cikis_nem;
float cikis_isi;};
struct calis davuk={36,70,21,10,3,75,37};
struct calis gaz={36,70,21,10,3,75,37};
const char *menuDizi[] ={"tavuk","kaz","manual","Tarih Ayarla"};
const char *geriDizi[] ={"isi","nem","Kac dk","cikis isi","cikis nem","baslat"};
struct calis secim;
template<typename T>
void wireSend(T *Send, bool first=false,bool setall=false){
        if(!first && setall==false){
        Wire.beginTransmission(111);
        char a = *Send;
        Wire.write(a);
        Wire.endTransmission(111);
        }
        else if(first==true && setall==true){
          Wire.beginTransmission(111);
          String sicaklikString;
        char sicaklikChar[5];
        sicaklikString="";
         sicaklikString += *Send;
         sicaklikString.toCharArray(sicaklikChar,5);
          Wire.write("P");
          Wire.write(sicaklikChar);
          Wire.endTransmission(111);
          }
        else{
        Wire.beginTransmission(111);
        //char sicaklikString[6];
        //dtostrf(*Send,4,2,sicaklikString);
        String sicaklikString;
        char sicaklikChar[5];
        sicaklikString="";
         sicaklikString += *Send;
         sicaklikString.toCharArray(sicaklikChar,5);
        Wire.write("C");
        Wire.write(sicaklikChar);
        Wire.endTransmission(111);
        }
        
    }
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  Wire.begin();
  lcd.begin();
  lcd.backlight();
  lcd.print("Baslatiliyor");
   if (! rtc.begin()) {
    Serial.flush();
    while (1) delay(10);
  }
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 // WiFi.setAutoReconnect(false);
   epromOku();
 //  while (WiFi.status() != WL_CONNECTED) {
  //  Serial.print(".");
 //   delay(500);
//  }
 
 
  secim.kacdk=1;
   
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  

   dht.begin();
  
  pinMode(kapaButon, INPUT_PULLUP);
   pinMode(artirButon, INPUT_PULLUP);
  pinMode(artirButon, INPUT_PULLUP);
  pinMode(azaltButon, INPUT_PULLUP);
  pinMode(okButon, INPUT_PULLUP);
  pinMode(geriButon, INPUT_PULLUP);
  Serial.begin(9600);
  issaniye=Firebase.getInt("Makine1/Istenen/Issaniye");
  delay(2000);
  menuOlustur(0,menuDizi,100);
}

void loop() {
    
  if(bitti)
  {
    uzaktanUpdate();
  menudizinti((sizeof(menuDizi)/sizeof(menuDizi[0]))-1,menuDizi,(sizeof(menuDizi)/sizeof(menuDizi[0]))-1);
  }
  else
  baslat();
}

int butonBasma(int gelenDeger,const char gelenYazi[40],int sutun,int satir,const char ch[40]){
  
   lcd.clear();lcd.print(gelenYazi);lcd.setCursor(sutun,satir);lcd.print(gelenDeger);
   bool menuBasla=true;
  while(menuBasla){
  delay(5);
  while(!digitalRead(artirButon)||!digitalRead(azaltButon)){
    if(girdi!=1){
    if(!digitalRead(artirButon))art_azal=1;
   else if(!digitalRead(azaltButon))art_azal=4;
    }
    delay(50);
    sayac++;
    if((sayac>20) && !digitalRead(artirButon)){gelenDeger+=10; art_azal=3;girdi=1;sayac=0;if(!(strcmp(ch,"saat"))&& gelenDeger>=24 )gelenDeger=23; if(!(strcmp(ch,"dakika"))&& gelenDeger>59 )gelenDeger=59; if(!(strcmp(ch,"ay"))&& gelenDeger>12 )gelenDeger=12; if(!(strcmp(ch,"gun"))&& gelenDeger>30 && ay!=2){if(ay<=7 && ay%2==1)gelenDeger=31; else if(ay>=7 && ay%2==0)gelenDeger=31; else gelenDeger=30;} else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4!=0)gelenDeger=28; else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4==0)gelenDeger=29; if(gelenDeger<=0)gelenDeger=1;ekranaYazdir(gelenYazi,gelenDeger,sutun,satir);}
    else if(sayac>20&&!digitalRead(azaltButon)){gelenDeger-=10; art_azal=3;girdi=1;sayac=0;if(!(strcmp(ch,"saat"))&& gelenDeger>24 )gelenDeger=23; if(!(strcmp(ch,"dakika"))&& gelenDeger>59 )gelenDeger=59;if(!(strcmp(ch,"ay"))&& gelenDeger>12 )gelenDeger=12; if(!(strcmp(ch,"gun"))&& gelenDeger>30 && ay!=2){if(ay<=7 && ay%2==1)gelenDeger=31; else if(ay>=7 && ay%2==0)gelenDeger=31; else gelenDeger=30;} else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4!=0)gelenDeger=28; else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4==0)gelenDeger=29; if(gelenDeger<=0)gelenDeger=1; ekranaYazdir(gelenYazi,gelenDeger,sutun,satir);}
  }
  
  sayac=0;
  
  if((girdi ==0) &&(art_azal==1)){ art_azal=3;gelenDeger++; sayac=0;if(!(strcmp(ch,"ay"))&& gelenDeger>12 )gelenDeger=12; if(!(strcmp(ch,"saat"))&& gelenDeger>=24 )gelenDeger=23; if(!(strcmp(ch,"dakika"))&& gelenDeger>59 )gelenDeger=59;if(!(strcmp(ch,"gun"))&& gelenDeger>30 && ay!=2){if(ay<=7 && ay%2==1)gelenDeger=31; else if(ay>=7 && ay%2==0)gelenDeger=31; else gelenDeger=30;} else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4!=0)gelenDeger=28; else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4==0)gelenDeger=29; if(gelenDeger<=0)gelenDeger=1; ekranaYazdir(gelenYazi,gelenDeger,sutun,satir);}
  else if((girdi ==0)&&(art_azal==4)){art_azal=3;gelenDeger--; sayac=0;if(!(strcmp(ch,"ay"))&& gelenDeger>12 )gelenDeger=12; if(!(strcmp(ch,"saat"))&& gelenDeger>=24 )gelenDeger=23; if(!(strcmp(ch,"dakika"))&& gelenDeger>59 )gelenDeger=59;if(!(strcmp(ch,"gun"))&& gelenDeger>30 && ay!=2){if(ay<=7 && ay%2==1)gelenDeger=31; else if(ay>=7 && ay%2==0)gelenDeger=31; else gelenDeger=30;} else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4!=0)gelenDeger=28; else if(!(strcmp(ch,"gun"))&& gelenDeger>28 && ay==2 && yil%4==0)gelenDeger=29; if(gelenDeger<=0)gelenDeger=1;ekranaYazdir(gelenYazi,gelenDeger,sutun,satir);}
  else if(girdi==1)girdi=0;
  
  if(!digitalRead(okButon)){while(!digitalRead(okButon)){delay(5);} menuBasla==false; delay(200); return gelenDeger; }
  }
  
  return 0;
}

void ekranaYazdir(const char gelenYazi[30], int gelenDeger, int sutun, int satir){
  lcd.clear();
  lcd.print(gelenYazi);
  
  if(gelenDeger != 9090){lcd.setCursor(satir,sutun); lcd.print(gelenDeger);}
}

void menuOlustur(int i,const char *dizi[],int boyut){
if((i%2)==0){
lcd.clear();
lcd.print(">");
lcd.setCursor(1,0);
lcd.print(dizi[i]);
  if(boyut>=i+1){
    lcd.setCursor(0,1);
    lcd.print(dizi[i+1]);}
  }
  else{
lcd.clear();
lcd.print(dizi[i-1]);
lcd.setCursor(0,1);
lcd.print(">");
lcd.setCursor(1,1);
lcd.print(dizi[i]);
  }
}
void menudizinti(int menuGezMax,const char *dizi[],int boyut){
  if(!digitalRead(artirButon)){
   if(menuGezMax>menuGez)
   menuGez++;
   Serial.print(menuGez);
   delay(100);
   
   menuOlustur(menuGez,dizi,boyut);
   while(!digitalRead(artirButon)){delay(5);}
  }
  else if(!digitalRead(azaltButon)){
   if(menuGez>=1)
    menuGez--;
    delay(100);
    menuOlustur(menuGez,dizi,boyut);
    
   while(!digitalRead(azaltButon)){delay(5);}
  }
  else if(!digitalRead(okButon)){
    while(!digitalRead(okButon)){delay(5);}
      
      if(!(strcmp(dizi[menuGez],"manual"))){delay(200);manualMenu();}
      else if(!(strcmp(dizi[menuGez],"Tarih Ayarla"))){geriGirdi=false; delay(200);TarihAyarla(); delay(500); menuOlustur(menuGez,dizi,boyut); }
      else if(!(strcmp(dizi[menuGez],"baslat"))){geriGirdi=false; geriCikti=true; epromYaz(); baslat();}
      else if(!(strcmp(dizi[menuGez],"nem"))){geriGirdi=true;delay(200); secim.nem=butonBasma(secim.nem,"Nemi belirleyin",1,1,"nem");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"isi"))){geriGirdi=true;delay(200); secim.sicaklik=butonBasma(secim.sicaklik,"isi belirleyin",1,1,"isi");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"Kac dk"))){geriGirdi=true;delay(200); secim.kacdk=butonBasma(secim.kacdk,"Kac dk donus",1,1,"A");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"cikis isi"))){geriGirdi=true;delay(200); secim.cikis_isi=butonBasma(secim.cikis_isi,"cikis isisi",1,1,"A");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"cikis nem"))){geriGirdi=true;delay(200); secim.cikis_nem=butonBasma(secim.cikis_nem,"cikis nemi",1,1,"A");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"cikis nem"))){geriGirdi=true;delay(200); secim.cikis_gun=butonBasma(secim.cikis_gun,"cikis gunu",1,1,"A");menuOlustur(menuGez,dizi,boyut);}
      else if(!(strcmp(dizi[menuGez],"tavuk"))){{; secim=davuk; baslat();}}
      else if(!(strcmp(dizi[menuGez],"kaz"))){{; secim=gaz; baslat();}}
  }

}


void baslat(){

if(geriCikti==false&&bitti==true){
baslangicZamani=rtc.now();
bitisZamani= rtc.now()+ TimeSpan(secim.gun, 0, 0, 0);bitti=false; issaniye=bitisZamani.second(); epromYaz();}
wireSend(&secim.sicaklik,true);
uzaktanSet();
baslatMenuGez=1;
if((WiFi.status() == WL_CONNECTED)){
Firebase.setBool("Makine1/Gelen/isStarted",true);
Firebase.setInt("Makine1/Gelen/Gdurum",1);
}
calismaMenu();

  while(bitti==false){
    calismamenuGez(3);
    delay(20);
     if (millis() - lastTime1 > 1000)
  {
    delay(20);
    if(WiFi.status() == WL_CONNECTED){
    Firebase.setString("Makine1/Gelen/Gsicaklik",gSicaklik);
    Firebase.setFloat("Makine1/Gelen/Gnem", h);
    uzaktanUpdate();
    bool x= Firebase.getBool("Makine1/Gelen/isStarted");
     bool y = Firebase.getInt("Makine1/Gelen/Gdurum");
     bool error;
    if (Firebase.failed()) {
      error = true;
      return;
  }
   
    if(x==0 && error==false){
    menuGez=0;
    nozul=false;
    isViyol=false;
    donusSayac=0;
    secim.nem=1;
    isCounted=false;
    geriGirdi=false; geriCikti=false;
    bitti=true;
    wireSend("D");
    epromYaz();
    menuOlustur(menuGez,menuDizi,sizeof(menuDizi));
      }
   else if(y==0 && error==false){
      geriGirdi=true;
      nozul=false;
      wireSend("D");
      menuGez=0; menuOlustur(menuGez,geriDizi,sizeof(geriDizi)); while(geriGirdi){ 
        uzaktanBaslatma();
        menudizinti((sizeof(geriDizi)/sizeof(geriDizi[0]))-1,geriDizi,(sizeof(geriDizi)/sizeof(geriDizi[0]))-1); delay(5); }
      }
    }
    delay(20);
    say++;
    lastTime1 = millis();
    }

  delay(20);
    calisma(h,rtc.now().minute());
    if(say>=2&&baslatMenuGez!=2){say=0; if(bitisZamani.unixtime()<=rtc.now().unixtime()) {bitti=true;epromYaz();menuOlustur(menuGez,menuDizi,sizeof(menuDizi));} h = dht.readHumidity(); wireGet(gSicaklik); wireSend(&secim.sicaklik,true,true); if(bitti==false){calismaMenu();}}
    
    if(!digitalRead(geriButon)){geriGirdi=true; 
    nozul=false;
    wireSend("D");
    if(WiFi.status() == WL_CONNECTED){
    Firebase.setInt("Makine1/Gelen/Gdurum",0);
    }
    {while(!digitalRead(geriButon)){delay(5);}  menuGez=0; menuOlustur(menuGez,geriDizi,sizeof(geriDizi)); while(geriGirdi){
      menudizinti((sizeof(geriDizi)/sizeof(geriDizi[0]))-1,geriDizi,(sizeof(geriDizi)/sizeof(geriDizi[0]))-1); uzaktanBaslatma(); delay(5); }
      
      }
      delay(5);}
  }
}
void manualMenu(){

secim.nem=butonBasma(secim.nem,"Nemi belirleyin",1,1,"nem");
secim.sicaklik=butonBasma(secim.sicaklik,"isi belirleyin",1,1,"isi");
secim.gun=butonBasma(secim.gun,"Kac gun surecek",1,1,"gunsure");
secim.kacdk=butonBasma(secim.kacdk,"Kac saatte bir",1,1,"gunsure");
secim.cikis_isi=butonBasma(secim.cikis_isi,"cikis isisi",1,1,"gunsure");
secim.cikis_nem=butonBasma(secim.cikis_nem,"cikis nemi",1,1,"gunsure");
secim.cikis_gun=butonBasma(secim.cikis_gun,"cikis gunu",1,1,"gunsure");
bitti=true;
geriCikti=false;
baslat();
 }
void TarihAyarla(){
yil=butonBasma(yil,"Yili girin:",1,1,"yil");
ay=butonBasma(ay,"ay girin:",1,1,"ay");
gun=butonBasma(gun,"gun girin:",1,1,"gun");
saat=butonBasma(saat,"Saati girin:",1,1,"saat");
dakika=butonBasma(dakika,"Dakika girin:",1,1,"dakika");
sprintf(yilchar,"%d",yil);
sprintf(aychar,"%d",ay);
sprintf(gunchar,"%d",gun);
sprintf(saatchar,"%d",saat); 
sprintf(dakikachar,"%d",dakika); 

lcd.clear();
lcd.print(F("Tarih Ayarlandi"));
lcd.setCursor(0,1);
strcat(gunchar,"/");
strcat(gunchar,aychar);
strcat(gunchar,"/");
strcat(gunchar,yilchar);
strcat(gunchar,"|");
strcat(gunchar,saatchar);
strcat(gunchar,":");
strcat(gunchar,dakikachar);
lcd.print(gunchar);
strcpy(gunchar, "");
   while(true){
     if(!digitalRead(okButon)){
    while(!digitalRead(okButon)){delay(5);}
      
      DateTime gir (yil, ay, gun, saat, dakika, 0);
      rtc.adjust(gir);
      break;
     }
   }
 }
void calismaMenu(){
    if(baslatMenuGez==1){
      lcd.clear();
      lcd.print(F("iSic:"));
      lcd.setCursor(5,0);
      lcd.print(secim.sicaklik);
      lcd.setCursor(9,0);
      lcd.print(F("iNem:"));
      lcd.print((int)secim.nem);
      lcd.setCursor(0,1);
      lcd.print(F("GSic:"));
      lcd.setCursor(5,1);
      char *sicaklik= &gSicaklik[0];
      lcd.print(sicaklik);
      lcd.setCursor(9,1);
      lcd.print(F("GNem:"));
      lcd.print((int)h);
      delay(5);
      }
    else if(baslatMenuGez==2)
    {
      lcd.clear();
      lcd.print(F("Bas: "));
      sprintf(yilchar,"%d",baslangicZamani.year());
      sprintf(aychar,"%d",baslangicZamani.month());
      sprintf(gunchar,"%d",baslangicZamani.day());
      strcat(gunchar,"/");
      strcat(gunchar,aychar);
      strcat(gunchar,"/");
      strcat(gunchar,yilchar);
      lcd.print(gunchar);
      strcpy(gunchar, "");
      lcd.setCursor(0,1);
      lcd.print("Bit: ");
      sprintf(yilchar,"%d",bitisZamani.year());
      sprintf(aychar,"%d",bitisZamani.month());
      sprintf(gunchar,"%d",bitisZamani.day());
      strcat(gunchar,"/");
      strcat(gunchar,aychar);
      strcat(gunchar,"/");
      strcat(gunchar,yilchar);
      lcd.print(gunchar);
      strcpy(gunchar, "");
      delay(5);
      }
    else if(baslatMenuGez==3)
    { 
      lcd.clear();
      TimeSpan ts1 = bitisZamani - rtc.now();
      sprintf(igun,"%d",ts1.days());
      sprintf(isaat,"%d",ts1.hours());
      sprintf(idk,"%d",ts1.minutes());
      sprintf(isn,"%d",ts1.seconds());
      lcd.print(F("Bitis suresi"));
      lcd.setCursor(0,1);
      
      strcat(tarihchar,igun);
      strcat(tarihchar,":");
      strcat(tarihchar,isaat);
      strcat(tarihchar,":");
      strcat(tarihchar,idk);
      strcat(tarihchar,":");
      strcat(tarihchar,isn);
      lcd.print(tarihchar);
      strcpy(tarihchar, "");
      delay(5);
      }
    
    
  
  
  
  
  }
void calismamenuGez(int menuGezMax){
  if(!digitalRead(artirButon)){
    
   if(menuGezMax>baslatMenuGez)
   baslatMenuGez++;
   calismaMenu();
   while(!digitalRead(artirButon)){delay(5);}
   
  }
  else if(!digitalRead(azaltButon)){
   if(baslatMenuGez>1)
    baslatMenuGez--;
    calismaMenu();
   while(!digitalRead(azaltButon)){delay(5);}
   
  }
else if(!digitalRead(kapaButon)){
    menuGez=0;
    nozul=false;
    isViyol=false;
    donusSayac=0;
    secim.nem=0;
    isCounted=false;
    geriGirdi=false; geriCikti=false;
    bitti=true;
    if(WiFi.status() == WL_CONNECTED){
    Firebase.setBool("Makine1/Gelen/isStarted",false);
    Firebase.setInt("Makine1/Gelen/Gdurum",0);
    }
    wireSend("D");
    epromYaz();
    
    while(!digitalRead(kapaButon)){delay(5);}
    menuOlustur(menuGez,menuDizi,sizeof(menuDizi));
   
  }
  
  }
void epromYaz(){
  EEPROM.put(1,bitisZamani);
  EEPROM.put(30,bitti);
  EEPROM.put(70,secim);
  EEPROM.put(130,baslangicZamani);
  EEPROM.commit();
    }
 void epromOku(){
  delay(50);
    baslangicZamani= EEPROM.get(130,baslangicZamani);
   bitisZamani= EEPROM.get(1,bitisZamani);
   bitti= EEPROM.get(30,bitti);
  secim=EEPROM.get(70,secim);

  }
void calisma(float nem,int dk){
  if(millis()-lastTime3>600000)
  {
  delay(50);
  lastTime3=millis();
  TimeSpan tss = bitisZamani - rtc.now();
  if(tss.days()>0){
  TimeSpan ts2 = bitisZamani - rtc.now();
  if(ts2.days()<secim.cikis_gun&&!calis_set){
   // Setpoint=secim.cikis_isi;
    secim.sicaklik=secim.cikis_isi;
    secim.nem=secim.cikis_nem;
    calis_set=true;
    wireSend(&secim.sicaklik,true);
    }
  else if(!calis_set2){ //Setpoint=secim.sicaklik; 
    calis_set2=true;}
  }
  }
  
  
  if(!calis_set2){ //Setpoint=secim.sicaklik; 
    calis_set2=true;}
    if(calis_set||calis_set2){/*PIDCONTROL*/
    
    if(nem<secim.nem||nozul)
    { if(!nozul){
      wireSend("N");
      }
      nozul=true;
      
      if((nozul&&nem>=secim.nem+5) || nem>=100)
      {nozul=false; 
     wireSend("M");
      }
    }
    
    
    }
    

  
    if((dk+1)%20==0){
      if(!isCounted){
        donusSayac++;
        isCounted=true;
        yeniZaman=millis();
      }
      if(donusSayac>=secim.kacdk*3){
         isViyol=true;
        wireSend("V");
         eskiZaman=millis();
         if(eskiZaman-yeniZaman>=7000 && isViyol){
          yeniZaman=millis();
          isViyol=false;
          wireSend("Y");
          }
        }
    }
    else if(isCounted){
      isCounted=false;
      }
    
    }


    void wireGet(String &sicaklik){
      String gsicaklik;
      Wire.requestFrom(111, 4);
       while(Wire.available()){
        char c = Wire.read();
        gsicaklik += c;
  }
     sicaklik=gsicaklik;
    }

void uzaktanBaslatma(){
  if(WiFi.status() == WL_CONNECTED){
    if(millis()-lastTime2>1000){
      lastTime2=millis();
      bool a = Firebase.getBool("Makine1/Gelen/isStarted");
      bool b = Firebase.getInt("Makine1/Gelen/Gdurum");
       if(!a){
    menuGez=0;
    geriGirdi=false; geriCikti=false;
    bitti=true;
    wireSend("D");
    epromYaz();
    menuOlustur(menuGez,menuDizi,sizeof(menuDizi));
      }
    else if(b){
      geriGirdi=false;
      geriCikti=true;
      uzaktanSet();
      wireSend(&secim.sicaklik,true);
      epromYaz();
      }
      }
  }
}
void uzaktanUpdate(){
  if(WiFi.status() == WL_CONNECTED){
  if(Firebase.getInt("Makine1/Istenen/Issaniye")!=issaniye){
      issaniye=Firebase.getInt("Makine1/Istenen/Issaniye");
      bitisZamani = DateTime (Firebase.getInt("Makine1/Istenen/Isyil"), Firebase.getInt("Makine1/Istenen/Isay"), Firebase.getInt("Makine1/Istenen/Isgun"), Firebase.getInt("Makine1/Istenen/Issaat"), Firebase.getInt("Makine1/Istenen/Isdakika"), Firebase.getInt("Makine1/Istenen/Issaniye"));
      baslangicZamani = DateTime(Firebase.getInt("Makine1/Istenen/Ibyil"), Firebase.getInt("Makine1/Istenen/Ibay"), Firebase.getInt("Makine1/Istenen/Ibgun"), Firebase.getInt("Makine1/Istenen/Ibsaat"), Firebase.getInt("Makine1/Istenen/Ibdakika"), Firebase.getInt("Makine1/Istenen/Ibsaniye"));
      rtc.adjust(baslangicZamani);
      secim.nem= Firebase.getFloat("Makine1/Istenen/Inem");
      secim.sicaklik= Firebase.getFloat("Makine1/Istenen/Isicaklik");
      secim.kacdk= Firebase.getInt("Makine1/Istenen/Idk");
      secim.cikis_nem=Firebase.getInt("Makine1/Istenen/Icikisnem");
      secim.cikis_isi=Firebase.getInt("Makine1/Istenen/Icikissicaklik");
      secim.cikis_gun=Firebase.getInt("Makine1/Istenen/Icikisgun");
      bitti=false;
      epromYaz();
      }
  
  }}
void uzaktanSet(){
  if(WiFi.status() == WL_CONNECTED){
      Firebase.setInt("Makine1/Istenen/Isyil",bitisZamani.year());
      Firebase.setInt("Makine1/Istenen/Isay",bitisZamani.month());
      Firebase.setInt("Makine1/Istenen/Isgun",bitisZamani.day());
      Firebase.setInt("Makine1/Istenen/Issaat",bitisZamani.hour());
      Firebase.setInt("Makine1/Istenen/Isdakika",bitisZamani.minute());
      Firebase.setInt("Makine1/Istenen/Issaniye",bitisZamani.second());
      Firebase.setInt("Makine1/Istenen/Ibyil",baslangicZamani.year());
      Firebase.setInt("Makine1/Istenen/Ibay",baslangicZamani.month());
      Firebase.setInt("Makine1/Istenen/Ibgun",baslangicZamani.day());
      Firebase.setFloat("Makine1/Istenen/Inem",secim.nem);
      Firebase.setFloat("Makine1/Istenen/Isicaklik",secim.sicaklik);
      Firebase.setInt("Makine1/Istenen/Idk",secim.kacdk);
      Firebase.setFloat("Makine1/Istenen/Icikisnem",secim.cikis_nem);
      Firebase.setFloat("Makine1/Istenen/Icikissicaklik",secim.cikis_isi);
      Firebase.setFloat("Makine1/Istenen/Icikisgun",secim.cikis_gun);
  
  }}
  
  
 
