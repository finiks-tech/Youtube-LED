#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define timer0_preload 40161290

byte sec = 0, mint = 0, hour = 0;
unsigned long day = 0;
double subs=0.0, views=0.0;
unsigned long lastTime = 0, lastTime2 = 0;
bool isSub = true;

MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, D7, D5, D8, 4);
//MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DIN, CLK, CS, MAX_DEVICES);

void inline interrupt_timer (void){//timer metod of Timer :: this metod Interrupt Every second
    sec++;                         //in timer test shode roye nodemcu V1 ESP8266 hast va faghat roy bord hay ESP8266 javab go hast
    if(sec == 60){                 //be dalil motefavet bodan memary 8266 ba Arduino bray estefade as interrupt timer roye arduino bayad as library TimerOne.h estefade shavad
        sec = 0;
        mint++;
      }
     if(mint == 60){
        mint = 0;
        hour++;
      }
      if(hour == 24){
        hour = 0;
        day++;
      }
        
        lastTime++;//increase 1sec in to lastTime
        lastTime2++;//increase 1sec in to lastTime2
        
    Serial.print("secend=");
    Serial.println(sec);
    Serial.print("mint=");
    Serial.println(mint);
    Serial.print("hour=");
    Serial.println(hour);
    Serial.print("day=");
    Serial.println(day);
    timer0_write(ESP.getCycleCount()+timer0_preload*2); //set time next Interrupt
    }

void setup() {
  Serial.begin(9600);
  myDisplay.begin();
  myDisplay.setIntensity(0); // 0-15
  myDisplay.setTextAlignment(PA_CENTER);

  WiFi.begin("NAME","PASSWORD");
  WiFi.mode(WIFI_STA);
  noInterrupts(); 
  timer0_isr_init();  //begin timer0
  timer0_attachInterrupt(interrupt_timer); //set metod Interrupt
  timer0_write(ESP.getCycleCount()+timer0_preload*2); // set time next Interrupt
  interrupts();
  
}


void getData(){
	HTTPClient http; 
	WiFiClientSecure client;
	client.setInsecure();
	client.connect("www.googleapis.com", 443);
	http.begin(client, "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=CHANNLEID&key=APIKEY");

	if (http.GET() == 200){
		DynamicJsonBuffer jsonBuffer(500);
		JsonObject& root = jsonBuffer.parseObject(http.getString());
		subs = ((int) root["items"][0]["statistics"]["subscriberCount"])/1000.0;
		views = ((int) root["items"][0]["statistics"]["viewCount"])/1000.0;
		lastTime = 0;

		myDisplay.print(String(subs)+"K");
	}
	http.end();
}

void loop() {
  if (lastTime > 60 || subs == 0){ // lastTime = second ,1mint == 60 ,5mint == 3600 ,10mint == 60000
    if (WiFi.status() == WL_CONNECTED) {
      getData(); 
    }
  } else {
    if (lastTime2 > 25){
      if (isSub){
        myDisplay.print(String(views)+"K");
      } else {
        myDisplay.print(String(subs)+"K");
      }
      isSub = !isSub;
      lastTime2 = 0;
    }
  }
}
