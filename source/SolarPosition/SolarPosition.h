// SolarPosition.h

// 2019 Ken Willmott
// Arduino library based on the program "Arduino Uno and Solar Position Calculations"
// (c) David R. Brooks, which can be found at http://www.instesre.org/ArduinoDocuments.htm
// and issued under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License:
// https://creativecommons.org/licenses/by-nc-nd/4.0/

//#include <TimeLib.h>            //https://github.com/PaulStoffregen/Time
//#include <Arduino.h>

#include <stdint.h>

const float KM_PER_AU = 149597870.7;  //kilometers per astronomical unit

//#if !defined(__time_t_defined) // avoid conflict with newlib or other posix libc
typedef unsigned long time_ta;
//#endif


typedef time_ta(*getExternalTime)();

// data structure to store solar position results
struct SolarPosition_t
{
  float elevation = 0;
  float azimuth = 0;
  float distance = 0;
  time_ta time = 0;
};

// utility functions
long JulianDate(int year, int month, int day);
SolarPosition_t calculateSolarPosition(time_ta tParam, float Latitude, float Longitude);

// class interface
class SolarPosition
{
  private:

    static getExternalTime getExtTimePtr;  // shared pointer to external sync function

    // current values:
    float Latitude;
    float Longitude;

    // results:
    SolarPosition_t result;

  public:

    SolarPosition(float Latitude, float Longitude);

    static void setTimeProvider(getExternalTime getTimeFunction);

    SolarPosition_t getSolarPosition();
    SolarPosition_t getSolarPosition(time_ta t);

    float getSolarElevation();
    float getSolarElevation(time_ta t);

    float getSolarAzimuth();
    float getSolarAzimuth(time_ta t);

    float getSolarDistance();
    float getSolarDistance(time_ta t);
    
    #define DEG_TO_RAD 0.017453292519943295769236907684886
    void setLatitude(float v) {
      Latitude = v * DEG_TO_RAD;
    }
    void setLongitude(float v) {
      Longitude = v * DEG_TO_RAD;
    }
};


//convenience macros to convert to and from tm years 
#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define  tmYearToY2k(Y)      ((Y) - 30)    // offset is from 2000
#define  y2kYearToTm(Y)      ((Y) + 30)   

typedef struct  { 
  uint8_t Second; 
  uint8_t Minute; 
  uint8_t Hour; 
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month; 
  uint8_t Year;   // offset from 1970; 
} 	tmElements_t, TimeElements, *tmElementsPtr_t;
void breakTime(time_ta timeInput, tmElements_t &tm);