#pragma once

#include <stdint.h>

#include "printf.h"


//
//
//
class TProtocolPacketTx {
    public :
        uint8_t myAddress = 0xAB;
        
        enum class TTxMode : uint8_t {
            GET     = 0x02,
            SET     = 0x01
        };

        void setBuffer(uint8_t * buffer, uint16_t length) {
            this->buffer = buffer;
            this->length = length;
        }

        void begin(uint8_t AddressTo, uint8_t Command, uint8_t subCommand, TTxMode reply) {            
            buffer[idxAddressTo] = AddressTo | (reply == TTxMode::SET?0x80:0x00);
            buffer[idxAddressFrom] = myAddress;
            buffer[idxCommand] = Command;
            buffer[idxSubCommand] = subCommand;

            // set index to first byte of payload
            idx = cResetIndex;
        }

        void addUint8(uint8_t data) {
            buffer[idx++] = data;
        }

        void addUint16(uint16_t data) {
            buffer[idx++] = (uint8_t)(data >> 8);
            buffer[idx++] = (uint8_t)(data);
        }

        void addUint32(uint32_t data) {
            buffer[idx++] = (uint8_t)(data >> 24);
            buffer[idx++] = (uint8_t)(data >> 16);
            buffer[idx++] = (uint8_t)(data >> 8);
            buffer[idx++] = (uint8_t)(data);
        }

        uint16_t getLength() {
            return idx;
        }

        uint8_t * getBuffer() {
            return buffer;
        }


    private :
        uint8_t * buffer;
        uint16_t length;
        uint16_t idx;

        
        const uint8_t idxAddressTo      = 0;
        const uint8_t idxAddressFrom    = 1;
        const uint8_t idxCommand        = 2;
        const uint8_t idxSubCommand     = 3;

        const uint8_t cResetIndex       = 4;

        const uint8_t cAddressBroadcast = 0xFF;
        const uint8_t cAddressMask_RW   = 0x80;
};


//
//
//
class TProtocolPacketRx {
    public :
        //
        TProtocolPacketRx(uint8_t * buffer, uint16_t length) :
            buffer(buffer), length(length) {

            resetIndex();
        }

        //
        void setBuffer(uint8_t * buffer, uint16_t length) {
            this->buffer = buffer;
            this->length = length;

            resetIndex();
        }

        // set index to first byte of payload
        void resetIndex() {
            idx = cResetIndex;
        }

        //
        uint8_t getAddress() {
            return buffer[idxAddress] & ~cAddressMask_RW;
        }

        //
        bool isAddressValid(uint8_t address) {
            if(isBroadcast()) {
                return true;
            }

            if(address == getAddress()) {
                return true;
            }

            if((address |  cAddressMask_RW) == getAddress()) {
                return true;
            }

            return false;
        }

        //
        bool isRead() {
            return (buffer[idxAddress] & cAddressMask_RW) != cAddressMask_RW;
        }

        //
        bool isWrite() {
            return (buffer[idxAddress] & cAddressMask_RW) == cAddressMask_RW;
        }

        //
        bool isBroadcast() {
            return buffer[idxAddress] == cAddressBroadcast;
        }

        // command byte (idx = 0 in buffer)
        uint8_t getCommand() {
            return buffer[idxCommand];
        }

        // command byte (idx = 0 in buffer)
        uint8_t getSubCommand() {
            return buffer[idxSubCommand];
        }

        // length of payload, without address, command & sub command byte
        uint16_t getLength() {
            return length - cResetIndex;
        }

        // get 1 byte from payload and increment index
        uint8_t getUint8() {
            // in range ? 
            if((idx + 1) > length) { return 0; }

            return buffer[idx++];
        }

        // get 2 bytes from payload and increment index
        uint16_t getUint16() {
            // in range ? 
            if((idx + 2) > length) { return 0; }

            uint16_t v;
            v  = (buffer[idx++] << 8);
            v |=  buffer[idx++];

            return v;
        }

        // get 4 bytes from payload and increment index
        uint32_t getUint32() {
            // in range ? 
            if((idx + 4) > length) { return 0; }

            uint32_t v;
            v  = ((buffer[idx++] << 24) & 0xFF000000);
            v |= ((buffer[idx++] << 16) & 0x00FF0000);
            v |= ((buffer[idx++] <<  8) & 0x0000FF00);
            v |= ((buffer[idx++] <<  0) & 0x000000FF);

            return v;
        }


    private :
        uint8_t * buffer;
        uint16_t length;
        uint16_t idx;

        const uint8_t idxAddress        = 0;
        const uint8_t idxCommand        = 1;
        const uint8_t idxSubCommand     = 2;

        const uint8_t cResetIndex       = 3;

        const uint8_t cAddressBroadcast = 0xFF;
        const uint8_t cAddressMask_RW   = 0x80;
};