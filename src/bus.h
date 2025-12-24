// Header file for the Bus component
// This defines the 64kB memory and the read write functionality of these
#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>

// This allocates exactly 64KB on the stack (if local) or data segment (if global)
// If I include this header in more than 1 file inline will make sure only one memory segment is created
inline std::array<int8_t, 65536> memory;

// Reads the 8 bit word in the specified adress
int8_t read_memory(uint16_t adress);

// Writes an 8 bit word into the specified memory adress
void write_memory(int8_t word, uint16_t adress);

#endif