#ifndef _COMMUNICATIONDEFINITIONS_H
#define _COMMUNICATIONDEFINITIONS_H


#include "CommonHeaders.h"

//FOR COMMUNICATION
struct LITENDIAN_REG{
    uint8_t byte1:8;       //7-0
    uint8_t byte2:8;       //15-8
    uint8_t byte3:8;       //23-16
    uint8_t byte4:8;       //31-24
};

union LITENDIAN_INT{
    uint32_t all;
    struct LITENDIAN_REG bytes;
};

union LITENDIAN_FLOAT{
    float all;
    struct LITENDIAN_REG bytes;
};

struct LIT_ENDIAN{
    union LITENDIAN_INT BE_INT;
    union LITENDIAN_FLOAT BE_FLOAT;
};





#endif