#ifndef CPU_H
#define CPU_H

#include <cstdint>

struct FlagsRegister    //Custom data type for flags register that avoids acidental manipulation of the lower 4 bits
{

    bool zero;          // Bit 7
    bool subtraction;   // Bit 6
    bool half_carry;    // Bit 5
    bool carry;         // Bit 4

    uint8_t bool_to_uint(){
        
    }

};


struct Registers {
    // "Physical" Registers
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;      // Special register for flags
    uint8_t h;
    uint8_t l;

    // Virtual Registers (“af”, “bc”, “de”, “hl” )
    
    /*  //Commented this out cause aparently the f register is a special register used for flags, so 
        //I believe I cannot alter it therefore rendering the virtual register "af" useless, in any case 
        //I'd rather have the code here to avoid busywork of copying it and altering var names, should I end up using it

    uint16_t get_af() {
        // Mask register "a" like a+00000000; Left shift "a" 8 times (multiply by 2^8 aka 256)
        uint16_t A_mask = (uint16_t)a * 256; 
        return A_mask | f;
    }
    void set_af(uint16_t value){
        // Set "a" to the first 8 bits of value and f to the last 8 bits
        a = (uint8_t)(value / 256);
        f = (uint8_t)(value & 0x00FF);
    }

    */

    uint16_t get_bc() {
        uint16_t B_mask = (uint16_t)b * 256; 
        return B_mask | c;
    }
    void set_bc(uint16_t value){
        b = (uint8_t)(value / 256);
        c = (uint8_t)(value & 0x00FF);
    }

    uint16_t get_de() {
        uint16_t D_mask = (uint16_t)d * 256; 
        return D_mask | e;
    }
    void set_de(uint16_t value){
        d = (uint8_t)(value / 256);
        e = (uint8_t)(value & 0x00FF);
    }

    uint16_t get_hl() {
        uint16_t H_mask = (uint16_t)h * 256; 
        return H_mask | l;
    }
    void set_hl(uint16_t value){
        h = (uint8_t)(value / 256);
        l = (uint8_t)(value & 0x00FF);
    }
};



#endif