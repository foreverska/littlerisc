#include <stdint.h>

#include "csr.h"

#define ROPERM      0xC00
#define RWPERM(x)   (x & ROPERM)

inline int setupCsr(riscvCore *pCore)
{
    pCore->csr[MARCHID] = UNIMPL;
    pCore->csr[MIMPID] = UNIMPL;
    pCore->csr[MHARTID] = UNIMPL;
    pCore->csr[MARCHID] = UNIMPL;

    pCore->csr[MCOUNTEREN] = CY_EN;
    pCore->csr[MIE] = MACH_IE;

    return CSR_OK;
}

inline int readCsr(riscvCore *pCore, uint16_t addr, uint32_t *pValue)
{
    *pValue = pCore->csr[addr];

    return CSR_OK;
}

inline int writeCsr(riscvCore *pCore, uint16_t addr, uint32_t value)
{
    if (RWPERM(addr) == ROPERM)
    {
        return CSR_NOK;
    }

    pCore->csr[addr] = value;

    return CSR_OK;
}
