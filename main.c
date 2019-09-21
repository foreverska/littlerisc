#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "littlerisc/include/littlerisc.h"

#define SERIAL_MMIO     0x1000

#define LOAD_OK         0
#define LOAD_NOK        -1

#define PERF_MON_EN     1
#define PERF_MON_DIS    0

typedef struct {
    uint64_t perfMon;
    uint32_t serialAddr;
    char *pFile;
} progOptions;

size_t getFileSize(FILE *pFile)
{
    size_t size;

    fseek(pFile, 0, SEEK_END);
    size = (size_t) ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    return size;
}

int loadMemoryFromFile(riscvCore *pCore, progOptions *pOptions)
{
    FILE *pFile;
    uint8_t *pBuffer;
    size_t fileSize;

    pFile = fopen(pOptions->pFile, "r");
    if (pFile == NULL)
    {
        printf("Error opening file\n");
        return LOAD_NOK;
    }
    fileSize = getFileSize(pFile);
    pBuffer = malloc(fileSize);
    assert(pBuffer != NULL);

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
    static time_t seconds = 0;
    time_t tempTime = 0;
    static uint64_t instructions = 0;
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

void parseOptions(int argc, char **argv, progOptions *pProgOptions)
{
    int opt;
    size_t temp;
    int serialAddr;

    pProgOptions->perfMon = PERF_MON_DIS;
    pProgOptions->pFile = NULL;
    pProgOptions->serialAddr = 0;

    while ((opt = getopt(argc, argv, "pf:s:")) != -1)
    {
        switch(opt)
        {
        case 'p':
            pProgOptions->perfMon = PERF_MON_EN;
            break;
        case 'f':
            temp = strlen(optarg);
            pProgOptions->pFile = malloc(temp);
            assert(pProgOptions->pFile != NULL);
            strncpy(pProgOptions->pFile, optarg, temp);
            break;
        case 's':
            serialAddr = atoi(optarg);
            if (serialAddr > 0)
            {
                pProgOptions->serialAddr = (uint32_t) serialAddr;
            }
            break;
        default:
            printf("Unrecognized option %c\n", opt);
            abort();
        }
    }

    if (pProgOptions->pFile == NULL)
    {
        printf("-f [filename] option required\n");
        abort();
    }
}

void destroyOptions(progOptions *pOptions)
{
    free(pOptions->pFile);
}

void serialMmio(riscvCore *pCore, uint32_t serialAddr)
{
    if (pCore->mmu.pMemory[serialAddr] != 0)
    {
        putchar(pCore->mmu.pMemory[serialAddr]);
        pCore->mmu.pMemory[serialAddr] = 0;
    }
}

int main(int argc, char **argv)
{
    riscvCore *pCore;
    progOptions options;
    int ret;

    parseOptions(argc, argv, &options);

    initCore(&pCore);
    initMemory(pCore, 32000);

    printf("Core initialized\n");

    ret = loadMemoryFromFile(pCore, &options);
    if (ret != LOAD_OK)
    {
        return -1;
    }

    printf("Memory Loaded\n\n");

    while (ret == CORE_OK)
    {
        ret = coreExecute(pCore);

        if (options.perfMon == 1)
        {
            performanceInfo(pCore);
        }

        if (options.serialAddr != 0)
        {
            serialMmio(pCore, options.serialAddr);
        }
    }

    printf("Core exited with ret: %d\n", ret);

    destroyMemory(pCore);
    destroyCore(pCore);
    destroyOptions(&options);

    return 0;
}
