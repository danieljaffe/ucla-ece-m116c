#include "CPU.hpp"

CPU::CPU(std::vector<uint8_t> &&iMem, std::vector<uint8_t> &&dMem) : insMem(std::move(iMem)), dataMem(std::move(dMem)) {
    std::fill(registerFile, registerFile + sizeof(registerFile), 0);
    pc = 0;
    killCounter = 0;
}


void CPU::fetch() {
    // Fetch one instruction from the program memory in little endian form.
    uint32_t byte1 = insMem[ifidCurr.pc];
    uint32_t byte2 = insMem[ifidCurr.pc + 1];
    uint32_t byte3 = insMem[ifidCurr.pc + 2];
    uint32_t byte4 = insMem[ifidCurr.pc + 3];

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
    idexNext.readData1 = registerFile[rs1];
    idexNext.readData2 = registerFile[rs2];
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
    // Update the EXMEM struct
    exmemNext.pc = idexCurr.pc;
    exmemNext.operation = idexCurr.operation;
    exmemNext.readData2 = idexCurr.readData2;
    exmemNext.rd = idexCurr.rd;
    switch(idexCurr.operation) {
        case Op::ADD:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.readData2;
            break;

        case Op::SUB:
            exmemNext.aluResult = idexCurr.readData1 - idexCurr.readData2;
            break;

        case Op::OR:
            exmemNext.aluResult = idexCurr.readData1 | idexCurr.readData2;
            break;

        case Op::AND:
            exmemNext.aluResult = idexCurr.readData1 & idexCurr.readData2;
            break;

        case Op::ADDI:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.immediate;
            break;

        case Op::ORI:
            exmemNext.aluResult = idexCurr.readData1 | idexCurr.immediate;
            break;

        case Op::ANDI:
            exmemNext.aluResult = idexCurr.readData1 & idexCurr.immediate;
            break;

        case Op::LW:  // Does the same as SW for the ALU result
        case Op::SW:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.immediate;
            break;

        case Op::ZE:
            exmemNext.aluResult = ZERO;
            break;

        case Op::ERROR:
            break;
    }
}

void CPU::memory() {
    // Update the MEMWB struct
    memwbNext.rd = exmemCurr.rd;
    memwbNext.aluResult = exmemCurr.aluResult;
    ifidNext.pc = exmemCurr.pc + 4;  // No branching or jumps in this version

    int32_t lByte1, lByte2, lByte3, lByte4;
    uint8_t sByte1, sByte2, sByte3, sByte4;
    switch(exmemCurr.operation) {
        case Op::LW:
            // Fetch 4 bytes from the data memory in little endian form.
            lByte1 = dataMem[exmemCurr.aluResult];
            lByte2 = dataMem[exmemCurr.aluResult + 1];
            lByte3 = dataMem[exmemCurr.aluResult + 2];
            lByte4 = dataMem[exmemCurr.aluResult + 3];

            // Convert to big endian and store as the aluResult
            memwbNext.memData = (lByte4 << 24) + (lByte3 << 16) + (lByte2 << 8) + lByte1;
            break;

        case Op::SW:
            // Separate bytes of readData2
            sByte1 = ((uint32_t) exmemCurr.readData2 >> 24) & 0xff000000;
            sByte2 = ((uint32_t) exmemCurr.readData2 >> 16) & 0xff0000;
            sByte3 = ((uint32_t) exmemCurr.readData2 >> 8) & 0xff00;
            sByte4 = (uint32_t) exmemCurr.readData2 & 0xff;

            // Store in dataMem in little endian form
            dataMem[exmemCurr.aluResult] = sByte4;
            dataMem[exmemCurr.aluResult + 1] = sByte3;
            dataMem[exmemCurr.aluResult + 1] = sByte2;
            dataMem[exmemCurr.aluResult + 1] = sByte1;
            break;

        default:
            break;
    }


}

void CPU::writeback() {
    if (ZERO == memwbCurr.rd || op::LW == memwbCurr.operation) {
        return;
    }  // Don't overwrite x0 ever! Also, LW doesn't write to a register.
    if (op::SW == memwbCurr.operation) { registerFile[memwbCurr.rd] = memwbCurr.memData; }
    else { registerFile[memwbCurr.rd] = memwbCurr.aluResult; }
}

void CPU::clockTick() {
    ++clockCount;
    ifidCurr = ifidNext;
    idexCurr = idexNext;
    exmemCurr = exmemNext;
    memwbCurr = memwbNext;
}

bool CPU::isFinished() const {
    // All five stages have ZERO op codes
    return 5 == killCounter;
}

void CPU::printStats() {
    std::cout << "(" << registerFile[10] << ", " << registerFile[11] << ")" << std::endl;
}
