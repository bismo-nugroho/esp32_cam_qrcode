#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32QRCodeReader.h>
#include <Wire.h>
#include "SSD1306.h"
#include <Preferences.h>

//#include <WebServer.h>
//#include "SPIFFS.h"
//#include <Arduino_JSON.h>

Preferences preferences;

// Set NodeMCU Wifi hostname based on chip mac address
char chip_id[15];

String wifi_ssid0 = "-MobHotHome-";
String wifi_password0 = "pisanggoreng";
String wifi_ssid1 = "-MobHot-";
String wifi_password1 = "pisanggoreng";
String webhook_url = "https//your-url:8080/endpoint";
String secret_key = "ABSNASU$##^$#@^%#^%#&$&$*&$*&";

String wifi_ssid = "QRSYSTEM-";// + String(chip_id);
String wifi_password = "QRReaderSystem";// + String(chip_id);;

bool readerstat = true;

bool configstat = true;

//IPAddress local_ip(192, 168, 123, 4); //ip address untuk akses ESP32
//IPAddress gateway(192, 168, 123, 1); //gateway
//IPAddress subnet(255, 255, 255, 0); //subnet
//WebServer server(80);  // port untuk akses HTTP



double idle_count = 0;
double timertick = 0;

String message = "";
//JSONVar sliderValues;


#define DOOR_RELAY_PIN 2
#define FLASH_GPIO_NUM 4
#define PIR_PIN 12
#define IDLE_ACT 10000

///AsyncWebServer server(80);
// Create a WebSocket object

//AsyncWebSocket ws("/ws");

float duration_us, distance_cm;

int passIdx = 0;

SSD1306 display(0x3c, 15, 13, GEOMETRY_128_32);

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

int countdown = 0;

#define BUZZER_PIN  2 // ESP32 pin GIOP18 connected to piezo buzzer

int count = 0;
bool camoff = true;
bool lcamoff = false;

bool isScan = false;


int playing = 0;

/*
  String getSliderValues() {

  //sliderValues["sliderValue1"] = String(sliderValue1);
  //sliderValues["sliderValue2"] = String(sliderValue2);
  //sliderValues["sliderValue3"] = String(sliderValue3);

  sliderValues["ssid0"] = wifi_ssid0;
  sliderValues["pass0"] = wifi_password0;
  sliderValues["ssid1"] = wifi_ssid0;
  sliderValues["pass1"] = wifi_password0;
  sliderValues["webhook_url"] = webhook_url;
  sliderValues["secret_key"] = secret_key;



  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
  }
*/
/*

  void notifyClients(String sliderValues) {
  //ws.textAll(sliderValues);
  }

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    Serial.println("Data from client" + String(message));
    /*
      if (message.indexOf("") >= 0) {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
      }
      if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
      }
      if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle3);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
      }
      if (strcmp((char*)data, "getValues") == 0) {
      notifyClients(getSliderValues());
      }
*/
/*
  }
  }

  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
  Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  break;
  case WS_EVT_DISCONNECT:
  Serial.printf("WebSocket client #%u disconnected\n", client->id());
  break;
  case WS_EVT_DATA:
  handleWebSocketMessage(arg, data, len);
  break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
  break;
  }
  }

  void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  }

*/
/*
  // Initialize SPIFFS
  void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else {
    Serial.println("SPIFFS mounted successfully");
  }
  }

*/

void tone(byte pin, int freq) {
  ledcSetup(3, 2000, 8); // setup beeper
  ledcAttachPin(pin, 3); // attach beeper
  ledcWriteTone(3, freq); // play tone
  playing = pin; // store pin
}
void noTone() {
  tone(playing, 0);
}

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
struct QRCodeData qrCodeData;
bool isConnected = false;

void openDoor()
{
  digitalWrite(DOOR_RELAY_PIN, LOW);
}

void closeDoor()
{
  digitalWrite(DOOR_RELAY_PIN, HIGH);
}


bool connectWifi2()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }


  WiFi.begin("-MobHot-", "pisanggoreng");
  int maxRetries = 10;
  display.clear();
  display.drawString(0, 0, "Connecting to Wifi...");
  display.display();
  Serial.println("Wifi Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    //maxRetries--;
    //if (maxRetries <= 0)
    // {
    return false;
    // }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  display.clear();
  display.drawString(0, 0, "Wifi Connected...");
  display.display();
  return true;
}

bool connectWifi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }

  if (passIdx == 0)
    WiFi.begin(wifi_ssid0.c_str(), wifi_password0.c_str());
  else if (passIdx == 1)
    WiFi.begin(wifi_ssid1.c_str(), wifi_password1.c_str());


  int maxRetries = 10;
  display.clear();
  display.drawString(0, 0, "Connecting to Wifi " + String(passIdx, 0));
  display.display();
  Serial.println("Wifi Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    //    return  connectWifi2();
    maxRetries--;
    if (maxRetries <= 0)
    {

      if (passIdx == 0)
        passIdx = 1;
      else
        passIdx = 0;

      return false;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  display.clear();
  display.drawString(0, 0, "Wifi Connected...");
  display.display();
  return true;
}

void callWebhook(String code, String types)
{
  HTTPClient http;
  http.begin(String(webhook_url) + "?type=" + types + "&nik=" + code + "&secret_key=" + secret_key );


  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK)
  {

    Serial.print("HTTP Response code: ");
    Serial.println(httpCode); \
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpCode);
  }
  /*

    if (types=="scan_qrcode"){

    }
    Serial.println("Open door");
    openDoor();
    delay(2000);
    closeDoor();
    }
    else
    {
    Serial.println("Not authorized");
    closeDoor();
    }
  */

  http.end();
}

//LED setup
int freq = 5000;
int ledCHannel = 2;
int res = 8;
const int ledPin = 4;
int brightness;






void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to APs successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println("info.disconnected.reason");
  Serial.println("Trying to Reconnect");
  //WiFi.begin(ssid, password);
}


void getPref() {
  configstat = preferences.getBool("configstat", true);
  wifi_ssid0 = preferences.getString("ssid0", wifi_ssid0);
  wifi_password0 = preferences.getString("pass0", wifi_password0);
  wifi_ssid1 = preferences.getString("ssid1", wifi_ssid1);
  wifi_password1 = preferences.getString("pass1", wifi_password1);

  webhook_url = preferences.getString("webhook_url", webhook_url);
  secret_key = preferences.getString("secret_key", secret_key);

  configstat = false;
}



void setup()
{
  Serial.begin(115200);
  Serial.println();

  preferences.begin("my-app", false);

  getPref();

  display.init();
  display.setFont(ArialMT_Plain_16);

  display.clear();
  display.drawString(0, 0, "Initializing...: ");
  display.display();


  if (configstat) {
    /*
        snprintf(chip_id, 15, "%04X", (uint16_t)(ESP.getEfuseMac() >> 32 ) );
        String hostname = "esp32cam" + String(chip_id);

        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 0, "Config Mode" );
        display.drawString(64, 16, "ID:" + String(chip_id) );
        display.display();

        wifi_ssid = "QRSYSTEM" + String(chip_id);
        wifi_password = "QRReaderSystem" + String(chip_id);


        WiFi.disconnect(true);
        WiFi.mode(WIFI_AP);

        WiFi.softAP(wifi_ssid.c_str(), wifi_password.c_str());
        WiFi.softAPConfig(local_ip, gateway, subnet);
        Serial.println("Terhubung ke Akses point");
        //server.on("/", handle_root);


        Serial.println("Start AP " + wifi_ssid);
        Serial.println("Start Password " + wifi_password);
        // Serial.println("Start AP IP "+myIP);



        delay(1000);

        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 0, wifi_ssid );
        display.drawString(64, 16, "P:" + wifi_password );
        display.display();

        //initWebSocket();


        // Web Server Root URL
        //server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
         // request->send(SPIFFS, "/index.html", "text/html");
       // });

        //server.on("/", []() {
        //server.send(200,SPIFFS, "/index.html", "text/html")
      //});

        server.serveStatic("/", SPIFFS, "/");

        // Start server
        server.begin();

    */
  } else {

    pinMode(PIR_PIN, INPUT);

    // configure the trigger pin to output mode
    //pinMode(TRIG_PIN, OUTPUT);
    // configure the echo pin to input mode
    //pinMode(ECHO_PIN, INPUT);
    //    pinMode (BUZZER_PIN, OUTPUT);

    // pinMode(DOOR_RELAY_PIN, OUTPUT);
    //closeDoor();

    reader.setup();
    //reader.setDebug(true);
    Serial.println("Setup QRCode Reader");

    reader.begin();
    Serial.println("Begin QR Code reader");


    //pinMode(FLASH_GPIO_NUM, OUTPUT);


    //digitalWrite (BUZZER_PIN, HIGH); //turn buzzer on
    tone(BUZZER_PIN, 1000);
    ledcSetup(ledCHannel, freq, res);

    ledcAttachPin(ledPin, ledCHannel);


    brightness = 10;
    ledcWrite(ledCHannel, brightness);
    // digitalWrite(FLASH_GPIO_NUM, HIGH);
    delay(1000);
    brightness = 0;
    ledcWrite(ledCHannel, brightness);
    // digitalWrite(FLASH_GPIO_NUM, LOW);
    //delay(1000);
    // digitalWrite (BUZZER_PIN, LOW); //turn buzzer on
    noTone();

    WiFi.disconnect(true);

  }
  /*
    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    //  display.flipScreenVertically();
  */

}


int counter = -1;

void showSuccess() {

  isScan = true;
  countdown = 5;
  display.clear();
  display.drawStringMaxWidth(0, 0, 128, "Absen Berhasil");
  display.drawStringMaxWidth(0, 15, 128, (const char *) qrCodeData.payload);
  //display.drawString(0,0, (const char *) qrCodeData.payload);
  doubleFlash();
  //display.drawString(0,0, "Stand By....");

  //display.clear();
  //display.drawString(0, 0, "Initializing...: ");
  display.display();
}

void doubleFlash() {
  brightness = 0;
  ledcWrite(ledCHannel, brightness);
  // digitalWrite (BUZZER_PIN, HIGH); //turn buzzer on
  tone(BUZZER_PIN, 1000);
  delay(200);
  brightness = 10;
  ledcWrite(ledCHannel, brightness);
  //digitalWrite (BUZZER_PIN, LOW); //turn buzzer on
  noTone();

  delay(200);
  brightness = 0;
  ledcWrite(ledCHannel, brightness);
  lcamoff  = false;
  camoff  = true;
  count = 5;

}

void triggerAct() {
  if (readerstat)
    idle_count = millis();



  if (!readerstat ) {
    idle_count = millis();
    lcamoff = !camoff;
    readerstat = true;
  }
}

void loop()
{



  //display.clear();
  // display.drawString(0,16, "Counter: " + String(count));

  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Movement detected.");
    triggerAct();

  } else {
    Serial.println("Did not detect movement.");
  }

  // Clears the trigPin
  // digitalWrite(TRIG_PIN, LOW);
  //  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  // digitalWrite(TRIG_PIN, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(TRIG_PIN, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  // duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  // distanceCm = duration * SOUND_SPEED/2;




  //  digitalWrite(TRIG_PIN, HIGH);
  // delayMicroseconds(10);
  //  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  // duration_us = pulseIn(ECHO_PIN, HIGH);


  distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  // Serial.print("distance: ");
  // Serial.print(distanceCm);
  //  Serial.println(" cm");

  //ledcWrite(ledCHannel, brightness);

  if (!configstat && readerstat ) {
    bool connected = connectWifi();
    if (isConnected != connected)
    {
      isConnected = connected;

      if ( isConnected ) {
        countdown = 3;
      }
    }

    if (countdown >= 0)
      countdown--;


    if ( countdown == 0 && readerstat) {

      //if ( !isScan){
      display.clear();
      display.drawString(0, 0, "Ready.... ");
      display.display();
      isScan = false;
      // }

    }

    Serial.println("loops=" + String(count));
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      triggerAct();
      //Serial.println("Found QRCode");
      //display.drawString(0,0, "QR Code Found : ");
      if (qrCodeData.valid)
      {
        showSuccess();

        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);

        callWebhook(String((const char *)qrCodeData.payload), "scan_qrcode");

      }
      else
      {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    display.display();
    delay(300);
    count++;

    if (count == 5) {
      camoff  = true;
    }

    if (count == 8) {
      camoff  = false;
      count = 0;
    }

    if (camoff != lcamoff && camoff == true) {
      //digitalWrite(FLASH_GPIO_NUM, LOW);
      brightness = 0;
      ledcWrite(ledCHannel, brightness);
      lcamoff = camoff;
    } else if ( camoff != lcamoff  && camoff == false ) {
      // digitalWrite(FLASH_GPIO_NUM, HIGH);
      brightness = 10;
      ledcWrite(ledCHannel, brightness);
      lcamoff = camoff;
    }///
    //ledcWrite(ledCHannel, brightness);

    if ( isScan == false  && millis() - idle_count > IDLE_ACT ) {
      readerstat = false;
      brightness = 0;
      ledcWrite(ledCHannel, brightness);
      lcamoff = !camoff;
      display.clear();
      display.drawString(0, 0, "");
      display.display();

      esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);

      Serial.println("Going to sleep now");
      esp_deep_sleep_start();

    }

  } else {

    //server.handleClient();
    delay(200);
  }

}
