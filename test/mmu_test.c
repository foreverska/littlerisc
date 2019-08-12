#include <stdlib.h>
#include <assert.h>

#include "../littlerisc/core.h"
#include "../littlerisc/include/littlerisc.h"

void LoadMemoryTest()
{
    riscvCore *pCore;
    uint8_t mem[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    initCore(&pCore);
    initMemory(pCore, 2000);

    loadMemory(pCore, mem, 0, 10);
    for (uint8_t i = 0; i < 10; i++)
    {
        assert(*readMemory(pCore, i) == (i+1));
    }

    destroyMemory(pCore);
    destroyCore(pCore);
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    LoadMemoryTest();
}
