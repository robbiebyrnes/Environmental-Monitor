// Licenced under GNU General Public License v3.0
// A project developed by Robert Byrnes
// This is the welcome home Bob and Doug version......congrats to Crew Dragon and Crew.
// Version 1.5.3 - first compile 02/08/2020
// This code version works for IoT weather station: BME280/Anemometer/ESP8266 Over the air programming/Local IP Server/Post to external server
// Hardware interrupt on pin D7 for anemometer and SDK timer interrupt for calculating windSpeed in
// calculateWS() and millis() interrupt for sending HTTP post to external server - server always returns error if calling from another sdk interrupt
// This version uses ESPWebserver to send string/html to local IP including 30 minute updates to web based server
// Version 1.5.3 includes bug fix for BME280 crashing (in handle_OnConnect()) also a catch routine in void loop ()

extern "C" {
#include "user_interface.h"
}
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


// The followiung section is for OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#ifndef STASSID
#define STASSID "NOWTVDEXXI_LIVING_ROOM"
#define STAPSK  "kF4QMhzc3xcS"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;
// End of OTA section

// The following section is for posting data to domain/server/database/php script
const char* serverName = "http://www.envirosample.online/derriston/post-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9"; // Must match API key in post-data.php
String sensorName = "Derriston";
String sensorLocation = "Crediton";
unsigned long lastTime = 0;
unsigned long timerDelay = 1800000;
// End

// The following section is for weather station
// For BME280
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // instantiate object
float temperature, humidity, pressure;
//float altitude; // uncomment if using altitude calculation

ESP8266WebServer server(80); // com port 80 - for local IP

// Anemometer
const int reedSwitch = D7; // define the Reed sensor interface
int revolutions = 0;
int RPS; // revolutions per second
const float r = 0.087; // raduis
float rotaryTravel; // in meters
float windspeed = 0.0; // in meters/second
float WINDSPEED = 0.0;
// End Anemometer Section

// Declare SDK timers
os_timer_t windSpeed; // Instantiate timer object as windSpeed
// End weather station section

void setup()
{
  Serial.begin(115200);

  // The following section is for OTA
  Serial.println("Booting");
  WiFi.begin(ssid, password);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // End OTA section

  // The following section is for weather station
  // Anemometer
  pinMode (reedSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(reedSwitch), Anemometer_ISR, RISING);
  user_init();

  // HEX address for BME280 sensor, start the sensor and read the data initialising pre-defined variables
  bme.begin(0x76);
  temperature = bme.readTemperature(); //bme library member function
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  //altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // Set up duties for ESP8266 Server
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

  // Setup and handling for OTA
  ArduinoOTA.setHostname("Derriston Weather");
  ArduinoOTA.setPassword("admin120846");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // End setup and handling for OTA
}

void loop()
{
  ArduinoOTA.handle(); // For OTA
  server.handleClient(); // For station mode on NodeMCU ESP8266
  //Serial.print("revolutions are: "); Serial.println(revolutions);
  //Serial.print("windspeed is: "); Serial.println(WINDSPEED);
  yield();
  // Set up duties for HTTP client HTTP Post routine
  if ((millis() - lastTime) > timerDelay) { 
  HTTPClient http;
  http.begin(serverName);
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Prepare your HTTP POST request data
  String httpRequestData = "api_key="+apiKeyValue+"&sensor="+sensorName+"&location="+sensorLocation+"&Temperature="+String(bme.readTemperature())+"&Humidity="+String(bme.readHumidity())+"&Pressure="+String(bme.readPressure() / 100.0F)+"&WindSpeed="+WINDSPEED+"";
  http.POST(httpRequestData);
  int httpResponseCode = http.POST(httpRequestData);
  String payload = http.getString(); //Get the response payload
  Serial.println(httpResponseCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end(); // Free up memory resources
    if (httpRequestData = "api_key="+apiKeyValue+"&sensor="+sensorName+"&location="+sensorLocation+"&Temperature="+0+"&Humidity="+0+"&Pressure="+0+"&WindSpeed="+WINDSPEED+"")
    {
      ESP.restart();
    }
  lastTime = millis();
  }
  // End HTTP Post routine 
}

// ISR to measure wind speed
ICACHE_RAM_ATTR void Anemometer_ISR()
{
  revolutions++;
  RPS = revolutions;
}

// Timer callback function for anemometer
void calculateWS(void *pArg)
{
  rotaryTravel = (r * 2) * 3.1414;
  RPS = RPS / 5;
  windspeed = RPS * rotaryTravel;
  WINDSPEED = windspeed;
  Serial.println("Hello from calculateWS()");
  revolutions = 0;
  windspeed = 0.0;
}

// Five second timer
void user_init()
{
  os_timer_setfn(&windSpeed, calculateWS, NULL);
  os_timer_arm(&windSpeed, 5000, 1);
  //Serial.println("Hello from user_init");
}

// Instruct ESP8266 to send the data stream
void handle_OnConnect()
{
  //Serial.println("Server handle on connect working");
  temperature = bme.readTemperature(); //bme library member function
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  if (pressure = 0)
  {
    digitalWrite (D8, LOW);
    delay(500);
    digitalWrite (D8, HIGH); 
  }
  //altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  //Serial.print ("Temp: "); Serial.println(temperature);
  //Serial.print ("Humidity: "); Serial.println(humidity);
  //Serial.print ("Pressure: "); Serial.println(pressure);
  server.send(200, "text/html", SendHTML(temperature, humidity, pressure, WINDSPEED));
}

// Report error if ESP8266 Station/Server failed to connect to WiFi
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

// Send the following data in HTML page to local IP
String SendHTML(float temperature, float humidity, float pressure, float WINDSPEED)
{
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 Derriston Weather</title>";
  //ptr +="<meta http-equiv = 'refresh' content = '2'>"; // meta tag for browser refresh in seconds
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr += "<style>";
  ptr += "html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr += "body{margin: 0px;} ";
  ptr += "h1 {margin: 50px auto 30px;} ";
  ptr += ".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr += ".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr += ".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  ptr += ".temperature .reading{color: #F29C1F;}";
  ptr += ".humidity .reading{color: #3B97D3;}";
  ptr += ".pressure .reading{color: #26B99A;}";
  ptr += ".WINDSPEED .reading{color: #dc6bff;}";
  ptr += ".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr += ".data{padding: 10px;}";
  ptr += ".container{display: table;margin: 0 auto;}";
  ptr += ".icon{width:65px}";
  ptr += "</style>";
  ptr += "<script>\n"; // script tag for data refreshing
  ptr += "setInterval(loadDoc,2000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>";
  ptr += "<body>";
  ptr += "<h1>Derriston Weather</h1>";
  ptr += "<div class='container'>";
  ptr += "<div class='data temperature'>";
  ptr += "<div class='side-by-side icon'>";
  ptr += "<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
  ptr += "C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
  ptr += "c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
  ptr += "c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
  ptr += "s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
  ptr += "</div>";
  ptr += "<div class='side-by-side text'>Temperature</div>";
  ptr += "<div class='side-by-side reading'>";
  ptr += temperature, 1; //add (int) after += to change to int
  ptr += "<span class='superscript'>&deg;C</span></div>";
  ptr += "</div>";
  ptr += "<div class='data humidity'>";
  ptr += "<div class='side-by-side icon'>";
  ptr += "<svg enable-background='new 0 0 29.235 40.64'height=40.64px id=Layer_1 version=1.1 viewBox='0 0 29.235 40.64'width=29.235px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><path d='M14.618,0C14.618,0,0,17.95,0,26.022C0,34.096,6.544,40.64,14.618,40.64s14.617-6.544,14.617-14.617";
  ptr += "C29.235,17.95,14.618,0,14.618,0z M13.667,37.135c-5.604,0-10.162-4.56-10.162-10.162c0-0.787,0.638-1.426,1.426-1.426";
  ptr += "c0.787,0,1.425,0.639,1.425,1.426c0,4.031,3.28,7.312,7.311,7.312c0.787,0,1.425,0.638,1.425,1.425";
  ptr += "C15.093,36.497,14.455,37.135,13.667,37.135z'fill=#3C97D3 /></svg>";
  ptr += "</div>";
  ptr += "<div class='side-by-side text'>Humidity</div>";
  ptr += "<div class='side-by-side reading'>";
  ptr += humidity, 1; //add (int) after += to change to int
  ptr += "<span class='superscript'>%</span></div>";
  ptr += "</div>";
  ptr += "<div class='data pressure'>";
  ptr += "<div class='side-by-side icon'>";
  ptr += "<svg enable-background='new 0 0 40.542 40.541'height=40.541px id=Layer_1 version=1.1 viewBox='0 0 40.542 40.541'width=40.542px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M34.313,20.271c0-0.552,0.447-1,1-1h5.178c-0.236-4.841-2.163-9.228-5.214-12.593l-3.425,3.424";
  ptr += "c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293c-0.391-0.391-0.391-1.023,0-1.414l3.425-3.424";
  ptr += "c-3.375-3.059-7.776-4.987-12.634-5.215c0.015,0.067,0.041,0.13,0.041,0.202v4.687c0,0.552-0.447,1-1,1s-1-0.448-1-1V0.25";
  ptr += "c0-0.071,0.026-0.134,0.041-0.202C14.39,0.279,9.936,2.256,6.544,5.385l3.576,3.577c0.391,0.391,0.391,1.024,0,1.414";
  ptr += "c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293L5.142,6.812c-2.98,3.348-4.858,7.682-5.092,12.459h4.804";
  ptr += "c0.552,0,1,0.448,1,1s-0.448,1-1,1H0.05c0.525,10.728,9.362,19.271,20.22,19.271c10.857,0,19.696-8.543,20.22-19.271h-5.178";
  ptr += "C34.76,21.271,34.313,20.823,34.313,20.271z M23.084,22.037c-0.559,1.561-2.274,2.372-3.833,1.814";
  ptr += "c-1.561-0.557-2.373-2.272-1.815-3.833c0.372-1.041,1.263-1.737,2.277-1.928L25.2,7.202L22.497,19.05";
  ptr += "C23.196,19.843,23.464,20.973,23.084,22.037z'fill=#26B999 /></g></svg>";
  ptr += "</div>";
  ptr += "<div class='side-by-side text'>Pressure</div>";
  ptr += "<div class='side-by-side reading'>";
  ptr += (int)pressure; //add (int) after += to change to int
  ptr += "<span class='superscript'>hPa</span></div>";
  ptr += "</div>";
  ptr += "<div class='data WINDSPEED'>";
  ptr += "<div class='side-by-side icon'>";
  ptr += "<svg enable-background:new 0 0 85 65'height=60.4986686706543px id=Layer_1 version=1.1 viewBox='0 0 81.88069152832031 60.4986686706543'width=81.88069152832031px xmlns=http://www.w3.org/2000/svg x=0px y=0px>";
  ptr += "<g transform='matrix(0.1171872690320015, 0, 0, 0.12407027184963226, 12.365875244140625, -1.1450347900390625)'><g><g>";
  ptr += "<path d='M189.63,94.8c-31.37,0-56.889,25.535-56.889,56.905c0,5.236,4.241,9.488,9.482,9.488c5.241,0,9.482-4.241,9.482-9.477&#10;&#9;&#9;&#9;c0-20.912,17.009-37.925,37.926-37.925c20.917";
  ptr += ",0,37.926,16.954,37.926,37.866s-17.009,37.865-37.926,37.865H9.482&#10;&#9;&#9;&#9;c-5.241,0-9.482,4.245-9.482,9.482s4.241,9.481,9.482,9.481H189.63c31.37,0,56.889-25.456,56.889-56.827&#10;&#9;&#9;&#9;C246.518,120.288,221,94.8,189.63,94.8z' style='stroke: rgb(220, 107, 255); fill: rgb(220, 107, 255);'/>";
  ptr += "</g></g><g><g><path d='M331.852,132.726c-31.37,0-56.889,25.535-56.889,56.905c0,5.236,4.241,9.488,9.482,9.488c5.241,0,9.481-4.241,9.481-9.477&#10;&#9;&#9;&#9;c0-20.912,17.009-37.925,37.926-37.925c20.917,0,37.926,16.954,37.926,37.866c0,20.912-17.009,37.865-37.926,37.865H9.482&#10;&#9;&#9;&#9;c-5.241,0-9.482,4.245-9.482,";
  ptr += "9.482s4.241,9.482,9.482,9.482h322.37c31.37,0,56.889-25.456,56.889-56.828&#10;&#9;&#9;&#9;C388.741,158.215,363.222,132.726,331.852,132.726z'fill=#dc6bff />";
  ptr += "</g></g><g><g><path d='M331.852,303.3H9.482C4.241,303.3,0,307.546,0,312.782s4.241,9.481,9.482,9.481h322.37&#10;&#9;&#9;&#9;c20.917,0,37.926,17.076,37.926,37.987c0,20.913-17.009,37.957-37.926,37.957c-20.917,0-37.926-16.998-37.926-37.91&#10;&#9;&#9;&#9;c0-5.236-4.241-9.475-9.481-9.475c-5.241,0-9.482,4.25-9.482,";
  ptr += "9.486c0,31.37,25.519,56.89,56.889,56.89&#10;&#9;&#9;&#9;c31.37,0,56.889-25.579,56.889-56.949C388.741,328.88,363.222,303.3,331.852,303.3z'fill=#dc6bff />";
  ptr += "</g></g><g><g><path d='M455.111,170.683c-31.37,0-56.889,25.519-56.889,56.889c0,5.236,4.241,9.481,9.482,9.481c5.241,0,9.481-4.245,9.481-9.481&#10;&#9;&#9;&#9;c0-20.912,17.009-37.926,37.926-37.926c20.917,0,37.926,16.953,37.926,37.864c0,20.913-17.009,37.865-37.926,37.865H9.482&#10;&#9;&#9;&#9;c-5.241,0-9.482,4.245-9.482,";
  ptr += "9.482c0,5.236,4.241,9.481,9.482,9.481h445.63c31.37,0,56.889-25.456,56.889-56.828&#10;&#9;&#9;&#9;C512,196.139,486.482,170.683,455.111,170.683z'fill=#dc6bff /></g></svg>";
  ptr += "</div>";
  ptr += "<div class='side-by-side text'>Wind Speed</div>";
  ptr += "<div class='side-by-side reading'>";
  ptr += WINDSPEED; //add (int) after += to change to int
  ptr += "<span class='superscript'>m/s</span></div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</body>";
  ptr += "</html>";
  return ptr;
}
