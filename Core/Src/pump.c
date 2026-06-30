#include "pump.h"

void Pump_Init(void)
{
    Pump_Off();
}

void Pump_On(void)
{
    HAL_GPIO_WritePin(PUMP_RELAY_GPIO_Port,
                      PUMP_RELAY_Pin,
                      GPIO_PIN_RESET);
}

void Pump_Off(void)
{
    HAL_GPIO_WritePin(PUMP_RELAY_GPIO_Port,
                      PUMP_RELAY_Pin,
                      GPIO_PIN_SET);
}
