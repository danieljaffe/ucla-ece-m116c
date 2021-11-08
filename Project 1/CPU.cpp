#include "CPU.hpp"

CPU::CPU(std::vector<uint8_t> &&iMem, std::vector<uint8_t> &&dMem) : insMem(std::move(iMem)), dataMem(std::move(dMem)) {
    std::fill(registerFile, registerFile + sizeof(registerFile), 0);
    pc = 0;
    killCounter = 0;
}


void CPU::fetch() {
    // Fetch one instruction from the program memory in little endian form.
    uint32_t byte1 = insMem[pc];
    uint32_t byte2 = insMem[pc + 1];
    uint32_t byte3 = insMem[pc + 2];
    uint32_t byte4 = insMem[pc + 3];

    // Concatenate them into a single 32 bit instruction in big endian form.
    ifidNext.instruction = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;

    // Ensures we can track 5 sequential NOPs
    if (ZERO == (ifidNext.instruction & 0x7fff)) { ++killCounter; }
    else { killCounter = 0; }
}

void CPU::decode() {

    // Extract and examine the instruction components
    uint32_t opcode = ifidCurr.instruction & 0x7fff;
    uint8_t func3 = (ifidCurr.instruction >> 12) & 0x7;
    uint8_t func7 = (ifidCurr.instruction >> 25) & 0x7f;
    uint8_t rs1 = (ifidCurr.instruction >> 15) & 0x1f;
    uint8_t rs2 = (ifidCurr.instruction >> 20) & 0x1f;

    // Update the IDEX struct
    idexNext.pc = ifidCurr.pc;
    idexNext.rs1Data = registerFile[rs1];
    idexNext.rs2Data = registerFile[rs2];
    idexNext.rd = (ifidCurr.instruction >> 7) & 0x1f;
    idexNext.immediate = (int32_t) ifidCurr.instruction >> 20;  // Performs sign extension

    // RTYPE operation
    if (RTYPE == opcode) {
        if (0x0 == func3) {
            if (0x0 == func7) { idexNext.operation = Op::ADD; }
            else if (0x20 == func7) { idexNext.operation = Op::SUB; }
            else { idexNext.operation = Op::ERROR; }  // Unrecognized opcode
        }
        else if (0x6 == func3) { idexNext.operation = Op::OR; }
        else if (0x7 == func3) { idexNext.operation = Op::AND; }
        else { idexNext.operation = Op::ERROR; }  // Unrecognized opcode
    }
    // ITYPE operation
    else if (ITYPE == opcode) {
        if (0x0 == func3) { idexNext.operation = Op::ADDI; }
        else if (0x6 == func3) { idexNext.operation = Op::ORI; }
        else if (0x7 == func3) { idexNext.operation = Op::ANDI; }
        else { idexNext.operation = Op::ERROR; }  // Unrecognized opcode
    }
    // LW operation
    else if (LOADWORD == opcode && 0x2 == func3) { idexNext.operation = Op::LW; }
    // SW operation
    else if (STOREWORD == opcode && 0x2 == func3) {
        auto imm11_5 = (int32_t) (ifidCurr.instruction & 0xfe000000);
        auto imm4_0 = (int32_t) ((ifidCurr.instruction & 0xf80) << 13);
        idexNext.immediate = (imm11_5 + imm4_0) >> 20; // Redefined for SW
        idexNext.operation = Op::SW;
    }
    // ZERO op code
    else if (ZERO == opcode) { idexNext.operation = Op::ZE; }
    else { idexNext.operation = Op::ERROR; }  // Unrecognized opcode

}

void CPU::execute() {
    exmemNext.pc = idexCurr.pc;
    exmemNext.operation = idexCurr.operation;
    exmemNext.rd = idexCurr.rd;
    switch(idexCurr.operation) {
        case Op::ADD:
            exmemNext.aluResult = idexCurr.rs1Data + idexCurr.rs2Data;
            break;

        case Op::SUB:
            exmemNext.aluResult = idexCurr.rs1Data - idexCurr.rs2Data;
            break;

        case Op::OR:
            exmemNext.aluResult = idexCurr.rs1Data | idexCurr.rs2Data;
            break;

        case Op::AND:
            exmemNext.aluResult = idexCurr.rs1Data & idexCurr.rs2Data;
            break;

        case Op::ADDI:
            exmemNext.aluResult = idexCurr.rs1Data + idexCurr.immediate;
            break;

        case Op::ORI:
            exmemNext.aluResult = idexCurr.rs1Data | idexCurr.immediate;
            break;

        case Op::ANDI:
            exmemNext.aluResult = idexCurr.rs1Data & idexCurr.immediate;
            break;

        case Op::LW:  // Does the same as SW for the ALU result
        case Op::SW:
            exmemNext.aluResult = idexCurr.rs1Data + idexCurr.immediate;
            break;

        case Op::ZE:
            exmemNext.aluResult = ZERO;
            break;

        case Op::ERROR:
            break;
    }
}

void CPU::memory() {
    switch(exmemCurr.operation) {
        case Op::LW:
            // Fetch 4 bytes from the data memory in little endian form.
            int32_t byte1 = dataMem[idexCurr.rs1Data + idexCurr.immediate];
            int32_t byte2 = dataMem[idexCurr.rs1Data + idexCurr.immediate + 1];
            int32_t byte3 = dataMem[idexCurr.rs1Data + idexCurr.immediate + 1];
            int32_t byte4 = dataMem[idexCurr.rs1Data + idexCurr.immediate + 1];

            // Convert to big endian and store as the aluResult
            exmemNext.aluResult = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
            break;

        case Op::SW:
//            TODO: store the word
            break;

        default:
//            TODO:

    }


}

void CPU::writeback() {

}

void CPU::clockTick() {
    ++clockCount;
}

bool CPU::isFinished() {
    return 5 == killCounter;
}

void CPU::printStats() {

}
