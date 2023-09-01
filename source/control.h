#pragma once

#include <stdint.h>
#include <ctime>
#include <string>

//
//
//
class TControl {
    public :
        TControl();

        void init();
        void run();

        void close() {
            runs = false;
        }

    private :
        bool runs;
        int32_t counter;

        enum class RunStates {
            Idle,
            CycleSetup,
            CycleWaitForRise,
            CycleRun,
            CycleEnd
        };
        RunStates runState;

        void nextRunState(RunStates next) {
            printf("\n->");
            switch(next) {
                case RunStates::Idle : printf("Idle"); break;
                case RunStates::CycleSetup : printf("CycleSetup"); break;
                case RunStates::CycleWaitForRise : printf("CycleWaitForRise"); break;
                case RunStates::CycleRun : printf("CycleRun"); break;
                case RunStates::CycleEnd : printf("CycleEnd"); break;

                default : printf("UNKNOWN runstate !!");
            }
            printf("\n");
            runState = next;
        }

        int t;

        time_t sunRiseTime;
        time_t sunSetTime;



        enum class SunRiseSetEvent {
            NONE,
            RISE,
            SET
        };
        SunRiseSetEvent getSunRiseSet();



        enum class TTimeEvent {
            NONE,
            MINUTE,
            RELOAD
        };
        TTimeEvent minute();

        void onIdleWork();
        void onCycleEnd();
};

extern TControl control;