#pragma once 

#include <stdint.h>
//#include "systick.h"

class TTimer {
    public :        
        void start(uint32_t interval) {
            this->interval = interval;  
            //last = millis;             
        }

        void reStart() {
            //last = millis;             
        }

        uint8_t expired() {
            //return ((millis - last) >= interval) ?1:0;
            return false;
        }

        void reset() {
            last += (interval + 1);
        }
        

    private :     
        uint32_t last;
        uint32_t interval = 1000;
};
