#ifndef CSR_H
#define CSR_H

#include "core.h"

#define MSTATUS     0x300
#define MIE         0x304
#define MCOUNTEREN  0x306
#define MIP         0x344
#define MCYCLE      0xB00
#define MCYCLEH     0xB80
#define MVENDORID   0xF11
#define MARCHID     0xF12
#define MIMPID      0xF13
#define MHARTID     0xF14

#define UNIMPL      0x00000000
#define CY_EN       0x00000001
#define MACH_IE     0x00000888
#define MACH_TIP    0x00000080

#define CSR_OK      0
#define CSR_NOK     -1

int setupCsr(riscvCore *pCore);

int readCsr(riscvCore *pCore, uint16_t addr, uint32_t *pValue);
int writeCsr(riscvCore *pCore, uint16_t addr, uint32_t value);

#endif
