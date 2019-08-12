#ifndef MMU_H
#define MMU_H

#include <stdint.h>

#define PERM_R      1
#define PERM_W      2
#define PERM_X      4

#define MMU_OK      0
#define MMU_NOK     -1

typedef struct
{
    uint32_t memStart;
    uint32_t memSize;
    uint8_t *pMemory;
    uint8_t memPermissions;
} lrMmu;



#endif
