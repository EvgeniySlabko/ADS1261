#include "ADSDriver.h"

uint8_t instanceCount = 0;
bool stopReading = false; 
//DRV_HANDLE currentHandle;
DRV_HANDLE Deinitialize(DRV_HANDLE handle)
{
    int i;
    for (i = 0; i < MAX_INSTANCES; i++)
    {
        if (instances[i] == (ADSContext *)handle)
        {
            instanceCount--;
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

bool VerifyHande(DRV_HANDLE adsHandle)
{
    if (adsHandle == 0) return false;
    int i;
    for (i = 0; i < MAX_INSTANCES; i++)
    {
        if (instances[i] = (ADSContext *)adsHandle)
        {
            return true;
        }
    }
    
    return false;
}

void UnSetCS(ADSContext *context)
{
    *(context->csPort) &= ~(context->csPinMask);
}

void SetCS(ADSContext *context)
{
    *(context->csPort) |= (context->csPinMask);
}