#include "dht11.h"

extern TIM_HandleTypeDef htim3;

#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_10

static void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);

    while (__HAL_TIM_GET_COUNTER(&htim3) < us)
    {
    }
}

static void DHT11_SetPinOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_SetPinInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static uint8_t DHT11_WaitForPin(GPIO_PinState state, uint16_t timeout_us)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == state)
    {
        if (__HAL_TIM_GET_COUNTER(&htim3) > timeout_us)
        {
            return 0;
        }
    }

    return 1;
}

uint8_t DHT11_Read(DHT11_Data *data)
{
    uint8_t i;
    uint8_t j;
    uint8_t bits[5] = {0};
    uint8_t checksum;

    DHT11_SetPinOutput();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    delay_us(30);

    DHT11_SetPinInput();

    if (DHT11_WaitForPin(GPIO_PIN_SET, 200) == 0U)
    {
        return 2;
    }

    if (DHT11_WaitForPin(GPIO_PIN_RESET, 200) == 0U)
    {
        return 3;
    }

    if (DHT11_WaitForPin(GPIO_PIN_SET, 200) == 0U)
    {
        return 4;
    }

    for (j = 0; j < 5; j++)
    {
        for (i = 0; i < 8; i++)
        {
            if (DHT11_WaitForPin(GPIO_PIN_RESET, 200) == 0U)
            {
                return 5;
            }

            delay_us(50);

            if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
            {
                bits[j] |= (1U << (7U - i));

                if (DHT11_WaitForPin(GPIO_PIN_SET, 200) == 0U)
                {
                    return 6;
                }
            }
        }
    }

    checksum = bits[0] + bits[1] + bits[2] + bits[3];

    if (checksum != bits[4])
    {
        return 0;
    }

    data->humidity = bits[0];
    data->temperature = bits[2];

    return 1;
}


