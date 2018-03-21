
#include "contactors.h"
#include "main.h"
#include "state.h"

void initLowSideContactor() {
    // Setup GPIO for MCU On signal (And set it to be on)
    GPIO_InitTypeDef gpioinit;
    LOW_SIDE_CONTACTOR_CLK_ENABLE();
    gpioinit.Pin = LOW_SIDE_CONTACTOR_PIN;
    gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioinit.Pull = GPIO_PULLUP;
    gpioinit.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LOW_SIDE_CONTACTOR_PORT, &gpioinit);

    closeLowSideContactor();
}

void initHighSideContactor() {
    // Setup GPIO for Close contactors signal (initially off)
    GPIO_InitTypeDef gpioinit;
    HIGH_SIDE_CONTACTOR_CLK_ENABLE();
    gpioinit.Pin = HIGH_SIDE_CONTACTOR_PIN;
    gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioinit.Pull = GPIO_PULLUP;
    gpioinit.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(HIGH_SIDE_CONTACTOR_PORT, &gpioinit);  
    
    openHighSideContactor();
}

void openLowSideContactor() {
    HAL_GPIO_WritePin(LOW_SIDE_CONTACTOR_PORT, LOW_SIDE_CONTACTOR_PIN, GPIO_PIN_RESET); // OFF
    commanded_contactors.low_side = CONTACTOR_OPEN; // in the state.h
}

void closeLowSideContactor() {
    HAL_GPIO_WritePin(LOW_SIDE_CONTACTOR_PORT, LOW_SIDE_CONTACTOR_PIN, GPIO_PIN_SET); // ON
    commanded_contactors.low_side = CONTACTOR_CLOSED; // in the state.h
}


void openHighSideContactor() {
    HAL_GPIO_WritePin(HIGH_SIDE_CONTACTOR_PORT, HIGH_SIDE_CONTACTOR_PIN, GPIO_PIN_RESET); // OFF
    commanded_contactors.high_side = CONTACTOR_OPEN; // in the state.h
}

void closeHighSideContactor() {
    HAL_GPIO_WritePin(HIGH_SIDE_CONTACTOR_PORT, HIGH_SIDE_CONTACTOR_PIN, GPIO_PIN_SET); // ON
    commanded_contactors.high_side = CONTACTOR_CLOSED;  // in the state.h
}
