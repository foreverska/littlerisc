#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "../littlerisc/include/littlerisc.h"
#include "../littlerisc/core.h"

#define RUN_TESTCASE_IMM(core, pc, r1, r1v, rd, rdv) { \
    setRegisterVal(core, r1, r1v); \
    ret = coreExecute(core); \
    assert(ret == CORE_OK); \
    assert(getRegisterVal(core, PC_REG) == pc); \
    assert(getRegisterVal(core, r1) == r1v); \
    assert(getRegisterVal(core, rd) == rdv); \
}

#define RUN_TESTCASE(core, pc, r1, r1v, r2, r2v, rd, rdv) { \
    setRegisterVal(core, r1, r1v); \
    setRegisterVal(core, r2, r2v); \
    ret = coreExecute(core); \
    assert(ret == CORE_OK); \
    assert(getRegisterVal(core, PC_REG) == pc); \
    assert(getRegisterVal(core, r1) == r1v && \
           getRegisterVal(core, r2) == r2v); \
    assert(getRegisterVal(core, rd) == rdv); \
} 

void storeTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[6] = {0x00208023, 0x00209023, 0x1e20aa23, 0xc0208c23};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //Store Byte
    RUN_TESTCASE(pCore, 4, 1, 1000, 2, 0xAB, 0, 0);
    assert(*readMemory(pCore, 1000) == 0xAB);

    //Store Halfword
    RUN_TESTCASE(pCore, 8, 1, 1000, 2, 0xABCD, 0, 0);
    assert(*readMemory(pCore, 1000) == 0xCD);
    assert(*readMemory(pCore, 1001) == 0xAB);

    //Store Word
    RUN_TESTCASE(pCore, 12, 1, 500, 2, 0xABCDEF01, 0, 0);
    assert(*readMemory(pCore, 1000) == 0x01);
    assert(*readMemory(pCore, 1001) == 0xEF);
    assert(*readMemory(pCore, 1002) == 0xCD);
    assert(*readMemory(pCore, 1003) == 0xAB);

    //Negative Immediate
    RUN_TESTCASE(pCore, 16, 1, 2000, 2, 0xAB, 0, 0);
    assert(*readMemory(pCore, 1000) == 0xAB);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void loadTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[7] = {0x00008183, 0xc1809183, 0x0000a183, 0x1f40c183,
                           0x0000d183, 0x00008003, 0x7d008183};

    int32_t signedMemory[1] = {(int32_t) 0xFEFEFEFE};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    loadMemory(pCore, (uint8_t*) signedMemory, 1000, sizeof(signedMemory));

    //Load Signed Byte
    RUN_TESTCASE_IMM(pCore, 4, 1, 1000, 3, 0xFFFFFFFE);

    //Load Signed Half-Word
    RUN_TESTCASE_IMM(pCore, 8, 1, 2000, 3, 0xFFFFFEFE);

    //Load Signed Word
    RUN_TESTCASE_IMM(pCore, 12, 1, 1000, 3, 0xFEFEFEFE);

    //Load Byte Unsigned
    RUN_TESTCASE_IMM(pCore, 16, 1, 500, 3, 0x000000FE);

    //Load Half-Word Unsigned
    RUN_TESTCASE_IMM(pCore, 20, 1, 1000, 3, 0x0000FEFE);

    //x0 Dest
    RUN_TESTCASE_IMM(pCore, 24, 1, 1000, 0, 0x0);

    //Out of Bounds (HIGH)
    setRegisterVal(pCore, 1, 1000);
    ret = coreExecute(pCore);
    assert(ret == CORE_MEMERR);
    assert(getRegisterVal(pCore, 1) == 1000);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void branchInequality()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[12] = {0x0220c663, 0x0020ca63, 0x00000013, 0x0220e063,
                           0x0020ea63, 0x00000013, 0x0020da63, 0xfe20d8e3,
                           0x00000013, 0x0020f463, 0xfe20fee3, 0x00000013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //BLT Greater Than
    RUN_TESTCASE(pCore, 0x04, 1, 5, 2, 2, 0, 0);

    //BLT Less Than
    RUN_TESTCASE(pCore, 0x18, 1, (uint32_t) -2, 2, 5, 0, 0);

    //BGE Less Than
    RUN_TESTCASE(pCore, 0x1C, 1, 2, 2, 5, 0, 0);

    //BGE Greater Than
    RUN_TESTCASE(pCore, 0x0C, 1, 5, 2, (uint32_t) -2, 0, 0);

    //BLTU Greater Than
    RUN_TESTCASE(pCore, 0x10, 1, 5, 2, 2, 0, 0);

    //BLTU Less Than
    RUN_TESTCASE(pCore, 0x24, 1, 2, 2, 5, 0, 0);

    //BGEU Less Than
    RUN_TESTCASE(pCore, 0x28, 1, 2, 2, 5, 0, 0);

    //BGEU Greater Than
    RUN_TESTCASE(pCore, 0x24, 1, 5, 2, 2, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void branchEquality()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[6] = {0x00208a63, 0x00208463, 0x00000013, 0x00209463,
                          0xfe2098e3, 0x00000013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //BEQ Not Equal
    RUN_TESTCASE(pCore, 0x4, 1, 5, 2, 4, 0, 0);

    //BEQ Equal
    RUN_TESTCASE(pCore, 0xC, 1, 5, 2, 5, 0, 0);

    //BNE Equal
    RUN_TESTCASE(pCore, 0x10, 1, 5, 2, 5, 0, 0);

    //BNE Not Equal
    RUN_TESTCASE(pCore, 0x0, 1, 5, 2, 4, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void jalrTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[4] = {0x004100e7, 0x00810067, 0x008100e7, 0x000100e7};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //positive offset
    setRegisterVal(pCore, 2, 0x4);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 2) == 0x4);
    assert(getRegisterVal(pCore, 1) == 0x4);
    assert(getRegisterVal(pCore, PC_REG) == 0x8);

    //negative offset
    setRegisterVal(pCore, 2, (uint32_t) -4);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 2) == (uint32_t) -4);
    assert(getRegisterVal(pCore, 1) == 0xC);
    assert(getRegisterVal(pCore, PC_REG) == 0x4);

    //x0 dest
    setRegisterVal(pCore, 2, 4);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 2) == 4);
    assert(getRegisterVal(pCore, 0) == 0x0);
    assert(getRegisterVal(pCore, PC_REG) == 0xC);

    //0 offset
    setRegisterVal(pCore, 2, 0);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 2) == 0);
    assert(getRegisterVal(pCore, 1) == 0x10);
    assert(getRegisterVal(pCore, PC_REG) == 0x0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void jalTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[4] = {0x008000ef, 0x0080006f, 0xffdff0ef, 0x000000ef};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //positive offset
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 1) == 0x4);
    assert(getRegisterVal(pCore, PC_REG) == 0x8);

    //negative offset
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 1) == 0xC);
    assert(getRegisterVal(pCore, PC_REG) == 0x4);

    //x0 dest
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 0) == 0x0);
    assert(getRegisterVal(pCore, PC_REG) == 0xC);

    //0 offset
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, 1) == 0x10);
    assert(getRegisterVal(pCore, PC_REG) == 0xC);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void auipcTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[3] = {0x11223097, 0x00000097, 0x00000017};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //simple offset
    setRegisterVal(pCore, 1, 5);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 4);
    assert(getRegisterVal(pCore, 1) == 0x11223000);

    //add 0
    setRegisterVal(pCore, 1, 5);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 8);
    assert(getRegisterVal(pCore, 1) == 0x00000004);

    //dest x0
    setRegisterVal(pCore, 1, 5);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 12);
    assert(getRegisterVal(pCore, 0) == 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void luiTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[2] = {0x112230b7, 0x11223037};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //load IMM
    setRegisterVal(pCore, 1, 5);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 4);
    assert(getRegisterVal(pCore, 1) == 0x11223000);

    //load to x0
    setRegisterVal(pCore, 0, 5);
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 8);
    assert(getRegisterVal(pCore, 0) == 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void lessThanImmTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[6] = {0x0070a193, 0xffe0a193, 0x00a0a013, 0xffe0b193,
                          0x0020b193, 0x00a0b013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //SLTI IMM Greater
    RUN_TESTCASE_IMM(pCore, 4, 1, 5, 3, 1);

    //SLTI IMM Less
    RUN_TESTCASE_IMM(pCore, 8, 1, 5, 3, 0);

    //SLTI x0 dest
    RUN_TESTCASE_IMM(pCore, 12, 1, 5, 0, 0);

    //SLTIU IMM Greater
    RUN_TESTCASE_IMM(pCore, 16, 1, 5, 3, 1);

    //SLTIU IMM Less
    RUN_TESTCASE_IMM(pCore, 20, 1, 5, 3, 0);

    //SLTIU x0 Dest
    RUN_TESTCASE_IMM(pCore, 24, 1, 5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void shiftImmTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[7] = {0x00209193, 0x00209013, 0x0020d193, 0x0020d013,
                          0x4020d193, 0x4010d193, 0x4020d013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //SLLI
    RUN_TESTCASE_IMM(pCore, 4, 1, 5, 3, 20);

    //SLLI x0 Dest
    RUN_TESTCASE_IMM(pCore, 8, 1, 5, 0, 0);

    //SRLI
    RUN_TESTCASE_IMM(pCore, 12, 1, 20, 3, 5);

    //SRLI x0 Dest
    RUN_TESTCASE_IMM(pCore, 16, 1, 20, 0, 0);

    //SRAI < 32
    RUN_TESTCASE_IMM(pCore, 20, 1, 20, 3, 5);

    //SRAI Neg
    RUN_TESTCASE_IMM(pCore, 24, 1, (uint32_t) -20, 3, (uint32_t) -10);

    //SRAI x0 Dest
    RUN_TESTCASE_IMM(pCore, 28, 1, 20, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void bitwiseImmTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[9] = {0xffe0c193, 0x0020c193, 0x0020c013, 0xffe0e193,
                          0x0020e193, 0x0020e013, 0xffb0f193, 0x0010f193,
                          0x0010f013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //XOR Neg
    RUN_TESTCASE_IMM(pCore, 4, 1, 5, 3, (uint32_t) -5);

    //XOR Pos
    RUN_TESTCASE_IMM(pCore, 8, 1, 5, 3, 7);

    //XOR x0 dest
    RUN_TESTCASE_IMM(pCore, 12, 1, 5, 0, 0);

    //OR Neg
    RUN_TESTCASE_IMM(pCore, 16, 1, 5, 3, (uint32_t) -1);

    //OR Pos
    RUN_TESTCASE_IMM(pCore, 20, 1, 5, 3, 7);

    //OR x0 dest
    RUN_TESTCASE_IMM(pCore, 24, 1, 5, 0, 0);

    //AND Neg
    RUN_TESTCASE_IMM(pCore, 28, 1, 5, 3, 1);

    //AND Pos
    RUN_TESTCASE_IMM(pCore, 32, 1, 5, 3, 1);

    //AND x0 dest
    RUN_TESTCASE_IMM(pCore, 36, 1, 5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void addImmTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[3] = {0x00208193, 0xffe08193, 0x00208013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //imm + rs1
    RUN_TESTCASE_IMM(pCore, 4, 1, 5, 3, 7);

    //sign extend
    RUN_TESTCASE_IMM(pCore, 8, 1, 5, 3, 3);

    //dest x0
    RUN_TESTCASE_IMM(pCore, 12, 1, 5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void lessThanTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[9] = {0x0020b1b3, 0x0020b1b3, 0x0020b033, 0x0020a1b3,
                          0x0020a1b3, 0x0020a1b3, 0x0020a1b3, 0x0020a1b3,
                          0x0030a033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    // RS1 < RS2
    RUN_TESTCASE(pCore, 4, 1, 2, 2, 5, 3, 1);

    // RS2 < RS1
    RUN_TESTCASE(pCore, 8, 1, 5, 2, 2, 3, 0)

    // SLTU DEST x0
    RUN_TESTCASE(pCore, 12, 1, 2, 2, 5, 0, 0);

    // RS1 < RS2 (RS1 NEG)
    RUN_TESTCASE(pCore, 16, 1, (uint32_t) -2, 2, 5, 3, 1);

    // RS1 < RS2 (RS1 POS)
    RUN_TESTCASE(pCore, 20, 1, 2, 2, 5, 3, 1);

    // RS2 < RS1 (RS2 POS)
    RUN_TESTCASE(pCore, 24, 1, 5, 2, 2, 3, 0);

    // RS2 < RS1 (RS2 NEG)
    RUN_TESTCASE(pCore, 28, 1, 5, 2, (uint32_t) -2, 3, 0);

    // RS2 < RS1 (BOTH NEG)
    RUN_TESTCASE(pCore, 32, 1, (uint32_t) -1, 2, (uint32_t) -2, 3, 0);

    // SLT DEST x0
    RUN_TESTCASE(pCore, 36, 1, 2, 2, 5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void shiftTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[9] = {0x001111b3, 0x001111b3, 0x00111033, 0x001151b3,
                          0x001151b3, 0x00115033, 0x401151b3, 0x401151b3,
                          0x40115033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //SLL x3, x2, x1
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 2, 3, 64);

    //SLL x3, x2, x1 (x2 > 31)
    RUN_TESTCASE(pCore, 8, 1, 33, 2, 5, 3, 10);

    //SLL x0, x2, x1
    RUN_TESTCASE(pCore, 12, 1, 33, 2, 5, 0, 0);

    //SRL x3, x2, x1
    RUN_TESTCASE(pCore, 16, 1, 2, 2, 5, 3, 1);

    //SRL x3, x2, x1 (x2 > 31)
    RUN_TESTCASE(pCore, 20, 1, 33, 2, 5, 3, 2);

    //SRL x0, x2, x1 (x2 > 31)
    RUN_TESTCASE(pCore, 24, 1, 33, 2, 5, 0, 0);

    //SRA x3, x2, x1
    RUN_TESTCASE(pCore, 28, 1, 2, 2, (uint32_t) -255, 3, (uint32_t) -64);

    //SRA x3, x2, x1 (x2 > 31)
    RUN_TESTCASE(pCore, 32, 1, 33, 2, (uint32_t) -255, 3, (uint32_t) -128);

    //SRA x0, x2, x1 (x2 > 31)
    RUN_TESTCASE(pCore, 36, 1, 323, 2, (uint32_t) -255, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void andTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[2] = {0x001171b3, 0x0020f033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //AND x3, x2, x1
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 5, 3, 5);

    //AND x0, x2, x1
    RUN_TESTCASE(pCore, 8, 1, 5, 2, 5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void orTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[2] = {0x001161b3, 0x0020e033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //OR x3, x2, x1
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 10, 3, 15);

    //OR x0, x2, x1
    RUN_TESTCASE(pCore, 8, 1, 5, 2, 10, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void xorTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[2] = {0x001141b3, 0x0020c033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //XOR x3, x2, x1
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 10, 3, 15);

    //XOR x0, x2, x1
    RUN_TESTCASE(pCore, 8, 1, 5, 2, 10, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void subTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[4] = {0x402081b3, 0x40118133, 0x403100b3, 0x40310033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //SUB x3, x1, x2
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 10, 3, (uint32_t) -5);

    //SUB x2, x3, x1
    RUN_TESTCASE(pCore, 8, 1, 5, 3, (uint32_t) -5, 2, (uint32_t) -10);

    //SUB x1, x2, x3
    RUN_TESTCASE(pCore, 12, 2, (uint32_t)-10, 3, (uint32_t)-5, 1, (uint32_t)-5);

    //SUB x0, x2, x3
    RUN_TESTCASE(pCore, 16, 2, (uint32_t) -10, 3, (uint32_t) -5, 0, 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void addTest()
{
    int ret;
    riscvCore *pCore;
    uint32_t memory[6] = {0x002081B3, 0x002081B3, 0x002081B3, 0x00208033,
                          0x00100133, 0x00000033};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    //Add x1 to x2 store x3
    RUN_TESTCASE(pCore, 4, 1, 5, 2, 10, 3, 15);

    //Add x1 to x2 store x3 (x1 neg)
    RUN_TESTCASE(pCore, 8, 1, (uint32_t) -5, 2, 10, 3, 5);

    //Add x1 to x2 store x3 (x1/x2 neg)
    RUN_TESTCASE(pCore, 12, 1, (uint32_t)-5, 2, (uint32_t)-10, 3, (uint32_t)-15);

    //Add x1 to x2 store x0
    RUN_TESTCASE(pCore, 16, 1, 5, 2, 10, 0, 0);

    //Mov by add (x0) from x1 to x2
    RUN_TESTCASE(pCore, 20, 1, 5, 2, 5, 0, 0);

    //NOP by Mov
    ret = coreExecute(pCore);
    assert(ret == CORE_OK);
    assert(getRegisterVal(pCore, PC_REG) == 24);
    assert(getRegisterVal(pCore, 0) == 0);

    destroyMemory(pCore);
    destroyCore(pCore);
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    addTest();
    subTest();
    xorTest();
    orTest();
    andTest();
    shiftTest();
    lessThanTest();
    addImmTest();
    bitwiseImmTest();
    shiftImmTest();
    lessThanImmTest();
    luiTest();
    auipcTest();
    jalTest();
    jalrTest();
    branchEquality();
    branchInequality();
    loadTest();
    storeTest();
}
