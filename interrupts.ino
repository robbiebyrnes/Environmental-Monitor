//BME280 and server code from https://lastminuteengineers.com
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SEALEVELPRESSURE_HPA (1013.25)
extern "C" {
  #define USE_US_TIMER 1
  #include "user_interface.h"
}

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;
/***********************************************************************************
 * ************************************Test Area***********************************/
const int Anemometer = 0;
int revolutions = 0;
float windspeed = 0.0;
ICACHE_RAM_ATTR void count()
{
  revolutions++;
}

LOCAL os_timer_t windSpeed;

void user_init()
{
os_timer_disarm(&windSpeed);
os_timer_setfn(&windSpeed, calculateWS, NULL);
os_timer_arm(&windSpeed, 5000, 1);
}
// callback
void calculateWS(void *pArg)
{
  int RPS = revolutions/5;
  int r =0.087;
  float rotaryTravel = r*2*3.1414;
  float windSpeed = rotaryTravel*RPS;
  windspeed = windSpeed;
}

/*Put your SSID & Password*/
const char* ssid = "NOWTVDEXXI";
const char* password = "kF4QMhzc3xcS";  //Enter Password here

ESP8266WebServer server(80);              
 
void setup() {
  pinMode(Anemometer, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Anemometer), count, RISING);
  
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.print(windspeed);
}
void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  user_init();
  system_timer_reinit();
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude,windspeed));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature,float humidity,float pressure,float altitude, float windspeed){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="<p>Pressure: ";
  ptr +=pressure;
  ptr +="hPa</p>";
  ptr +="<p>Altitude: ";
  ptr +=altitude;
  ptr +="m</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  ptr +="<p>Wind Speed: ";
  ptr +=windspeed;
  ptr +="m/s</p>";
  return ptr;
}
