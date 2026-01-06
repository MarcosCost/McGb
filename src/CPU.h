#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <iostream>
#include <optional>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "bus.h"

#define RESETT       "\033[0m"
#define RED         "\033[31m"      // For "Error"
#define GREEN_TEAL  "\033[36m"      // For Types (Instruction, CPU)
#define YELLOW      "\033[33m"      // For Methods (Execute)


struct FlagsRegister    //Custom data type for flags register that avoids acidental manipulation of the lower 4 bits
{
    bool zero;          // Bit 7
    bool subtraction;   // Bit 6
    bool half_carry;    // Bit 5
    bool carry;         // Bit 4

    uint8_t bool_to_uint(){
        return  (uint8_t)((uint8_t)zero << 7 | (uint8_t)subtraction << 6 | (uint8_t)half_carry << 5 | (uint8_t)carry << 4); 
    }

    FlagsRegister uint8_t_to_bool(uint8_t u8){
        FlagsRegister ret;
        ret.zero = (bool)(u8 >> 7);
        ret.subtraction = (bool)( (u8 >> 6) & 1 );
        ret.half_carry = (bool)( (u8 >> 5) & 1 );
        ret.carry = (bool)( (u8 >> 4) & 1 );
        return ret;
    }
};

struct Registers {
    // "Physical" Registers
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    FlagsRegister f;      // Special register for flags
    uint8_t h;
    uint8_t l;

    // Virtual Registers (“af”, “bc”, “de”, “hl” )
    
    /*  //Commented this out cause aparently the f register is a special register used for flags, so 
        //I believe I cannot alter it therefore rendering the virtual register "af" useless, in any case 
        //I'd rather have the code here to avoid busywork of copying it and altering var names, should I end up using it

        //Also the f register has suffered custom type alterarion in order to make it error proof so these wont work without refactoring the code

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

enum class ArithmeticTarget {
    a,b,c,d,e,h,l,
    bc,de,hl,hl_notadress,sp,
    Null
};
enum InstructionType {
    ADD,    //DONE
    ADDHL,  //DONE
    ADC,    //DONE
    SUB,    //DONE
    SBC,    //DONE
    AND,    //DONE
    OR,     //DONE    
    XOR,    //DONE
    CP,     //DONE
    INC,    //DONE
    DEC,    //DONE
    CCF,    //DONE
    SCF,    //DONE
    RRA,    //DONE
    RLA,    //DONE
    RRCA,   //DONE
    RRLA,   //DONE
    CPL,    //DONE
    BIT,    //DONE
    RESET,
    SET,
    SRL,
    RR,
    RL,
    RRC,
    RLC,
    SRA,
    SLA,
    SWAP
};
struct Instruction {
    InstructionType Type;
    ArithmeticTarget Target;
    std::optional<int8_t> d8; // Only has a value if Target is "NULL" or BIT operations

    // Keep the constructor private to force use of the factory methods
    private:
        Instruction(InstructionType t, ArithmeticTarget tg, std::optional<int8_t> imm) 
            : Type(t), Target(tg), d8(imm) {}

    public:
        // Factory for register-based instructions (e.g., ADD A, B)
        static Instruction InstructionWithTargetRegister(InstructionType t, ArithmeticTarget tg) {
            return Instruction(t, tg, std::nullopt);
        }

        // Factory for immediate-value instructions (e.g., ADD A, d8)
        static Instruction InstructionWithImmediate(InstructionType t, int8_t val) {
            return Instruction(t, ArithmeticTarget::Null, val);
        }

};

//Simulated CPU
struct CPU {
    Registers reg;
    uint16_t PC;
    uint16_t SP;

    void execute(Instruction instruction) {
        switch (instruction.Type) {
            case InstructionType::ADD : {
                switch (instruction.Target) {
                    case ArithmeticTarget::a : {
                        add(reg.a);
                        break;
                    }
                    case ArithmeticTarget::b : {
                        add(reg.b);
                        break;
                    }
                    case ArithmeticTarget::c : {
                        add(reg.c);
                        break;
                    }
                    case ArithmeticTarget::d : {
                        add(reg.d);
                        break;  
                    }
                    case ArithmeticTarget::e : {
                        add(reg.e);
                        break;
                    }
                    case ArithmeticTarget::h : {
                        add(reg.h);
                        break;
                    }
                    case ArithmeticTarget::l : {
                        add(reg.l);
                        break;
                    }
                    case ArithmeticTarget::hl : {  //Add value in 16bit memory adress HL
                        add(read_memory( (reg.h << 8) & (reg.l) ));
                        break;
                    }
                    default: {
                        int8_t val;
                        if (instruction.d8.has_value()) {
                            val = instruction.d8.value(); // Safe access
                        } else { throw; }
                        add(val);
                        break;
                    }
                }
                break;
            }
            case InstructionType::ADDHL : {
                switch (instruction.Target){
                    case ArithmeticTarget::bc : {
                        addhl(reg.get_bc());
                        break;
                    }
                    case ArithmeticTarget::de : {
                        addhl(reg.get_de());
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        addhl(reg.get_hl());
                        break;
                    }
                    case ArithmeticTarget::sp : {
                        addhl(SP);
                        break;
                    }
                }
                break;
            }
            case InstructionType::ADC : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        adc(reg.a);
                        break;
                    }
                    case ArithmeticTarget::b : {
                        adc(reg.b);
                        break;
                    }
                    case ArithmeticTarget::c : {
                        adc(reg.c);
                        break;
                    }
                    case ArithmeticTarget::d : {
                        adc(reg.d);
                        break;
                    }
                    case ArithmeticTarget::e : {
                        adc(reg.e);
                        break;
                    }
                    case ArithmeticTarget::h : {
                        adc(reg.h);
                        break;
                    }
                    case ArithmeticTarget::l : {
                        adc(reg.l);
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        adc(read_memory(reg.get_hl()));
                        break;
                    }
                    default: {
                        int8_t val;
                        if (instruction.d8.has_value()) {
                            val = instruction.d8.value();
                        } else { throw; }
                        adc(val);
                        break;
                    }
                }
                break;
            }
            case InstructionType::SUB : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        reg.a = (uint8_t)0; // register a minus register a = 0 always
                        break;
                    }
                    case ArithmeticTarget::b : {
                        sub(reg.b);
                        break;
                    }
                    case ArithmeticTarget::c : {
                        sub(reg.c);
                        break;
                    }
                    case ArithmeticTarget::d : {
                        sub(reg.d);
                        break;
                    }
                    case ArithmeticTarget::e : {
                        sub(reg.e);
                        break;
                    }
                    case ArithmeticTarget::h : {
                        sub(reg.h);
                        break;
                    }
                    case ArithmeticTarget::l : {
                        sub(reg.l);
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        sub(read_memory(reg.get_hl()));
                        break;
                    }
                    default: {
                        int8_t val;
                        if (instruction.d8.has_value()) {
                            val = instruction.d8.value();
                        } else { throw; }
                        adc(val);
                        break;
                    }
                }
                break;
            }
            case InstructionType::SBC : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        reg.a = 0;
                        break;
                    }
                    case ArithmeticTarget::b : {
                        sbc(reg.b);
                        break;
                    }
                    case ArithmeticTarget::c : {
                        sbc(reg.c);
                        break;
                    }
                    case ArithmeticTarget::d : {
                        sbc(reg.d);
                        break;
                    }
                    case ArithmeticTarget::e : {
                        sbc(reg.e);
                        break;
                    }
                    case ArithmeticTarget::h : {
                        sbc(reg.h);
                        break;
                    }
                    case ArithmeticTarget::l : {
                        sbc(reg.l);
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        sbc(read_memory(reg.get_hl()));
                        break;
                    }
                    default: {
                        int8_t val;
                        if(instruction.d8.has_value()){
                            val = instruction.d8.value();
                        } else { throw; }
                        sbc(val);
                        break;
                    }
                }
            }
            case InstructionType::AND : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        break; // a & a = a aka no alterations
                    }
                    case ArithmeticTarget::b : {
                        reg.a = reg.a & reg.b;
                        break;
                    }
                    case ArithmeticTarget::c : {
                        reg.a = reg.a & reg.c;
                        break;
                    }
                    case ArithmeticTarget::d : {
                        reg.a = reg.a & reg.d;
                        break;
                    }
                    case ArithmeticTarget::e : {
                        reg.a = reg.a & reg.e;
                        break;
                    }
                    case ArithmeticTarget::h : {
                        reg.a = reg.a & reg.h;
                        break;
                    }
                    case ArithmeticTarget::l : {
                        reg.a = reg.a & reg.l;
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        reg.a = reg.a & read_memory(reg.get_hl());
                        break;
                    }
                    default: {
                        reg.a = reg.a & instruction.d8.value();
                        break;
                    }
                }
            }
            case InstructionType::OR : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        break; // a | a = a aka no alterations
                    }
                    case ArithmeticTarget::b : {
                        reg.a = reg.a | reg.b;
                        break;
                    }
                    case ArithmeticTarget::c : {
                        reg.a = reg.a | reg.c;
                        break;
                    }
                    case ArithmeticTarget::d : {
                        reg.a = reg.a | reg.d;
                        break;
                    }
                    case ArithmeticTarget::e : {
                        reg.a = reg.a | reg.e;
                        break;
                    }
                    case ArithmeticTarget::h : {
                        reg.a = reg.a | reg.h;
                        break;
                    }
                    case ArithmeticTarget::l : {
                        reg.a = reg.a | reg.l;
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        reg.a = reg.a | read_memory(reg.get_hl());
                        break;
                    }
                    default: {
                        reg.a = reg.a | instruction.d8.value();
                        break;
                    }
                }
            }
            case InstructionType::XOR : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        reg.a = reg.a ^ reg.a;
                        break;
                    }
                    case ArithmeticTarget::b : {
                        reg.a = reg.a ^ reg.b;
                        break;
                    }
                    case ArithmeticTarget::c : {
                        reg.a = reg.a ^ reg.c;
                        break;
                    }
                    case ArithmeticTarget::d : {
                        reg.a = reg.a ^ reg.d;
                        break;
                    }
                    case ArithmeticTarget::e : {
                        reg.a = reg.a ^ reg.e;
                        break;
                    }
                    case ArithmeticTarget::h : {
                        reg.a = reg.a ^ reg.h;
                        break;
                    }
                    case ArithmeticTarget::l : {
                        reg.a = reg.a ^ reg.l;
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        reg.a = reg.a ^ read_memory(reg.get_hl());
                        break;
                    }
                    default: {
                        reg.a = reg.a ^ instruction.d8.value();
                        break;
                    }
                }
            }
            case InstructionType::CP : {
                switch (instruction.Target){
                    case ArithmeticTarget::a : {
                        compare(reg.a);
                        break;
                    }
                    case ArithmeticTarget::b : {
                        compare(reg.b);
                        break;
                    }
                    case ArithmeticTarget::c : {
                        compare(reg.c);
                        break;
                    }
                    case ArithmeticTarget::d : {
                        compare(reg.d);
                        break;
                    }
                    case ArithmeticTarget::e : {
                        compare(reg.e);
                        break;
                    }
                    case ArithmeticTarget::h : {
                        compare(reg.h);
                        break;
                    }
                    case ArithmeticTarget::l : {
                        compare(reg.l);
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        compare(read_memory(reg.get_hl()));
                        break;
                    }
                    default: {
                        compare(instruction.d8.value());
                        break;
                    }
                        
                }
            }
            case InstructionType::INC : {
                switch (instruction.Target) {
                    case ArithmeticTarget::a : {
                        reg.a++;
                        break;
                    }
                    case ArithmeticTarget::b : {
                        reg.b++;
                        break;
                    }
                    case ArithmeticTarget::c : {
                        reg.c++;
                        break;
                    }
                    case ArithmeticTarget::d : {
                        reg.d++;
                        break;
                    }
                    case ArithmeticTarget::e : {
                        reg.e++;
                        break;
                    }
                    case ArithmeticTarget::h : {
                        reg.h++;
                        break;
                    }
                    case ArithmeticTarget::l : {
                        reg.l++;
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        uint16_t hl = reg.get_hl();
                        write_memory(hl, read_memory(hl) + 1);
                        break;
                    }
                    case ArithmeticTarget::bc : {
                        reg.set_bc(reg.get_bc() + 1);
                        break;
                    }
                    case ArithmeticTarget::de : {
                        reg.set_de(reg.get_de() + 1);
                        break;
                    }
                    case ArithmeticTarget::sp : {
                        SP++;
                        break;
                    }
                    case ArithmeticTarget::hl_notadress : {
                        reg.set_hl(reg.get_hl() + 1);
                        break;
                    }
                    default : {
                        spdlog::error("Invalid target for INC instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                        break;
                    }
                }
                break;
            }
            case InstructionType::DEC : {
                switch (instruction.Target) {
                    case ArithmeticTarget::a : {
                        reg.a--;
                        break;
                    }
                    case ArithmeticTarget::b : {
                        reg.b--;
                        break;
                    }
                    case ArithmeticTarget::c : {
                        reg.c--;
                        break;
                    }
                    case ArithmeticTarget::d : {
                        reg.d--;
                        break;
                    }
                    case ArithmeticTarget::e : {
                        reg.e--;
                        break;
                    }
                    case ArithmeticTarget::h : {
                        reg.h--;
                        break;
                    }
                    case ArithmeticTarget::l : {
                        reg.l--;
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        uint16_t hl = reg.get_hl();
                        write_memory(hl, read_memory(hl) - 1);
                        break;
                    }
                    case ArithmeticTarget::bc : {
                        reg.set_bc(reg.get_bc() - 1);
                        break;
                    }
                    case ArithmeticTarget::de : {
                        reg.set_de(reg.get_de() - 1);
                        break;
                    }
                    case ArithmeticTarget::sp : {
                        SP--;
                        break;
                    }
                    case ArithmeticTarget::hl_notadress : {
                        reg.set_hl(reg.get_hl() - 1);
                        break;
                    }
                    default : {
                        spdlog::error("Invalid target for DEC instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                        break;
                    }
                }
                break;
            }
            case InstructionType::CCF : {
                reg.f.carry = !reg.f.carry;
                break;
            }
            case InstructionType::SCF : {
                reg.f.carry = true;
                break;
            }
            case InstructionType::RRA : {
                uint8_t bit0 = (reg.a & 0x01);           // Capture bit that will fall off
                reg.a = (reg.a >> 1) | (reg.f.carry << 7); // Shift and pull in OLD carry
                reg.f.carry = bit0;
                break;
            }
            case InstructionType::RLA : {
                uint8_t bit7 = (reg.a >> 7) & 0x01;      // Capture bit that will fall off
                reg.a = (reg.a << 1) | reg.f.carry;      // Shift and pull in OLD carry
                reg.f.carry = bit7;
                break;
            }
            case InstructionType::RRCA : {
                reg.f.carry = (reg.a & 0x01);            // Bit 0 goes to carry
                reg.a = (reg.a >> 1) | (reg.f.carry << 7);;
                break;
            }    
            case InstructionType::RRLA : {
                reg.f.carry = (reg.a >> 7) & 0x01;       // Bit 7 goes to carry
                reg.a = (reg.a << 1) | reg.f.carry;
                break;
            }
            case InstructionType::CPL : {
                reg.a = ~reg.a;
                break;
            }
            case InstructionType::BIT : {
                switch (instruction.Target) {
                    case ArithmeticTarget::a : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.a & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.a & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.a & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.a & 0x08) != 0;
                                break;
                            }
                            case 4 : {
                                reg.f.zero = (reg.a & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.a & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.a & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.a & 0x80) != 0;
                                break;
                            }
                            default : { 
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }

                        }
                        break;  
                    }
                    case ArithmeticTarget::b : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.b & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.b & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.b & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.b & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.b & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.b & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.b & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.b & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::c : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.c & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.c & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.c & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.c & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.c & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.c & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.c & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.c & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::d : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.d & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.d & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.d & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.d & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.d & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.d & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.d & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.d & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::e : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.e & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.e & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.e & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.e & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.e & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.e & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.e & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.e & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::h : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.h & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.h & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.h & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.h & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.h & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.h & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.h & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.h & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::l : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (reg.l & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (reg.l & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (reg.l & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (reg.l & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (reg.l & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (reg.l & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (reg.l & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (reg.l & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                    case ArithmeticTarget::hl : {
                        switch (instruction.d8.value()) {
                            case 0 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x01) != 0;
                                break;
                            }
                            case 1 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x02) != 0;
                                break;
                            }
                            case 2 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x04) != 0;
                                break;
                            }
                            case 3 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x08) != 0;
                                break;
                            } 
                            case 4 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x10) != 0;
                                break;
                            }
                            case 5 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x20) != 0;
                                break;
                            }
                            case 6 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x40) != 0;
                                break;
                            }
                            case 7 : {
                                reg.f.zero = (read_memory(reg.get_hl()) & 0x80) != 0;
                                break;
                            }
                            default : {
                                spdlog::error("Invalid bit number for BIT instruction. |-> {}, line {}", __FILE_NAME__, __LINE__);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            default: {
                // No matching instruction
                // Log using spdlog
                spdlog::error("Em CPU::execute: Tipo de instrucao desconhecida ou nao implementada: {} |-> {}, line {}", (int)instruction.Type, __FILE_NAME__, __LINE__);
                break;
            }
            }
        
        }

    void add(uint8_t value) {
        uint8_t a = reg.a;
        uint16_t result = (uint16_t)a + (uint16_t)value;
        uint8_t new_value = (uint8_t)result;

        // Set Flags
        reg.f.zero = (new_value == 0);
        reg.f.subtraction = false;
        // Half-carry: check if there's a carry from bit 3 to bit 4
        reg.f.half_carry = ((a & 0xF) + (value & 0xF)) > 0xF;
        reg.f.carry = result > 0xFF;

        reg.a = new_value;    
    }
    void addhl(uint16_t value) {
        uint16_t hl = reg.get_hl();
        uint32_t result = (uint32_t)hl + (uint32_t)value;

        // Set Flags
        reg.f.zero = (result == 0);
        reg.f.subtraction = false;
        // Half-carry: check if there's a carry from bit 3 to bit 4
        reg.f.half_carry = ((hl & 0xF) + (value & 0xF)) > 0xF;
        reg.f.carry = result > 0xFFFF;

        reg.set_hl((uint16_t)result);
    }
    void adc(uint8_t value){
        uint8_t a = reg.a;
        uint8_t carry = reg.f.carry ? 1:0;
        uint16_t result = (uint16_t)a + (uint16_t)value + (uint16_t)1;

        reg.a = (uint8_t)result;

        //Flags
        reg.f.zero = (reg.a == 0);
        reg.f.subtraction = false;
        reg.f.carry = result > 0xFF;
        reg.f.half_carry = ( (a & 0xF) + (value & 0xF) + 1 ) > 0xF;
    }
    void sub(uint8_t value){
        reg.a = reg.a - value;
        reg.f.zero = (reg.a == 0);
        reg.f.subtraction = true;
    }
    void sbc(uint8_t value){
        reg.a = reg.a - value - (bool)reg.f.carry;
        reg.f.zero = (reg.a == 0);
        reg.f.subtraction = true;
    }
    void compare(uint8_t value){
        uint8_t a = reg.a;
        sub(value);
        if (reg.a = 0x0) {
            reg.f.zero = true;
        } else {
            reg.f.zero = false;
        }
        reg.f.subtraction = true;
        reg.a = a;
    }
};

#endif