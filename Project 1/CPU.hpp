#ifndef CPU_INCLUDED
#define CPU_INCLUDED

#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>

// Opcode types
#define ZERO 0x0  // Five in a row indicates the program has completed
#define RTYPE 0x33
#define ITYPE 0x13
#define LOADWORD 0x3
#define STOREWORD 0x23

//struct control {
//    uint8_t writeback;
//    uint8_t memory;
//    uint8_t rs2_current;
//};


class CPU
{
public:
    explicit CPU(std::vector<uint8_t>&& iMem, std::vector<uint8_t>&& dMem);
    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
    void clockTick();
    bool isFinished() const;
    void printStats();

private:
    std::vector<uint8_t> insMem;
    std::vector<uint8_t> dataMem;
    int32_t registerFile [32];
    uint32_t pc;
    unsigned short killCounter;
    unsigned int clockCount;

    enum class Op {
        ERROR,
        ZE,
        ADD,
        SUB,
        OR,
        AND,
        ADDI,
        ORI,
        ANDI,
        LW,
        SW,
    };

    struct IFID {
        uint32_t pc = 0;
        uint32_t instruction = 0;
    } ifidCurr, ifidNext;

    struct IDEX {
        uint32_t pc = 0;
        int32_t readData1 = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        int32_t immediate = 0;
        Op operation = Op::ZE;
    } idexCurr, idexNext;

    struct EXMEM {
        uint32_t pc = 0;
        int32_t aluResult = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        Op operation = Op::ZE;
    } exmemCurr, exmemNext;

    struct MEMWB {
        uint32_t rd = 0;
        int32_t aluResult = 0;
        int32_t memData = 0;
        Op operation = Op::ZE;
    } memwbCurr, memwbNext;
};


#endif //CPU_INCLUDED