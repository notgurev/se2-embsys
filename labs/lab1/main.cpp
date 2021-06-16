#include "hal.h"

#define _ 0

int delay = 500;
int pattern[8][8] = {
        {1, 1, _, _, _, _, _, 1},
        {_, 1, 1, _, _, _, 1, _},
        {_, _, 1, 1, _, 1, _, _},
        {_, _, _, 1, 1, _, _, _},
        {_, _, _, 1, 1, 1, _, _},
        {_, _, 1, _, _, 1, 1, _},
        {_, 1, _, _, _, _, 1, 1},
        {1, _, _, _, _, _, _, 1},
};
int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
int leds_num[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                  GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                  GPIO_PIN_11, GPIO_PIN_12};

bool check_switches() {
    // N = 0x1
    return HAL_GPIO_ReadPin(GPIOE, sw_num[0]) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(GPIOE, sw_num[1]) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(GPIOE, sw_num[2]) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(GPIOE, sw_num[3]) == GPIO_PIN_SET;
}

void diode(GPIO_PinState pin13, GPIO_PinState pin14, GPIO_PinState pin15) {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, pin13);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, pin14);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, pin15);
}

void green() {
    diode(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
}

void yellow() {
    diode(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET);
}

void red() {
    diode(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET);
}

bool check_nButton_pressed() {
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET;
}

void pause_and_wait_for_nButton() {
    red();
    HAL_Delay(250);
    while (!check_nButton_pressed()) {}; // wait until button is pressed again
    green();
}

void play_animation() {
    for (int frame = 0; frame < 8; ++frame) {
        for (int pin = 0; pin < 8; ++pin) {
            HAL_GPIO_WritePin(GPIOD, leds_num[pin], pattern[frame][pin] == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
        if (check_nButton_pressed()) pause_and_wait_for_nButton();
        if (!check_switches()) return;
        HAL_Delay(delay);
    }
}

void reset_leds(int first, int last) {
    for (int i = first; i <= last; ++i) {
        HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
    }
}

void mirror_switches() {
    reset_leds(4, 7);
    for (int i = 0; i < 4; i++) {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, sw_num[i]);
        HAL_GPIO_WritePin(GPIOD, leds_num[i], state);
    }
}

int umain() {
    while (true) {
        if (check_switches()) {
            reset_leds(0, 7);
            green();
            play_animation();
        } else {
            yellow();
            mirror_switches();
        }
    }
}



