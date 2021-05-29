#include "lfsr.hpp"

LFSR::LFSR(uint16_t pass): lfsr(0)
{
    lfsr = pass;
}

LFSR::~LFSR()
{
}
#include <iostream>
char LFSR::getByte(char byte)
{
    lfsr = ((( (lfsr>>0)^(lfsr>>2)^(lfsr>>11) ) & 1 ) << 11 ) | (lfsr>>1);
    byte ^= (char)(lfsr & 1);

    return byte;
}
