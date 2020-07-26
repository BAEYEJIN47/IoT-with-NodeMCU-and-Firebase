#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include <time.h>

//Firebase의 연결을 위해 주소와 비밀번호를 설정
#define FIREBASE_HOST "hello-firebase-0630.firebaseio.com"
#define FIREBASE_AUTH "uTs4ph8QlMAi8zF5giBcOnFJz9c07n4pLNxjwhje"

//WiFi 값 설정
#define WIFI_SSID "U+Net2960" 
#define WIFI_PASSWORD "DD87000595" 

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//한국시간 : 협정시계시보다 9시 빠름
int timezone = 9; 
int dst = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  //WiFi 초기화
  WiFi.mode(WIFI_STA); 
  //WiFi 연결
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting......");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //타임서버도메인주소를 사용해 NTP 설정(네트워크 시간 설정)
  configTime(timezone * 3600, dst, "kr.pool.ntp.org", "time.kriss.re.kr");
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Now: " + NowString());

  //Firebase 실시간 데이터베이스 인증
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  delay(5000);
  // temp, Humidity를 가져와 저장
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // 위에서 측정한 값중 Nan값이 측정되면 오류를 출력하고 프로그램을 종료
  if (isnan(humidity) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(500);
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");
  Serial.println();
  
  StaticJsonBuffer<200> jsonBuffer;
  //JSON은 미리 할당 된 메모리 풀을 사용하여 JsonObject 트리를 저장
  JsonObject& root = jsonBuffer.createObject();
  //데이터를 저장할 root라는 JsonObject를 만들고 값을 저장
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root["time"] = NowString();
  
  //logdht에 root값을 append
  String name = Firebase.push("logDHT", root);
  //오류가 있을 때
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logDHT/");
  Serial.println(name);
  delay(5000);
}

// 현재 시간 받아오는 함수
String NowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);
  String tmpNow = "";
  tmpNow += String(newtime->tm_year + 1900);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mon + 1);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mday);
  tmpNow += " ";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);
  return tmpNow;
}
