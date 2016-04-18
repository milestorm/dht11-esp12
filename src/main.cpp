#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Arduino.h>
//#include <math.h>

#include <DHT.h>

//Initialization
// Sensor
#define DHTTYPE DHT11
#define DHTPIN D2


// Initialize DHT sensor
// For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float humidity, temp_f;  // Values read from sensor
String webString="";     // String to display

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

String readTemp = ""; // initialize global vars for temp and hum
String readHum = "";

ESP8266WebServer server(80);    // set server

int counter = 0;

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_f = dht.readTemperature(false);     // Read temperature as Celsius
    // Check if any reads failed and exit early (to try again).
    /*if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }*/
  }

}

void readTempFromSensor(){
  gettemperature();
  readTemp = String((int)temp_f);
  readHum = String((int)humidity);
}

void setup()
{
  // Init serial
  Serial.begin(230400);
  Serial.println("Booting ESP8266");

  dht.begin();

  // Init wifi manager
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP8266 Sensor");
  // Init ArduinoOta
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("ESP8266FU");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("*OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\n*OTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("*OTA: Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("*OTA: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("*OTA: Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("*OTA: Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("*OTA: Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("*OTA: Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("*OTA: End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("*OTA: Ready");
  Serial.print("*OTA: IP address: ");Serial.println(WiFi.localIP());

  server.on("/", [](){
    server.send(200, "text/plain", "Accessed " + String(++counter) + " times.\nTEMP: "+readTemp+"*C\nHUM: "+readHum+"\%");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();

  //gettemperature();       // read sensor
  //webString="Teplota: "+String((int)temp_f)+"*C --- "+"Vlhkost: "+String((int)humidity)+"%";
  readTempFromSensor();
  webString="Teplota: "+readTemp+"*C --- "+"Vlhkost: "+readHum+"%";
  Serial.println(webString);

  delay(2000);
}
