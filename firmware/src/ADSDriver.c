#include "ADSDriver.h"

uint8_t instanceCount = 0;
DRV_HANDLE currentHandle;
DRV_HANDLE Deinitialize(DRV_HANDLE handle)
{
    int i;
    for (i = 0; i < MAX_INSTANCES; i++)
    {
        if (instances[i] == (ADSContext *)handle)
        {
            free(instances[i]);
            instances[i] = NULL;
            return NULL;
        }
    }
    
    return DRV_HANDLE_INVALID;
}

DRV_HANDLE GetInstance()
{
    int i;
    if (instanceCount >= MAX_INSTANCES)
    {
        return DRV_HANDLE_INVALID;
    }
        
    for (i = 0; i < MAX_INSTANCES; i++)
    {
        if (instances[i] == NULL)
        {
            instances[i] = (ADSContext *)malloc(sizeof(ADSContext));
            instanceCount++;
            return  (DRV_HANDLE) instances[i];        
        }
    }
    
    return DRV_HANDLE_INVALID;
}

void UnSetCS()
{

    *(currentContext->csPort) &= ~(currentContext->csPinMask);
}

void SetCS()
{
    *(currentContext->csPort) |= currentContext->csPinMask;
}