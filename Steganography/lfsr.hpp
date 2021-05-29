#pragma once

#include <cstdint>

class LFSR
{
public:
	LFSR(uint16_t);
	~LFSR();

    char getByte(char);

private:
    uint16_t lfsr;

};
