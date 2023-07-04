//Board esp8266 v3.0.2 

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "localserv";
const char* password = "05071999";
const char* CodeCountry = "SG";
int RelayPin = 0, VPNerr = 0, WIFIerr = 0, NotAccess = 0, ledNotif = 2, rebSTB = 0, TimeTry = 4, SelfTest = 0;

void setup () {
  WiFiClient client;
  HTTPClient http;

  //reset value after reboot from loop
  rebSTB = 0;
  VPNerr = 0;
  NotAccess = 0;
  
  pinMode(RelayPin, OUTPUT);
  pinMode(ledNotif, OUTPUT);
  
  digitalWrite(ledNotif, LOW);
  digitalWrite(RelayPin, HIGH);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  delay(5000);
  Serial.println("");
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    digitalWrite(ledNotif, LOW);
  }
  
  Serial.println("Terhubung");
  delay(2000);
  
  http.begin(client, "http://ifconfig.io/country_code");
  int httpCode = http.GET();

  //SelfTest
  if(SelfTest > 10){
    Serial.println("Reboot Router wait 5 second - SelfTest Not Success");
    digitalWrite(RelayPin, LOW);
    delay(5000);
    digitalWrite(RelayPin, HIGH);
    WIFIerr = 0;
    Serial.println("Done Reboot STB");
    SelfTest = 0;
    setup();
  }else{
    if (httpCode > 0) {
      String payload = http.getString();
      payload.trim();
      if(payload == CodeCountry){
        Serial.println("SelfTest OK");
        loop();
      }else{
        SelfTest++;
        setup();
      }
    }else{
      SelfTest++;
      setup();
    }
  }
  delay(2000);
}

void loop() {
WiFiClient client;
HTTPClient http;
  
if (WiFi.status() == WL_CONNECTED) {
  WIFIerr = 0;
  
  if(rebSTB == 2){
    Serial.println("Reboot Router wait 5 second - VPN Not Connect");
    digitalWrite(RelayPin, LOW);
    delay(5000);
    digitalWrite(RelayPin, HIGH);
    Serial.println("Done Reboot STB");
    rebSTB = 0;
    setup();
  }else{
     http.begin(client, "http://ifconfig.io/country_code");
     int httpCode = http.GET();
     
     if (httpCode > 0) {
      String payload = http.getString();
      payload.trim();
      
      http.begin(client, "http://github.githubassets.com/images/modules/profile/badge--acv-64.png");
      int httpCodeGit = http.GET();
      
      if(payload == CodeCountry && httpCodeGit > 0){
        Serial.println("Normal Mode");
        digitalWrite(ledNotif, HIGH);
        delay(1000);
        digitalWrite(ledNotif, LOW);
        
        //resetting value
        rebSTB = 0;
        VPNerr = 0;
        NotAccess = 0;
      }else{
        VPNerr++;
        digitalWrite(ledNotif, LOW);
        if(VPNerr > TimeTry){
          http.begin(client, "http://192.168.1.1/xderm/reconnect-xderm.php");
          http.GET();
          VPNerr = 0;
          rebSTB++;
          Serial.println("Reset VPNerr, Done Recoonect Xderm...");
        }
        Serial.println("Internet found, VPN Not Connect " + String(VPNerr));
      }
    }else{
      NotAccess++;
      digitalWrite(ledNotif, LOW);
      if(NotAccess > TimeTry){
        http.begin(client, "http://192.168.1.1/xderm/reconnect-xderm.php");
        http.GET();
        NotAccess = 0;
        rebSTB++;
        Serial.println("Reset NotAccess, Done Recoonect Xderm...");
      }
      Serial.println("Modem not found or ifconfig.io cannot access " + String(NotAccess));
    }
    http.end();
   }
}else{
    digitalWrite(ledNotif, LOW);
    WIFIerr++;
    if(WIFIerr > TimeTry){
      Serial.println("Reboot Router wait 5 second - WiFi Not Connect");
      digitalWrite(RelayPin, LOW);
      delay(5000);
      digitalWrite(RelayPin, HIGH);
      WIFIerr = 0;
      Serial.println("Done Reboot STB");
      setup();
    }
    Serial.println("WiFi Disconnect " + String(WIFIerr));
  }
delay(2000);
}
