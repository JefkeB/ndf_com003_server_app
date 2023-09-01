// SolarPosition.cpp

// 2019 Ken Willmott
// Arduino library based on the program "Arduino Uno and Solar Position Calculations"
// (c) David R. Brooks, which can be found at http://www.instesre.org/ArduinoDocuments.htm
// and issued under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License:
// https://creativecommons.org/licenses/by-nc-nd/4.0/

#include "SolarPosition.h"
#include <math.h>


#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307179586476925286766559

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105




// leap year calculator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

void breakTime(time_ta timeInput, tmElements_t &tm){
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970 
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1  
  tm.Day = time + 1;     // day of month
}

getExternalTime SolarPosition::getExtTimePtr = nullptr; // pointer to external sync function

// constructor
//
SolarPosition::SolarPosition(const float Lat, const float Lon) {
	Latitude = Lat * DEG_TO_RAD;
	Longitude = Lon * DEG_TO_RAD;
}

// *** public methods: ***

// assign a Time provider function
//
void SolarPosition::setTimeProvider(getExternalTime getTimeFunction) {
	getExtTimePtr = getTimeFunction;
}

// Get current Position
//
SolarPosition_t SolarPosition::getSolarPosition() {
	const SolarPosition_t nullPos;

	if (getExtTimePtr != nullptr) {
		time_ta timeNow = getExtTimePtr();
		SolarPosition_t pos;
		result = calculateSolarPosition(timeNow, Latitude, Longitude);
		pos.elevation = result.elevation * RAD_TO_DEG;
		pos.azimuth = result.azimuth * RAD_TO_DEG;
		pos.distance = result.distance * KM_PER_AU;
		pos.time = timeNow;
		return pos;
	} else {
		return nullPos;
	}
}

// Get Position for specified time
//
SolarPosition_t SolarPosition::getSolarPosition(time_ta t) {
	SolarPosition_t pos;
	result = calculateSolarPosition(t, Latitude, Longitude);
	pos.elevation = result.elevation * RAD_TO_DEG;
	pos.azimuth = result.azimuth * RAD_TO_DEG;
	pos.distance = result.distance * KM_PER_AU;
	pos.time = t;
	return pos;
}

// Get current Elevation
//
float SolarPosition::getSolarElevation() {
	if (getExtTimePtr != nullptr) {
		result = calculateSolarPosition(getExtTimePtr(), Latitude, Longitude);
		return result.elevation * RAD_TO_DEG;
	} else {
		return 0;
	}
}

// Get Elevation for specified time
//
float SolarPosition::getSolarElevation(time_ta t) {
	result = calculateSolarPosition(t, Latitude, Longitude);
	return result.elevation * RAD_TO_DEG;
}

// Get current Azimuth
//
float SolarPosition::getSolarAzimuth() {
	if (getExtTimePtr != nullptr) {
		result = calculateSolarPosition(getExtTimePtr(), Latitude, Longitude);
		return result.azimuth * RAD_TO_DEG;
	} else {
		return 0;
	}
}

// Get Azimuth for specified time
//
float SolarPosition::getSolarAzimuth(time_ta t) {
	result = calculateSolarPosition(t, Latitude, Longitude);
	return result.azimuth * RAD_TO_DEG;
}

// Get current Solar distance in AU
//
float SolarPosition::getSolarDistance() {
	if (getExtTimePtr != nullptr) {
		result = calculateSolarPosition(getExtTimePtr(), Latitude, Longitude);
		return result.distance * KM_PER_AU;
	} else {
		return 0;
	}
}

// Get Solar distance in AU for specified time
//
float SolarPosition::getSolarDistance(time_ta t) {
	result = calculateSolarPosition(t, Latitude, Longitude);
	return result.distance * KM_PER_AU;
}

//
// *** end of public methods: ***

// *** beginning of utility functions ***
//

long JulianDate(int year, int month, int day) {
	long JD_whole;
	int A, B;
	if (month <= 2) {
		year--;
		month += 12;
	}
	A = year / 100;
	B = 2 - A + A / 4;
	JD_whole = (long) (365.25 * (year + 4716)) + (int) (30.6001 * (month + 1))
			+ day + B - 1524;
	return JD_whole;
}

SolarPosition_t calculateSolarPosition(time_ta tParam, float Latitude,
		float Longitude) {

	const float DAYS_PER_JULIAN_CENTURY = 36525.0;
	const long Y2K_JULIAN_DAY = 2451545;

	tmElements_t timeCandidate;
	static time_ta timePrevious = 0;
	static float latPrevious;
	static float lonPrevious;
	static SolarPosition_t result;

	long JD_whole;
	long JDx;
	float JD_frac;
	float rightAscension;
	float Declination;
	float hourAngle;
	float GreenwichHourAngle;
	float elapsedT;
	float solarLongitude;
	float solarMeanAnomaly;
	float earthOrbitEccentricity;
	float sunCenter;
	float solarTrueLongitude;
	float solarTrueAnomaly;
	float equatorObliquity;

	if (tParam != timePrevious or Latitude != latPrevious
			or Longitude != lonPrevious) // only calculate if time or location has changed
					{
		breakTime(tParam, timeCandidate);
		JD_whole = JulianDate(tmYearToCalendar(timeCandidate.Year),
				timeCandidate.Month, timeCandidate.Day);
		JD_frac = (timeCandidate.Hour + timeCandidate.Minute / 60.0
				+ timeCandidate.Second / 3600.0) / 24.0 - 0.5;

		elapsedT = JD_whole - Y2K_JULIAN_DAY;
		elapsedT = (elapsedT + JD_frac) / DAYS_PER_JULIAN_CENTURY;

		solarLongitude = DEG_TO_RAD
				* fmod(280.46645 + 36000.76983 * elapsedT, 360);
		solarMeanAnomaly = DEG_TO_RAD
				* fmod(357.5291 + 35999.0503 * elapsedT, 360);
		earthOrbitEccentricity = 0.016708617 - 0.000042037 * elapsedT;

		sunCenter = DEG_TO_RAD
				* ((1.9146 - 0.004847 * elapsedT) * sin(solarMeanAnomaly)
						+ (0.019993 - 0.000101 * elapsedT)
								* sin(2 * solarMeanAnomaly)
						+ 0.00029 * sin(3 * solarMeanAnomaly));

		solarTrueAnomaly = solarMeanAnomaly + sunCenter;
		equatorObliquity = DEG_TO_RAD
				* (23 + 26 / 60. + 21.448 / 3600. - 46.815 / 3600 * elapsedT);

		JDx = JD_whole - Y2K_JULIAN_DAY;

		GreenwichHourAngle = 280.46061837 + (360 * JDx) % 360
				+ .98564736629 * JDx + 360.98564736629 * JD_frac;
		GreenwichHourAngle = fmod(GreenwichHourAngle, 360.0);

		solarTrueLongitude = fmod(sunCenter + solarLongitude, TWO_PI);

		rightAscension = atan2(sin(solarTrueLongitude) * cos(equatorObliquity),
				cos(solarTrueLongitude));

		Declination = asin(sin(equatorObliquity) * sin(solarTrueLongitude));
		hourAngle = DEG_TO_RAD * GreenwichHourAngle + Longitude
				- rightAscension;

		// results:
		result.distance = 1.000001018
				* (1 - earthOrbitEccentricity * earthOrbitEccentricity)
				/ (1 + earthOrbitEccentricity * cos(solarTrueAnomaly));

		// elevation from the horizon
		result.elevation = asin(
				sin(Latitude) * sin(Declination)
						+ cos(Latitude) * (cos(Declination) * cos(hourAngle)));

		// Azimuth measured eastward from north.
		result.azimuth = PI
				+ atan2(sin(hourAngle),
						cos(hourAngle) * sin(Latitude)
								- tan(Declination) * cos(Latitude));

		// copy the time
		result.time = tParam;

		// remember the parameters
		timePrevious = tParam;
		latPrevious = Latitude;
		lonPrevious = Longitude;
	}
	return result;
}

// *** end of utility functions ***
