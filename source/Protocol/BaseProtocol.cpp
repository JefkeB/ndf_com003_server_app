// 
// [COM][STX] [HEADER] [DATA] [COMM][ETX]
// [HEADER] : [ADDRESS]
// [DATA]   : [PAYLOAD]
//
//
// ADDRESS  : 0x00..0x7F    
//       D7 : 0x00  Read    (Get/Query)
//          : 0x80  Write   (Set)

#include "BaseProtocol.h"




//
//
//
TBaseProtocol::TBaseProtocol(int BufferSize ) 
{
    FRxState = RXSTATE::IDLE;

    FMaxPacketSize = BufferSize;

    FRxBuffer = new uint8_t[FMaxPacketSize]; 
    FRxCrc    = new TCrc16();

    FTxBuffer = new uint8_t[FMaxPacketSize];   
    FTxCrc    = new TCrc16();

    rxTimeout.start(25);
    
}


//
//
//
TBaseProtocol::~TBaseProtocol( )
{
    delete   FRxCrc;
    delete[] FRxBuffer;

    delete   FTxCrc;
    delete[] FTxBuffer;
}

#include "printf.h"
//
//
//
void TBaseProtocol::Receive( int kar )
{
    if(kar == -1) {
        return;
    }
    //printf("%02x[%c] ", kar, kar);

    if(rxTimeout.expired()) {
        rxTimeout.reStart();
        FRxState = RXSTATE::IDLE;
    }
    

    switch( FRxState )
    {
    case RXSTATE::IDLE : 
        if( kar == static_cast<uint8_t>(COMM::COM) )
        {
            FRxState = RXSTATE::START;
        }
        break;

    case RXSTATE::START : 
        if( kar == static_cast<uint8_t>(COMM::STX) )
        {
            FRxState = RXSTATE::DATA;
            FRxIndex = 0;

            FRxCrc->Clear();
        }
        else
        {
            FRxState = RXSTATE::IDLE;
        }
        break;

    case RXSTATE::DATA : 
        if( kar == static_cast<uint8_t>(COMM::COM) )
        {
            FRxState = RXSTATE::ESCAPE;
        }
        else
        {
            FRxBuffer[ FRxIndex++ ] = (uint8_t)kar;
            FRxCrc->Add( (uint8_t)kar );

            if( FRxIndex > FMaxPacketSize )
            {
                FRxState = RXSTATE::IDLE;
            }
        }
        break;


    case RXSTATE::ESCAPE : 
        if( kar == static_cast<uint8_t>(COMM::ESC) )             // byte stuffing removal
        {
            FRxBuffer[ FRxIndex++ ] = static_cast<uint8_t>(COMM::COM);
            FRxCrc->Add( static_cast<uint8_t>(COMM::COM) );

            if( FRxIndex > FMaxPacketSize )
            {
                FRxState = RXSTATE::IDLE;
                return;
            }

            FRxState = RXSTATE::DATA;
        }

        if( kar == static_cast<uint8_t>(COMM::ETX) )             // end of frame
        {

            if( FRxCrc->Crc() == 0 )
            {
                HandlePacket( FRxBuffer, FRxIndex -2 );
            }
            else
            {
                // crc fout
            }

            FRxState = RXSTATE::IDLE;
            return;
        }
        break;


        default : 
            FRxState = RXSTATE::IDLE;
    }

}


//
//
//
void TBaseProtocol::AddByteCrc( uint8_t data )
{  
    FTxBuffer[ FTxIndex++ ] = data;
    FTxCrc->Add( data );

    if( data == static_cast<uint8_t>(COMM::COM) )
    {
        FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::ESC);
    }
}    


//
//
//
void TBaseProtocol::PutByte( uint8_t data )
{  
    FTxBuffer[ FTxIndex++ ] = data;

    if( data == static_cast<uint8_t>(COMM::COM) )
    {
        FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::ESC);
    }
}


//
//
//
int TBaseProtocol::SendPacket( uint8_t * data, int size )
{
    FTxIndex = 0;
    FTxCrc->Clear();

    FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::COM);
    FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::STX);

    for( int idx = 0; idx < size; idx++ )
    {
        AddByteCrc( *data );
        data++;
    }

    PutByte( FTxCrc->CrcHi() );
    PutByte( FTxCrc->CrcLo() );

    FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::COM);
    FTxBuffer[ FTxIndex++ ] = static_cast<uint8_t>(COMM::ETX);

    // DumpHex( FTxBuffer, FTxIndex );
    Send( FTxBuffer, FTxIndex );

    return FTxIndex ;
}


//
//
//
void TBaseProtocol::sendAck()
{
    uint8_t buffer[1] = { 0x02 };
    SendPacket(buffer, 1);
}


//
//
//
void TBaseProtocol::sendNack()
{
    uint8_t buffer[1] = { 0x03 };
    SendPacket(buffer, 1);
}
