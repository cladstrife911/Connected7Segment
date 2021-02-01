#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#include "wifi_credential.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN       15
#define PIXEL_PER_SEGMENTS 2

Adafruit_NeoPixel pixels(PIXEL_PER_SEGMENTS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 1000

#define DEFAULT_PIXEL_BRIGHTNESS 10
#define NORMAL_PIXEL_BRIGHTNESS 80
#define OFF_PIXEL_BRIGHTNESS 0
int bluePixel=DEFAULT_PIXEL_BRIGHTNESS;
int redPixel=DEFAULT_PIXEL_BRIGHTNESS;
int greenPixel=DEFAULT_PIXEL_BRIGHTNESS;

const char* ssid = MY_SSID;           
const char* password = MY_PASSWORD;

const char* mqtt_server = "192.168.1.2";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  Serial.print("### RESET ###");
  
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  initWifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  pixels.begin();
}

void loop() {
  pixels.clear();
  for(int i=0; i< PIXEL_PER_SEGMENTS; i++) {
    
    pixels.setPixelColor(i, pixels.Color(redPixel, greenPixel, bluePixel));
  }
  pixels.show();
  delay(DELAYVAL);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void initWifi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      //digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      //digitalWrite(ledPin, LOW);
    }
  }else if (String(topic) == "esp32/time") {
    Serial.println(messageTemp);
  }else if (String(topic) == "esp32/color") {
    if(messageTemp == "blue"){
      bluePixel = NORMAL_PIXEL_BRIGHTNESS;
      redPixel = OFF_PIXEL_BRIGHTNESS;
      greenPixel = OFF_PIXEL_BRIGHTNESS;
    }else if(messageTemp == "red"){
      bluePixel = OFF_PIXEL_BRIGHTNESS;
      redPixel = NORMAL_PIXEL_BRIGHTNESS;
      greenPixel = OFF_PIXEL_BRIGHTNESS;
    }else if(messageTemp == "green"){
      bluePixel = OFF_PIXEL_BRIGHTNESS;
      redPixel = OFF_PIXEL_BRIGHTNESS;
      greenPixel= NORMAL_PIXEL_BRIGHTNESS;
    }else{
      /*white*/
      bluePixel = NORMAL_PIXEL_BRIGHTNESS;
      redPixel = NORMAL_PIXEL_BRIGHTNESS;
      greenPixel= NORMAL_PIXEL_BRIGHTNESS;
    }
  }else{
    //unregistered topic
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      client.subscribe("esp32/time");
      client.subscribe("esp32/color");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
