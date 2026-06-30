#ifndef DHT11_H
#define DHT11_H

#include "main.h"

typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data;

uint8_t DHT11_Read(DHT11_Data *data);

#endif
