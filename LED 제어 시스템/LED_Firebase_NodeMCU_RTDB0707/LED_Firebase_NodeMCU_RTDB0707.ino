
#include <ESP8266WiFi.h>                                               
#include <FirebaseArduino.h>      
#define FIREBASE_HOST "fir-led-b5b0a.firebaseio.com"     
#define FIREBASE_AUTH "W7X4zqCNPWEUu9eMIGJe9gnlzkF2kNWbbJhWSppV"
#define WIFI_SSID "U+Net2960"                                          
#define WIFI_PASSWORD "DD87000595"  

String fireStatus = "";          
int led = D4;
                                                              
void setup() 
{
  Serial.begin(9600);
  delay(1000);    
  pinMode(led, OUTPUT);//LED 상태를 OUTPUT으로 지정
                 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                               
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.setString("LED_STATUS", "OFF");//LED_STATUS 상태를 OFF로 지정
}
 
void loop() 
{
  fireStatus = Firebase.getString("LED_STATUS"); //LED_STATUS의 값을 가져와 저장
  if (fireStatus == "ON")   { 
    Serial.println("LED Turned ON");                                                        
    digitalWrite(led, HIGH); //fireStatus가 ON이면 led의 불을 켜고
  } 
  else if (fireStatus == "OFF") {
    Serial.println("LED Turned OFF");
    digitalWrite(led, LOW); //fireStatus가 OFF이면 led의 불을 끈다
  } else {
    Serial.println("Command Error! Please send ON/OFF");
  }

  delay(1000);

}
