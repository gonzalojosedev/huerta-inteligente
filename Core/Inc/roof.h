#ifndef ROOF_H
#define ROOF_H

#include "main.h"

typedef enum
{
    ROOF_STOPPED = 0,
    ROOF_OPENING,
    ROOF_CLOSING,
    ROOF_MANUAL_STOP
} RoofState;

void Roof_Init(void);
void Roof_Open(void);
void Roof_Close(void);
void Roof_Stop(void);
RoofState Roof_GetState(void);
void Roof_ResetManualStop(void);

#endif
