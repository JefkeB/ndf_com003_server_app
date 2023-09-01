//---------------------------------------------------------------------------
#ifndef CRC16H
#define CRC16H
//---------------------------------------------------------------------------

#include <stdint.h>

class TCrc16
{
 
    public:   
        TCrc16();

        uint16_t crc;

        void Clear();
        void Add  ( uint8_t val );

        uint16_t Crc();
        uint8_t  CrcHi();
        uint8_t  CrcLo();

        void init();
        void calc( uint8_t x );

    private:
        uint16_t               FCrc;
};

#endif
