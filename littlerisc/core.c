#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/littlerisc.h"

#include "core.h"
#include "csr.h"

#define OPCODE(x)   (x & 0x7F)
#define LOW_IMM(x)  RD(x)
#define RD(x)       ((x >> 7) & 0x1F)
#define IMM20(x)    ((x >> 12) & 0xFFFFF)
#define SIMM20(x)   (x & 0xFFFFF000)
#define JIMM20(x)   ((x & 0x80000000) | ((x & 0xFF000) << 11) | \
                     ((x & 0x100000)<<2) | ((x & 0x7FE00000) >> 8))
#define BRIMM12(x)  ((x & 0x80000000) | ((x & 0x80)<<23) | \
                    ((x & 0x7E000000)>>1) | ((x & 0xF00) << 12))
#define FUNCT3(x)   ((x >> 12) & 0x7)
#define RS1(x)      ((x >> 15) & 0x1F)
#define RS2(x)      ((x >> 20) & 0x1F)
#define FUNCT7(x)   IMM7(x)
#define SIMM7(x)    (x & 0xFE000000)
#define IMM7(x)     ((x >> 25) & 0x7F)
#define IMM12(x)    ((x >> 20) & 0xFFF)
#define SIMM12(x)   (x & 0xFFF00000)

#define WORDALIGN(x) (__builtin_assume_aligned(x,4))

#define LOWER5      0x1F

#define LOAD        0x03
#define IMATH       0x13
#define AUIPC       0x17
#define STORE       0x23
#define RMATH       0x33
#define LUI         0x37
#define BRANCH      0x63
#define JALR        0x67
#define JAL         0x6F

#define FUNCBEQ     0x00
#define FUNCBNE     0x01
#define FUNCBLT     0x04
#define FUNCBGE     0x05
#define FUNCBLTU    0x06
#define FUNCBGEU    0x07

#define FUNCADDI    0x00
#define FUNCSLLI    0x01
#define FUNCSLTI    0x02
#define FUNCSLTIU   0x03
#define FUNCXORI    0x04
#define FUNCSRXI    0x05
#define FUNCORI     0x06
#define FUNCANDI    0x07

#define FUNCADDSUB  0x00
#define FUNCSLL     0x01
#define FUNCSLT     0x02
#define FUNCSLTU    0x03
#define FUNCXOR     0x04
#define FUNCSR      0x05
#define FUNCOR      0x06
#define FUNCAND     0x07

#define FUNCLB      0x00
#define FUNCLH      0x01
#define FUNCLW      0x02
#define FUNCLBU     0x04
#define FUNCLHU     0x05

#define FUNCSB      0x00
#define FUNCSH      0x01
#define FUNCSW      0x02

#define EVENFUNC    0x00
#define ODDFUNC     0x20

#define REG(x)      (pCore->regs[x])
#define SREG(x)     ((int32_t) pCore->regs[x])

#define EXTEND      0xFFFFF000
#define BIT12       0x00000800

void signExtend12Bit(uint32_t *pVal)
{
    if ((*pVal & BIT12) != 0)
    {
        *pVal |= EXTEND;
    }
}

int coreExecute(riscvCore *pCore)
{
    uint32_t instruction, address, val;
    uint8_t rs1, rs2, rd;
    int32_t imm, temp;
    uint8_t *pVal;

    if (pCore->mmu.memSize == 0)
    {
        return CORE_MEMERR;
    }

    if (pCore->pc%4 != 0)
    {
        return CORE_UNALIGNED;
    }

    instruction = *((uint32_t*) WORDALIGN(&pCore->mmu.pMemory[pCore->pc]));

    switch (OPCODE(instruction))
    {
    case STORE:
        imm = ((int32_t) SIMM7(instruction) |
               ((int32_t) LOW_IMM(instruction)<<20))>>20;
        rs1 = RS1(instruction);
        rs2 = RS2(instruction);
        imm += REG(rs1);
        address = (uint32_t) imm;
        temp = 0;

        if (address > pCore->mmu.memSize)
        {
            return CORE_MEMERR;
        }

        val = REG(rs2);
        pVal = (uint8_t*) &val;

        switch (FUNCT3(instruction))
        {
        case FUNCSB:
            pCore->mmu.pMemory[address] = val&0xFF;
            break;
        case FUNCSH:
            pCore->mmu.pMemory[address] = val&0xFF;
            pCore->mmu.pMemory[address+1] = (val&0xFF00)>>8;
            break;
        case FUNCSW:
            pCore->mmu.pMemory[address] = pVal[0];
            pCore->mmu.pMemory[address+1] = pVal[1];
            pCore->mmu.pMemory[address+2] = pVal[2];
            pCore->mmu.pMemory[address+3] = pVal[3];
            break;
        default: return CORE_UNKINST;
        }
        break;
    case LOAD:
        imm = ((int32_t) SIMM12(instruction))>>20;
        rd = RD(instruction);
        rs1 = RS1(instruction);
        imm += REG(rs1);
        address = (uint32_t) imm;
        temp = 0;

        if (address > pCore->mmu.memSize)
        {
            return CORE_MEMERR;
        }

        switch (FUNCT3(instruction))
        {
        case FUNCLB:
            temp = pCore->mmu.pMemory[address]<<24;
            val = (uint32_t) (temp>>24);
            break;
        case FUNCLH:
            temp = (pCore->mmu.pMemory[address] |
                    pCore->mmu.pMemory[address+1]<<8)<<16;
            val = (uint32_t) (temp>>16);
            break;
        case FUNCLW:
            val = (uint32_t) (pCore->mmu.pMemory[address] |
                                  pCore->mmu.pMemory[address+1]<<8 |
                                  pCore->mmu.pMemory[address+2]<<16 |
                                  pCore->mmu.pMemory[address+3]<<24 );
            break;
        case FUNCLBU:
            val = pCore->mmu.pMemory[address];
            break;
        case FUNCLHU:
            val = (uint32_t) (pCore->mmu.pMemory[address] |
                                  pCore->mmu.pMemory[address+1]<<8);
            break;
        default: return CORE_UNKINST;
        }

        if (rd != 0)
        {
            REG(rd) = val;
        }

        break;
    case BRANCH:
        switch (FUNCT3(instruction))
        {
        case FUNCBEQ:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (REG(rs1) == REG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        case FUNCBNE:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (REG(rs1) != REG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        case FUNCBLT:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (SREG(rs1) < SREG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        case FUNCBGE:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (SREG(rs1) >= SREG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        case FUNCBLTU:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (REG(rs1) < REG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        case FUNCBGEU:
            rs1 = RS1(instruction);
            rs2 = RS2(instruction);
            imm = (int32_t) BRIMM12(instruction);
            imm >>= 20;

            if (REG(rs1) >= REG(rs2))
            {
                //TODO: FIX THIS MINUS 4 HACKYNESS
                pCore->pc += (uint32_t) (imm*2) - 4;
            }
            break;
        default:
            return CORE_UNKINST;
        }
        break;
    case JAL:
        rd = RD(instruction);
        imm = (int32_t) JIMM20(instruction);
        imm >>= 12;
        if (rd != 0)
        {
            REG(rd) = pCore->pc + 4;
        }
        //TODO: TAKE ANOTHER LOOK AT THIS
        pCore->pc += (uint32_t) (imm - 4);
        break;
    case JALR:
        rd = RD(instruction);
        rs1 = RS1(instruction);
        imm = (int32_t) SIMM12(instruction);
        imm >>= 20;
        if (rd != 0)
        {
            REG(rd) = pCore->pc + 4;
        }
        //TODO: TAKE ANOTHER LOOK AT -4 BS
        pCore->pc = (uint32_t) ((imm + SREG(rs1) & (int32_t) 0xFFFFFFFE) - 4);
        break;
    case LUI:
        imm = (int32_t) SIMM20(instruction);
        rd = RD(instruction);
        if (rd != 0)
        {
            REG(rd) = (uint32_t) imm | (REG(rd) ^ REG(rd));
        }
        break;
    case AUIPC:
        imm = (int32_t) SIMM20(instruction);
        rd = RD(instruction);
        if (rd != 0)
        {
            REG(rd) = (uint32_t) imm + pCore->pc;
        }
        break;
    case IMATH:
        rs1 = RS1(instruction);
        rd = RD(instruction);
        imm = ((int32_t) SIMM12(instruction))>>20;

        if (rd != 0)
        {
            switch (FUNCT3(instruction))
            {
            case FUNCADDI: REG(rd) = (uint32_t) imm + REG(rs1); break;
            case FUNCXORI: REG(rd) = (uint32_t) imm ^ REG(rs1); break;
            case FUNCORI: REG(rd) = (uint32_t) imm | REG(rs1); break;
            case FUNCANDI: REG(rd) = (uint32_t) imm & REG(rs1); break;
            case FUNCSLLI: REG(rd) = REG(rs1) << (imm & LOWER5); break;
            case FUNCSRXI:
                if (imm <= LOWER5) {REG(rd) = REG(rs1) >> (imm & LOWER5);}
                else {REG(rd) = (uint32_t) (SREG(rs1) >> (imm & LOWER5));}
                break;
            case FUNCSLTI: REG(rd) = SREG(rs1) < imm; break;
            case FUNCSLTIU: REG(rd) = REG(rs1) < (uint32_t) imm; break;
            default: return CORE_UNKINST;
            }
        }
        break;
    case RMATH:
        rs1 = RS1(instruction);
        rs2 = RS2(instruction);
        rd = RD(instruction);
        if (rd != 0)
        {
            switch (FUNCT3(instruction))
            {
            case FUNCADDSUB:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) + REG(rs2); break;
                case ODDFUNC: REG(rd) = REG(rs1) - REG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCXOR:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) ^ REG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCOR:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) | REG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCAND:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) & REG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCSLL:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) << (REG(rs2) & LOWER5); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCSR:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) >> (REG(rs2) & LOWER5); break;
                case ODDFUNC: REG(rd) = (uint32_t)(SREG(rs1) >> (REG(rs2) & LOWER5));
                    break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCSLT:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = SREG(rs1) < SREG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            case FUNCSLTU:
                switch (IMM7(instruction))
                {
                case EVENFUNC: REG(rd) = REG(rs1) < REG(rs2); break;
                default: return CORE_UNKINST;
                }
                break;
            }
        }
        break;
    default:
        return CORE_UNKOPCODE;
    }

    pCore->pc += 4;

    readCsr(pCore, MCOUNTEREN, &val);
    if ((val & CY_EN) == CY_EN)
    {
        uint32_t temp;
        readCsr(pCore, MCYCLE, &val);
        temp = val;
        val++;
        writeCsr(pCore, MCYCLE, val);
        if (val < temp)
        {
            readCsr(pCore, MCYCLEH, &val);
            val++;
            writeCsr(pCore, MCYCLEH, val);
        }
    }

    return CORE_OK;
}

void initMemory(riscvCore *pCore, uint32_t size)
{
    pCore->mmu.memSize = size;
    pCore->mmu.pMemory = aligned_alloc(4, size);
    memset(pCore->mmu.pMemory, 4, size/4);
}

void destroyMemory(riscvCore *pCore)
{
    if (pCore->mmu.pMemory != NULL)
    {
        free(pCore->mmu.pMemory);
    }
}

void loadMemory(riscvCore *pCore, uint8_t *pMem, uint32_t loc, uint32_t size)
{
    if (loc >= pCore->mmu.memSize)
    {
        printf("don't load memory to locations that dont exit. stupid\n");
        return;
    }

    memcpy(&pCore->mmu.pMemory[loc], pMem, size);
}

uint8_t *readMemory(riscvCore *pCore, uint32_t loc)
{
    return &pCore->mmu.pMemory[loc];
}

uint32_t getRegisterVal(riscvCore *pCore, uint16_t regNumber)
{
    if (regNumber == PC_REG)
    {
        return pCore->pc;
    }

    return pCore->regs[regNumber];
}

void setRegisterVal(riscvCore *pCore, uint16_t regNumber, uint32_t val)
{
    if (regNumber == PC_REG)
    {
        pCore->pc = val;
    }
    else if (regNumber != 0)
    {
        pCore->regs[regNumber] = val;
    }
}

void initCore(riscvCore **pCore)
{
    *pCore = calloc(sizeof(riscvCore), 1);

    setupCsr(*pCore);
}

void destroyCore(riscvCore *pCore)
{
    free(pCore);
}
