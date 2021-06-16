#include "hal.h"

#define _ 0
#define DEFAULT_TIME 500
#define T 100

int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
int leds_num[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};

int pattern[7][8] = {{1, _, _, _, 1, _, _, _},
                     {_, 1, _, _, _, 1, _, _},
                     {_, _, 1, _, _, _, 1, _},
                     {_, _, _, 1, _, _, _, 1},
                     {_, _, 1, _, _, _, 1, _},
                     {_, 1, _, _, _, 1, _, _},
                     {1, _, _, _, 1, _, _, _}};

int read_pin(int index) {
    return HAL_GPIO_ReadPin(GPIOE, sw_num[index]);
}

int get_number_from_pins() {
    return read_pin(0) * 8 + read_pin(1) * 4 + read_pin(3) * 2 + read_pin(4);
}

int frame = 0;

void TIM6_IRQ_Handler() {
    for (int pin = 0; pin < 8; pin++) {
        HAL_GPIO_WritePin(GPIOD, leds_num[pin], pattern[frame][pin] == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    frame++;
    if (frame == 7) frame = 0;
    WRITE_REG(TIM6_ARR, DEFAULT_TIME + get_number_from_pins() * T);
}

int umain() {
    registerTIM6_IRQHandler(TIM6_IRQ_Handler);

    __enable_irq();

    WRITE_REG(TIM6_ARR, DEFAULT_TIME);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);
    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
}
