#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../littlerisc/core.h"
#include "../littlerisc/csr.h"

#define CHECK_CSR(core, csr, csrVal) { \
    ret = readCsr(core, csr, &value); \
    assert(ret == CSR_OK); \
    assert(value == csrVal); \
}

void MachIntTest()
{
    int ret;
    uint32_t value;
    riscvCore *pCore;
    uint32_t memory[6] = {0x00000013, 0x00000013, 0x00000013, 0x00000013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    CHECK_CSR(pCore, MIE, MACH_IE);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void MachCounterTest()
{
    int ret;
    uint32_t value;
    riscvCore *pCore;
    uint32_t memory[6] = {0x00000013, 0x00000013, 0x00000013, 0x00000013};

    initCore(&pCore);
    initMemory(pCore, 2000);
    loadMemory(pCore, (uint8_t*) memory, 0, sizeof(memory));

    CHECK_CSR(pCore, MCOUNTEREN, CY_EN);

    CHECK_CSR(pCore, MCYCLE, 0);
    CHECK_CSR(pCore, MCYCLEH, 0);

    ret = coreExecute(pCore);
    assert(ret == CORE_OK);

    CHECK_CSR(pCore, MCYCLE, 1);
    CHECK_CSR(pCore, MCYCLEH, 0);

    ret = coreExecute(pCore);
    assert(ret == CORE_OK);

    CHECK_CSR(pCore, MCYCLE, 2);
    CHECK_CSR(pCore, MCYCLEH, 0);

    pCore->csr[MCYCLE] = 0xFFFFFFFF;

    ret = coreExecute(pCore);
    assert(ret == CORE_OK);

    CHECK_CSR(pCore, MCYCLE, 0);
    CHECK_CSR(pCore, MCYCLEH, 1);

    ret = coreExecute(pCore);
    assert(ret == CORE_OK);

    CHECK_CSR(pCore, MCYCLE, 1);
    CHECK_CSR(pCore, MCYCLEH, 1);

    destroyMemory(pCore);
    destroyCore(pCore);
}

void MachTrapSetupTest()
{
    int ret;
    uint32_t value;
    riscvCore *pCore;

    initCore(&pCore);

    //Machine Counter Enable (mcounteren)
    CHECK_CSR(pCore, MCOUNTEREN, CY_EN);

    destroyCore(pCore);
}

void MachInfoTest()
{
    int ret;
    uint32_t value;
    riscvCore *pCore;

    initCore(&pCore);

    //VENDOR ID (mvendorid)
    ret = readCsr(pCore, MVENDORID, &value);
    assert(ret == CSR_OK);
    assert(value == 0);

    //Architecture ID (marchid)
    ret = readCsr(pCore, MARCHID, &value);
    assert(ret == CSR_OK);
    assert(value == 0);

    //Implementation ID (mimpid)
    ret = readCsr(pCore, MIMPID, &value);
    assert(ret == CSR_OK);
    assert(value == 0);

    //Hardware Thread ID (mhartid)
    ret = readCsr(pCore, MHARTID, &value);
    assert(ret == CSR_OK);
    assert(value == 0);

    destroyCore(pCore);
}

void AccessTest()
{
    int ret;
    uint32_t value;
    uint32_t temp;
    riscvCore *pCore;

    initCore(&pCore);

    //Read RW Register
    ret = readCsr(pCore, MSTATUS, &value);
    assert(ret == CSR_OK);

    //Write RW Register
    temp = 32;
    ret = writeCsr(pCore, MSTATUS, temp);
    assert(ret == CSR_OK);
    ret = readCsr(pCore, MSTATUS, &value);
    assert(ret == CSR_OK);
    assert(value == temp);

    //Read RO Register
    ret = readCsr(pCore, MVENDORID, &value);
    assert(ret == CSR_OK);

    //Write RO Register
    ret = writeCsr(pCore, MVENDORID, value);
    assert(ret == CSR_NOK);

    destroyCore(pCore);
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    AccessTest();
    MachInfoTest();
    MachTrapSetupTest();
    MachCounterTest();
    MachIntTest();
}
