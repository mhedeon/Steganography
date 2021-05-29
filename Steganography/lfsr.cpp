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
    // std::cout << (int)byte << std::endl;
    lfsr = ((( (lfsr>>0)^(lfsr>>2)^(lfsr>>11) ) & 1 ) << 11 ) | (lfsr>>1);
    byte ^= (char)(lfsr & 1);
    
    // std::cout << lfsr << std::endl;
    // std::cout << (int)byte << std::endl;
    // std::cout << "----------------------" << std::endl;

    return byte;
}
