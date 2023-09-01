#include "control.h"

#include <unistd.h>
#include <thread>

#include "config.h"
#include "SunRise/SunRise.h"

#include "wan.h"

TControl control;

#define _ms(x)      (x*1000)
#define _s(x)       (_ms(x * 1000))

//
//
//
TControl::TControl()
{

}


//
//
//
void TControl::init()
{
    printf("control init\r\n");

    nextRunState(RunStates::Idle);

    runs = true;
    run();
}


//
//
//
void TControl::run()
{
    time_t nu = time(nullptr);
    printf("control run started @%s\r\n", ctime(&nu));

    while(runs) 
    {
        usleep(_ms(100));

        switch(runState)
        {
            case RunStates::Idle :
                onIdleWork();
                nextRunState(RunStates::CycleSetup);
            break;

            // get some info for this day
            case RunStates::CycleSetup :
            {  
                printf("Do stuff at start of cycle\n");
                switch(getSunRiseSet())
                {
                    // ?
                    case SunRiseSetEvent::NONE :
                    break;

                    // rise
                    case SunRiseSetEvent::RISE :    
                        // we must wait for the sun set time
                        printf("-> wait for day to start @ %.24s\r\n", ctime(&sunRiseTime));      
                        counter = 0;              
                        nextRunState(RunStates::CycleWaitForRise);                        
                    break;

                    // set
                    case SunRiseSetEvent::SET :
                        // we already had the sunrise event -> run and wait for sun set
                        printf("-> wait for day to end @ %.24s\r\n", ctime(&sunSetTime));
                        nextRunState(RunStates::CycleRun); 
                        t = 0;     
                    break;
                }
            }
            break;

            //
            case RunStates::CycleWaitForRise :
            {   
                time_t nu = time(nullptr);
                if( nu > sunRiseTime ) {
#if 0
                    // get sunSet
                    printf("%d\r\n", static_cast<uint8_t>(getSunRiseSet()));
                    printf("-> wait for day to end @ %.24s\r\n", ctime(&sunSetTime));


                    nextRunState(RunStates::DayRun); 
                    t = 0;
#endif                    
                    {  
                        switch(getSunRiseSet())
                        {
                            // ?
                            case SunRiseSetEvent::NONE :
                            break;

                            // rise
                            case SunRiseSetEvent::RISE :    
                                // we must wait for the sun set time
                                printf("-> wait for day to start @ %.24s\r\n", ctime(&sunRiseTime));                    
                                nextRunState(RunStates::CycleWaitForRise);                        
                            break;

                            // set
                            case SunRiseSetEvent::SET :
                                // we already had the sunrise event -> run and wait for sun set
                                printf("-> wait for day to end @ %.24s\r\n", ctime(&sunSetTime));
                                nextRunState(RunStates::CycleRun); 
                                t = 0;     
                            break;
                        }
                    }

                } else {
                    // update every minute or so to indicate we are still alive
                    counter--;
                    if(counter <= 0) {
                        printf("."); fflush(stdout);
                        counter = 600;
                    }
                }

            }
            break;

            //
            case RunStates::CycleRun :
                // day done ?
                if(time(nullptr) >= sunSetTime) {
                    nextRunState(RunStates::CycleEnd);
                    break;
                }

                // minute tasks
                switch(minute())
                {
                    case TTimeEvent::MINUTE :
                        printf("-\r\n");
                        protocol.sendTick();
                    break;

                    case TTimeEvent::RELOAD :
                        printf("reload\r\n");
                        protocol.sendPing();
                    break;
                }

            break;

            //
            case RunStates::CycleEnd :
                printf("do stuff at end of cycle\r\n");
                onCycleEnd();
                nextRunState(RunStates::Idle);
            break;
        }
    }

    printf("control thread stopped\r\n");
}


//
//
//
TControl::SunRiseSetEvent TControl::getSunRiseSet()
{
    time_t t = time(nullptr);
    auto utcOffset = +2;

    printf("getSunRiseSet\n");
    //t = 1693429200;
    printf("\tnu=  %.24s\n", ctime(&t));

    auto latitude = config.data.location.lat;
    auto longitude = config.data.location.lon;

    // Find the last and next sun set and rise.  
    SunRise sr;
    sr.calculate(latitude, longitude, t + utcOffset);

    // Returned values:
    bool sunVisible = sr.isVisible;
    bool sunHasRise = sr.hasRise;
    bool sunHasSet = sr.hasSet;
    float sunRiseAz = sr.riseAz;	      // Where the sun will rise/set in degrees from
    float sunSetAz = sr.setAz;	      // North.

    // Additional returned values requiring conversion from UTC to local time zone
    // on the Arduino.
    time_t sunQueryTime = sr.queryTime - utcOffset;
    sunRiseTime = sr.riseTime - utcOffset;
    sunSetTime = sr.setTime - utcOffset;
    //printf("\tr: %.24s \ts: %.25s\n", ctime(&sunRiseTime), ctime(&sunSetTime));

#if 0
    // Use the results as desired (use the utcOffset variables on the Arduino):
    printf("Sun rise/set nearest %.24s for latitude %.2f longitude %.2f:\n",
        ctime(&sr.queryTime), latitude, longitude);

    printf("Preceding event:\n");
    if ((!sr.hasRise || (sr.hasRise && sr.riseTime > sr.queryTime)) &&
        (!sr.hasSet || (sr.hasSet && sr.setTime > sr.queryTime)))
        printf("\tNo sun rise or set during preceding %d hours\n", SR_WINDOW/2);
    if (sr.hasRise && sr.riseTime < sr.queryTime)
        printf("\tSun rise at %.24s, Azimuth %.2f\n", ctime(&sr.riseTime), sr.riseAz);
    if (sr.hasSet && sr.setTime < sr.queryTime)
        printf("\tSun set at  %.24s, Azimuth %.2f\n", ctime(&sr.setTime), sr.setAz);

    printf("Succeeding event:\n");
    if ((!sr.hasRise || (sr.hasRise && sr.riseTime < sr.queryTime)) &&
        (!sr.hasSet || (sr.hasSet && sr.setTime < sr.queryTime)))
        printf("\tNo sun rise or set during succeeding %d hours\n", SR_WINDOW/2);
    if (sr.hasRise && sr.riseTime > sr.queryTime)
        printf("\tSun rise at %.24s, Azimuth %.2f\n", ctime(&sr.riseTime), sr.riseAz);
    if (sr.hasSet && sr.setTime > sr.queryTime)
        printf("\tSun set at  %.24s, Azimuth %.2f\n", ctime(&sr.setTime), sr.setAz);

    if (sr.isVisible)
        printf("Sun visible.\n");
    else
        printf("Sun not visible.\n");  
#endif

    //printf("\tnext event = \n");
    if ((!sr.hasRise || (sr.hasRise && sr.riseTime < sr.queryTime)) &&
        (!sr.hasSet || (sr.hasSet && sr.setTime < sr.queryTime))) {
        printf("\tNo sun rise or set during succeeding %d hours\n", SR_WINDOW/2);

        return SunRiseSetEvent::NONE;
    }

    if (sr.hasRise && sr.riseTime > sr.queryTime) {
        printf("\trise @ %.24s\n", ctime(&sr.riseTime));

        return SunRiseSetEvent::RISE;
    }
    if (sr.hasSet && sr.setTime > sr.queryTime) {
        printf("\tset @ %.24s\n", ctime(&sr.setTime));

        return SunRiseSetEvent::SET;
    }

    return SunRiseSetEvent::NONE;
      
}


//
//
//
void TControl::onIdleWork() 
{
    int t = 0;
    while(1) {
        if(t >= 30) {
            printf("\n");
            break;
        }

        printf("i"); fflush(stdout);
        t++;

        sleep(1);
    }
}


//
//
//
void TControl::onCycleEnd() 
{
    int t = 0;
    while(1) {
        if(t >= 30) {
            printf("\n");
            break;
        }

        printf("e"); fflush(stdout);
        t++;

        sleep(1);
    }
}

//
//
//
TControl::TTimeEvent TControl::minute()
{
#if 0	
  bool expired = false;
  struct timeval now;
  
  gettimeofday(&now, NULL);
  if(now.tv_sec == 0) {
	expired = true;
  } else {
	  printf("%d\r\n", now.tv_sec);
  }
  
  return expired;
#endif

  static uint8_t expiredstate = 0;

  const std::time_t now = std::time(nullptr);
  const std::tm calendar_time = *std::localtime(std::addressof(now));

  switch (expiredstate)
  {
  case 0:
      // 00:00:00
      if ((calendar_time.tm_sec == 16) &&
          (calendar_time.tm_min == 57) &&
          (calendar_time.tm_hour == 16))
      {
        expiredstate = 2;
        return TTimeEvent::RELOAD;
      }

    
      if (calendar_time.tm_sec == 0) 
      {
        expiredstate = 1;
        return TTimeEvent::MINUTE;
      }

    break;

  case 1:
      if (calendar_time.tm_sec == 2)
      {
        expiredstate = 0;
      }
    break;

  case 2:
      if (calendar_time.tm_sec == 18)
      {
        expiredstate = 0;
      }
    break;


  }

  return TTimeEvent::NONE;
}