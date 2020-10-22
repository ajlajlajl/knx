#include "bits.h"
#include <cstring> // for memcpy()

const uint8_t* popByte(uint8_t& b, const uint8_t* data)
{
    b = *data;
    data += 1;
    return data;
}

void printHex(const char* suffix, const uint8_t *data, size_t length)
{
    print(suffix);
    for (size_t i = 0; i < length; i++) {
        if (data[i] < 0x10) { print("0"); }
        print(data[i], HEX);
        print(" ");
    }
    println();
}

const uint8_t* popWord(uint16_t& w, const uint8_t* data)
{
    w = getWord(data);
    data += 2;
    return data;
}

const uint8_t* popInt(uint32_t& i, const uint8_t* data)
{
    i = getInt(data);
    data += 4;
    return data;
}

const uint8_t* popByteArray(uint8_t* dst, uint32_t size, const uint8_t* data)
{
    for (uint32_t i = 0; i < size; i++)
        dst[i] = data[i];

    data += size;
    return data;
}

uint8_t* pushByte(uint8_t b, uint8_t* data)
{
    data[0] = b;
    data += 1;
    return data;
}

uint8_t* pushWord(uint16_t w, uint8_t* data)
{
    data[0] = ((w >> 8) & 0xff);
    data[1] = (w & 0xff);
    data += 2;
    return data;
}

uint8_t* pushInt(uint32_t i, uint8_t* data)
{
    data[0] = ((i >> 24) & 0xff);
    data[1] = ((i >> 16) & 0xff);
    data[2] = ((i >> 8) & 0xff);
    data[3] = (i & 0xff);
    data += 4;
    return data;
}

uint8_t* pushByteArray(const uint8_t* src, uint32_t size, uint8_t* data)
{
    for (uint32_t i = 0; i < size; i++)
        data[i] = src[i];
    
    data += size;
    return data;
}

uint16_t getWord(const uint8_t* data)
{
    return (data[0] << 8) + data[1];
}

uint32_t getInt(const uint8_t * data)
{
    return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
}

void sixBytesFromUInt64(uint64_t num, uint8_t* toByteArray)
{
    toByteArray[0] = ((num >> 40) & 0xff);
    toByteArray[1] = ((num >> 32) & 0xff);
    toByteArray[2] = ((num >> 24) & 0xff);
    toByteArray[3] = ((num >> 16) & 0xff);
    toByteArray[4] = ((num >> 8) & 0xff);
    toByteArray[5] = (num & 0xff);
}

uint64_t sixBytesToUInt64(uint8_t* data)
{
    uint64_t l = 0;

    for (uint8_t i = 0; i < 6; i++)
    {
        l = (l << 8) + data[i];
    }
    return l;
}

const uint16_t eMask = 0x7800; //01111000 00000000
const uint16_t mMask = 0x87FF; //10000111 11111111
const uint16_t sMask = 0x8000; //10000000 00000000

float Decode_DPT9_Float(uint16_t n)
{
    uint16_t e = n & eMask;
    uint16_t m = n & mMask;
    int16_t sm = m;
    if (m & sMask)
    {
        m = (~m & mMask) + 1;
        sm = -(int)m;
    }
    return (0.01 * (float)sm) * pow(2, e);
}

void memcpyInverted(uint8_t* dst, uint8_t* src, size_t length)
{
    for (int i = 0; i < length; i++)
    {
        dst[i] = src[length - i - 1];
    }
}

// The CRC of the Memory Control Block Table Property is a CRC16-CCITT with the following
// parameters:
// Width = 16 bit
// Truncated polynomial = 1021h
// Initial value = FFFFh
// Input date is NOT reflected.
// Output CRC is NOT reflected.
// No XOR is performed on the output CRC.
// EXAMPLE The correct CRC16-CCITT of the string ‘123456789’ is E5CCh.

uint16_t crc16Ccitt(uint8_t* input, uint16_t length)
{
        uint32_t polynom = 0x1021;
        uint8_t padded[length+2];

        memcpy(padded, input, length);
        memset(padded+length, 0x00, 2);

        uint32_t result = 0xffff;
        for (uint32_t i = 0; i < 8 * (uint32_t)sizeof(padded); i++) {
            result <<= 1;
            uint32_t nextBit = (padded[i / 8] >> (7 - (i % 8))) & 0x1;
            result |= nextBit;
            if ((result & 0x10000) != 0)
                result ^= polynom;
        }
        return result & 0xffff;
}
