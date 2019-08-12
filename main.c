#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "littlerisc/include/littlerisc.h"

#define LOAD_OK     0
#define LOAD_NOK    -1

#define PERF_MON_EN 0

size_t getFileSize(FILE *pFile)
{
    size_t size;

    fseek(pFile, 0, SEEK_END);
    size = (size_t) ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    return size;
}

int loadMemoryFromFile(riscvCore *pCore)
{
    FILE *pFile;
    uint8_t *pBuffer;
    size_t fileSize;

    pFile = fopen("/home/adam/riscv_emu/helloworld/hello_world.bin", "r");
    if (pFile == NULL)
    {
        printf("Error opening file\n");
        return LOAD_NOK;
    }
    fileSize = getFileSize(pFile);
    if (fileSize == 0)
    {
        return LOAD_NOK;
    }
    pBuffer = malloc(fileSize);
    if (pBuffer == NULL)
    {
        printf("Error making temporary buffer\n");
        return LOAD_NOK;
    }

    if(fread(pBuffer, 1, fileSize, pFile) != fileSize)
    {
        printf("Something went wrong with fread\n");
        return LOAD_NOK;
    }

    loadMemory(pCore, pBuffer, 0, (uint32_t) fileSize);

    free(pBuffer);
    fclose(pFile);

    return LOAD_OK;
}

void performanceInfo(riscvCore *pCore)
{
    time_t seconds = 0;
    time_t tempTime = 0;
    uint64_t instructions = 0;
    uint64_t temp;

    tempTime = time(NULL);
    if ((tempTime - seconds) > 5)
    {
        temp = (uint64_t) pCore->csr[MCYCLEH]<<32 | pCore->csr[MCYCLE];
        printf("CPS: %ld\n", (temp-instructions)/5);
        instructions = temp;
        seconds = tempTime;
    }
}

int main()
{
    riscvCore *pCore;


    int ret;

    initCore(&pCore);
    initMemory(pCore, 32000);

    printf("Core initialized\n");

    ret = loadMemoryFromFile(pCore);
    if (ret != LOAD_OK)
    {
        return -1;
    }

    printf("Memory Loaded\n");

    pCore->regs[2] = 0x600;

    while (ret == CORE_OK)
    {
        ret = coreExecute(pCore);

        if (PERF_MON_EN == 1)
        {
            performanceInfo(pCore);
        }
    }

    printf("Core exited with ret: %d\n", ret);

    destroyMemory(pCore);
    destroyCore(pCore);
    return 0;
}
