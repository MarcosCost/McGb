#include "bus.h"

int8_t read_memory(uint16_t adress){
    return memory[adress];
}

void write_memory(int8_t word, uint16_t adress){
    memory[adress] =  word;
}