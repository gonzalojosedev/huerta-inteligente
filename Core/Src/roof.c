#include "roof.h"

static RoofState roofState = ROOF_STOPPED;

void Roof_Init(void)
{
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_RESET);

    roofState = ROOF_STOPPED;
}

void Roof_Open(void)
{
    if (roofState == ROOF_MANUAL_STOP)
    {
        return;
    }

    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_RESET);

    roofState = ROOF_OPENING;
}

void Roof_Close(void)
{
    if (roofState == ROOF_MANUAL_STOP)
    {
        return;
    }

    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_SET);

    roofState = ROOF_CLOSING;
}

void Roof_Stop(void)
{
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_RESET);

    roofState = ROOF_MANUAL_STOP;
}

RoofState Roof_GetState(void)
{
    return roofState;
}

void Roof_ResetManualStop(void)
{
    if (roofState == ROOF_MANUAL_STOP)
    {
        roofState = ROOF_STOPPED;
    }
}
