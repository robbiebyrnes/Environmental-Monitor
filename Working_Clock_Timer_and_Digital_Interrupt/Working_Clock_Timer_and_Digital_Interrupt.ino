// Successful for interrupt and for timer!
extern "C" {
#include "user_interface.h"
}
const int reedSwitch = D7; // define the Reed sensor interface
int revolutions = 0;
int RPS; // revolutions per second
const float r = 0.087; // raduis
float rotaryTravel; // in meters
float windspeed = 0.0; // in meters/second
float WINDSPEED = 0.0;

os_timer_t windSpeed; // instantiate timer object as windSpeed

void setup ()
{
  Serial.begin(115200);
  pinMode (reedSwitch, INPUT_PULLUP) ; // output interface as defined Reed sensor
  attachInterrupt(digitalPinToInterrupt(reedSwitch), Anemometer_ISR, RISING);
  user_init();
}

void loop ()
{
Serial.print("revolutions are: "); Serial.println(revolutions);
Serial.print("windspeed is: "); Serial.println(WINDSPEED);
delay(2000);
yield();
}

// ISR 
ICACHE_RAM_ATTR void Anemometer_ISR()
{
  revolutions++;
  RPS = revolutions;
}

// Timer callback function
void calculateWS(void *pArg)
{
  rotaryTravel = (r * 2) * 3.1414; // 
  RPS = RPS/5;
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
  Serial.println("Hello from user_init");
}
