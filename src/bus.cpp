#include "bus.h"

int8_t read_memory(uint16_t address){
    return memory[address];
}

void write_memory(int8_t word, uint16_t address){
    memory[address] =  word;
}