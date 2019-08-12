#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#include "mmu.h"

#define PC_REG 2048

#define CORE_OK         0
#define CORE_NOK        -1
#define CORE_UNALIGNED  -2
#define CORE_UNKOPCODE  -3
#define CORE_UNKINST    -4
#define CORE_MEMERR     -5

typedef struct
{
    uint32_t regs[32];
    uint32_t pc;
    lrMmu mmu;
    uint32_t csr[4096];
}riscvCore;

void setRegisterVal(riscvCore *pCore, uint16_t regNumber, uint32_t val);
uint32_t getRegisterVal(riscvCore *pCore, uint16_t regNumber);

void initMemory(riscvCore *pCore, uint32_t size);
void destroyMemory(riscvCore *pCore);
void loadMemory(riscvCore *pCore, uint8_t *pMem, uint32_t loc, uint32_t size);
uint8_t *readMemory(riscvCore *pCore, uint32_t loc);

int coreExecute(riscvCore *pCore);

void initCore(riscvCore **pCore);
void destroyCore(riscvCore *pCore);

#endif
