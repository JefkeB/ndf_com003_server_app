//
//
//
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>

#include "Ini/SimpleIni.h"
#include "wan.h"
#include "config.h"

#include <thread>
#include "control.h"

using namespace std;

bool runs = true;



//
//
//
void changemode(int dir)
{
  static struct termios oldt, newt;

  if (dir == 1)
  {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


//
//
//
int _kbhit(void)
{
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET(STDIN_FILENO, &rdfs);

  select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}


//
//
//
void my_handler(int s)
{
  printf("Caught signal %d\n", s);

  control.close();
  protocol.close();
  runs = false;
}


//
//
//
void sigHandler()
{
  // ctrl+C
  signal(SIGINT, my_handler);
  // termination request kill
  signal(SIGTERM, my_handler);
}

#if 0
//
struct timeval startTime;

void TimerStart() { gettimeofday(&startTime, NULL); }

//
//
//
double TimerElapsed(struct timeval stamp)
{
  struct timeval now;

  gettimeofday(&now, NULL);

  double dt = (now.tv_sec - stamp.tv_sec) * 1000.0;
  dt += (now.tv_usec - stamp.tv_usec) / 1000.0;

  return dt;
}
#endif



//
//
//
int main(int argc, char *argv[])
{
  sigHandler();

  if(config.load("collector.cfg") == false) {
    printf("Failed to open config file !!\r\n");
    return 1;
  }

  if (!protocol.open(config.data.serialPort))
  {
    fprintf(stderr, "Can not open comport\n");
    exit(0);
  }

  printf("Wan port open\r\n");

  changemode(1);

  
  std::thread thrd{&TControl::init, &control};

  while (runs)
  {
    protocol.takt();

    usleep(1 * 1000); // sleep for 10 milliSeconds

    if (_kbhit())
    {
      auto kar = getchar();
      switch (kar)
      {
      case 'P':
        printf("ping\r\n");
        protocol.sendPing();
        break;

      case 'T':
        printf("tick\r\n");
        protocol.sendTick();
        break;
      }
    }  
  }

  changemode(0);


  //
  thrd.join();

  printf("bye\n");

  return 0;
}

