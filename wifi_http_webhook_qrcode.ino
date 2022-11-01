#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32QRCodeReader.h>
#include <Wire.h>
#include "SSD1306.h"

#define WIFI_SSID0 "-MobHotHome-"
#define WIFI_PASSWORD0 "pisanggoreng"
#define WIFI_SSID1 "-MobHot-"
#define WIFI_PASSWORD1 "pisanggoreng"
#define WIFI_SSID2 "-MobHotHome-"
#define WIFI_PASSWORD2 "pisanggoreng"


#define WEBHOOK_URL "https//your-url:8080/endpoint"
#define DOOR_RELAY_PIN 12
#define FLASH_GPIO_NUM 4

#define TRIG_PIN 13 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 15 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin

float duration_us, distance_cm;


int passIdx = 0;


SSD1306 display(0x3c, 15, 13, GEOMETRY_128_32);

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;


#define BUZZER_PIN  12 // ESP32 pin GIOP18 connected to piezo buzzer

int count = 0;
bool camoff = true;
bool lcamoff = false;


int playing = 0;
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
    WiFi.begin(WIFI_SSID0, WIFI_PASSWORD0);
  else if (passIdx == 1)
    WiFi.begin(WIFI_SSID1, WIFI_PASSWORD1);


  int maxRetries = 10;
  display.clear();
  display.drawString(0, 0, "Connecting to Wifi " + String(passIdx));
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

void callWebhook(String code)
{
  HTTPClient http;
  http.begin(String(WEBHOOK_URL) + "?code=" + code);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
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

  http.end();
}

//LED setup
int freq = 5000;
int ledCHannel = 2;
int res = 8;
const int ledPin = 4;
int brightness;
void setup()
{
  Serial.begin(115200);
  Serial.println();

  display.init();
  display.setFont(ArialMT_Plain_16);

  display.clear();
  display.drawString(0, 0, "Initializing...: ");
  display.display();
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

  //  display.flipScreenVertically();


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

void loop()
{

  //display.clear();
  // display.drawString(0,16, "Counter: " + String(count));


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
  bool connected = connectWifi();
  if (isConnected != connected)
  {
    isConnected = connected;
  }
  Serial.println("loops=" + String(count));
  if (reader.receiveQrCode(&qrCodeData, 100))
  {
    //Serial.println("Found QRCode");
    //display.drawString(0,0, "QR Code Found : ");
    if (qrCodeData.valid)
    {
      display.clear();
      display.drawStringMaxWidth(0, 0, 128, (const char *) qrCodeData.payload);
      //display.drawString(0,0, (const char *) qrCodeData.payload);
      doubleFlash();
      //display.drawString(0,0, "Stand By....");

      Serial.print("Payload: ");
      Serial.println((const char *)qrCodeData.payload);

      callWebhook(String((const char *)qrCodeData.payload));
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



}
