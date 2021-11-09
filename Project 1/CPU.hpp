#ifndef CPU_INCLUDED
#define CPU_INCLUDED

#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <bitset>

// Opcode types
#define ZERO 0x0  // Five in a row indicates the program has completed
#define RTYPE 0x33
#define ITYPE 0x13
#define LOADWORD 0x3
#define STOREWORD 0x23


class CPU {
public:

    /**
     * The CPU constructor.
     *
     * This can be utilized to simulate a RISC-V pipelined CPU.
     * Only the instructions ADD, SUB, OR, AND, ADDI, ORI, ANDI,
     * LW, and SW working in this version. Branching, jumps,
     * bubbling, and forwarding will be added in future versions.
     *
     * @param a vector of unsigned bytes for instruction memory
     * @param a vector of unsigned bytes for data memory
     * @return a CPU object
     */
    explicit CPU(std::vector<uint8_t> &&iMem, std::vector<uint8_t> &&dMem);

    /**
     * The fetch stage of a CPU pipeline.
     *
     * This grabs 4 bytes from the instruction memory at the location of
     * the PC and proceeds to concatenate them into a single instruction.
     * In the concatenation process, this restructures the word from
     * little endian to big endian form.
     *
     * @param none
     * @return none
     */
    void fetch();

    /**
     * The Decode stage of a CPU pipeline.
     *
     * This is simulates the control block as well as the immediate
     * generator. It is easily the most complex part of this CPU
     * simulation as it must decode the binary instructions received
     * from the prior fetch cycle.
     *
     * @param none
     * @return none
     */
    void decode();

    /**
     * The Execute stage of a CPU pipeline.
     *
     * This is where the ALU functions and hence, all arithmetic and
     * logical operations are performed.
     *
     * @param none
     * @return none
     */
    void execute();

    /**
     * The Memory stage of a CPU pipeline.
     *
     * This handles accessing the data memory when load or store
     * instructions are run.
     *
     * @param none
     * @return none
     */
    void memory();

    /**
     * The Writeback stage of a CPU pipeline.
     *
     * Writes to the indicated register for r-type, i-type, and LW
     * instructions. Note that SW does not write to a register.
     *
     * @param none
     * @return none
     */
    void writeback();

    /**
     * Acts as a CPU cycle.
     *
     * This will update the "current" values for each stage to the
     * "next" values. This allows for accurate simulation of the
     * CPU pipeline.
     *
     * @param none
     * @return none
     */
    void clockTick();

    /**
     * Verify process completion.
     *
     * This will verify that zero ops have filled all stages of the CPU
     * pipeline. Additionally it will calculate metrics for display
     * before the program terminates.
     *
     * @param none
     * @return boolean showing if the program is done processing input.
     */
    bool isFinished();

    /**
     * Prints final metrics.
     *
     * Displays the final state of registers a0 and a1 to stdout.
     * Also displays the number of clock cycles, the number of r-type
     * instructions run and the IPC.
     *
     * @param none
     * @return none
     */
    void printStats();

private:
    std::vector<uint8_t> insMem;    // Instruction Memory
    std::vector<uint8_t> dataMem;   // Data Memory
    int32_t registerFile[32];       // Registers x0-x31
    uint32_t pc;                    // Program counter
    unsigned short killCounter;     // Number of sequencial ZERO op codes
    unsigned int clockCount;        // Number of clock cycles executed
    unsigned int rTypeCount;        // Number of r-type instructions executed
    unsigned int iTypeCount;        // Number of i-type instructions executed
    unsigned int swCount;           // Number of SW instructions executed
    unsigned int lwCount;           // Number of LW instructions executed
    unsigned int totalInsCount;     // Total instruction count
    double ipc;                     // Instructions per cycle

    // Acts as CPU control tracking the current operation.
    enum class Op {
        ERROR,                      // Unrecognized commands. Ignored.
        ZE,                         // ZERO op code
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

    // Information representing the register block between the Instruction
    // Fetch and Instruction Decode stages.
    struct IFID {
        uint32_t pc = 0;
        uint32_t instruction = 0;
    } ifidCurr, ifidNext; // Structs for both "current" and "next"

    // Information representing the register block between the Instruction
    // Decode and Execute stages.
    struct IDEX {
        uint32_t pc = 0;
        int32_t readData1 = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        int32_t immediate = 0;
        Op operation = Op::ZE;
    } idexCurr, idexNext; // Structs for both "current" and "next"

    // Information representing the register block between the Execute
    // and Memory stages.
    struct EXMEM {
        uint32_t pc = 0;
        int32_t aluResult = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        Op operation = Op::ZE;
    } exmemCurr, exmemNext; // Structs for both "current" and "next"

    // Information representing the register block between the Memory
    // and Writeback stages.
    struct MEMWB {
        uint32_t rd = 0;
        int32_t aluResult = 0;
        int32_t memData = 0;
        Op operation = Op::ZE;
    } memwbCurr, memwbNext; // Structs for both "current" and "next"
};


#endif //CPU_INCLUDED