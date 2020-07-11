// Successful


#include "user_interface.h"

int Sensor = D7; // define the Reed sensor interfaces
int revolutions = 0; // define numeric variables val
float windspeed = 0.0;
os_timer_t windSpeed; // declare timer for windSpeed

void user_init()
{
os_timer_setfn(&windSpeed, calculateWS, NULL);
os_timer_arm(&windSpeed, 5000, true); 
}

// callback function for Timer windSpeed
void calculateWS(void *pArg)
{
  int RPS = revolutions / 5;
  int r = 0.087;
  float rotaryTravel = r * 2 * 3.1414;
  float windSpeed = rotaryTravel * RPS;
  windspeed = windSpeed;
  Serial.println(windspeed);
}
void setup ()
{
  Serial.begin(115200);
  user_init();
  pinMode (Sensor, INPUT_PULLUP) ; // output interface as defined Reed sensor
  attachInterrupt(digitalPinToInterrupt(Sensor), bananas, RISING);
  user_init();
  
}
void loop ()
{
Serial.print("revolutions are: "); Serial.println(revolutions);
Serial.print("Wind Speed: "); 
delay(2000);
}

ICACHE_RAM_ATTR void bananas()
{
  revolutions++;
}
