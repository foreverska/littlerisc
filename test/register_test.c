#include <stdlib.h>
#include <assert.h>

#include "../littlerisc/include/littlerisc.h"
#include "../littlerisc/core.h"

void setRegisterTest()
{
    riscvCore *pCore = NULL;

    initCore(&pCore);
    assert(pCore != NULL);

    assert(getRegisterVal(pCore, 0) == 0);
    setRegisterVal(pCore, 1, 32);
    assert(getRegisterVal(pCore, 1) == 32);

    assert(getRegisterVal(pCore, 0) == 0);
    setRegisterVal(pCore, 0, 56);
    assert(getRegisterVal(pCore, 0) == 0);

    assert(getRegisterVal(pCore, PC_REG) == 0);
    setRegisterVal(pCore, PC_REG, 32);
    assert(getRegisterVal(pCore, PC_REG) == 32);

    destroyCore(pCore);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    setRegisterTest();
}
