#pragma once

#include "CRC16.h"
#include <stdint.h>

#include "ProtocolPacket.h"
#include "../Timer/timer.h"

//
//
//
class TBaseProtocol
{
    public:
      TBaseProtocol( int BufferSize = 128 );
      ~TBaseProtocol( );

      void Receive( int kar );

      int SendPacket( uint8_t * data, int size );
      
      int SendPacket(TProtocolPacketTx &txPacket) {
        return SendPacket(txPacket.getBuffer(), txPacket.getLength());
      }

      void sendAck();
      void sendNack();

      enum class COMM : uint8_t {
        COM   = 0x60,
        STX   = 0x61,
        ETX   = 0x62,
        ESC   = 0x63,
      };

      enum class RXSTATE : uint8_t {
        IDLE,
        START,
        DATA,
        ESCAPE
      };
      

    protected:       
      int                                   FMaxPacketSize;

      virtual void HandlePacket( uint8_t* buffer, int size ) = 0;
      virtual void Send( uint8_t* buffer, int size ) = 0;

      void AddByteCrc( uint8_t data );
      void PutByte( uint8_t data );

    private  :
      RXSTATE                               FRxState;
      int                                   FRxIndex;
      TCrc16 *                              FRxCrc;
      uint8_t *                             FRxBuffer;    
      TTimer                                rxTimeout;

      int                                   FTxIndex;
      TCrc16 *                              FTxCrc;
      uint8_t *                             FTxBuffer;
};

