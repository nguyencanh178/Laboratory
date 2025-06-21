#include "stm32f10x.h"

void delay(unsigned int time) {
		int i, j;
    for ( i = 0; i < time; i++) {
        for ( j = 0; j < 0x2AFF; j++) {

        }
    }
}

uint8_t seg7_arr[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void GPIO_Config(void) {
		GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                    GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOA, &gpio);
}

void clear_segments(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                           GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}

void display_digit(uint8_t num, uint8_t digit_pos) {
		int i;
		uint8_t led = seg7_arr[num];
    clear_segments();

    for (i = 0; i < 7; i++) {
        if (led & (1 << i)) {
            GPIO_SetBits(GPIOA, (1 << i)); 
        } else {
            GPIO_ResetBits(GPIOA, (1 << i));
        }
    }

    if (digit_pos == 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_8); 
        GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    } else {
        GPIO_SetBits(GPIOA, GPIO_Pin_9);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    }

    delay(2);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9);
}

int main(void) {
    GPIO_Config();

    while (1) {
				int i, j;
        for (i = 0; i <= 99; i++) {
            for (j = 0; j < 50; j++) {
                display_digit(i / 10, 0); 
                display_digit(i % 10, 1);
            }

            delay(100); 
        }
    }
}
