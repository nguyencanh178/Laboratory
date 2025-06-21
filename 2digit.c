#include "stm32f10x.h"
#include "stm32f10x_GPIO.h"
#include "stm32f10x_rcc.h"

void delay(unsigned int time) {
    unsigned int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 0x2aff; j++){

        }
    }   
}

uint16_t seg7_A[] = {
    GPIO_Pin_0, 
    GPIO_Pin_1, 
    GPIO_Pin_2 , 
    GPIO_Pin_3, 
    GPIO_Pin_4, 
    GPIO_Pin_5, 
    GPIO_Pin_6, 
    GPIO_Pin_7
};

uint16_t seg7_B[] = {
    GPIO_Pin_0, 
    GPIO_Pin_1, 
    GPIO_Pin_2 , 
    GPIO_Pin_3, 
    GPIO_Pin_4, 
    GPIO_Pin_5, 
    GPIO_Pin_6, 
    GPIO_Pin_7
};


void GPIOA_init(void) {
    GPIO_InitTypeDef GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //  Cau hinh chan led hien thi 7 doan
    GPIO.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;

    // Cau hinh chan dieu khien
    GPIO.GPIO_Pin = GPIO_Pin_9;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO);

}

void GPIOB_init(void){
    GPIO_InitTypeDef GPIO;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //  Cau hinh chan led hien thi 7 doan
    GPIO.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;

    // Cau hinh chan dieu khien
    GPIO.GPIO_Pin = GPIO_Pin_9;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO);
}

uint16_t seg7_hex[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0x8F, 0x80, 0x90}; // Led 7 doan

// led 7 thanh 2 digit thi phai cau hinh them 2 chan dieu khien cho 2 con led 7 thanh
int main() {
    int i, loop;
    GPIOA_init();
    GPIOB_init();

    while(1) {
    for (i = 0; i <= 9; i++){
        //Quet led 7 doan
        GPIO_ResetBits(GPIOA, GPIO_Pin_9); //Thuc hien tat tat ca cac led
        GPIOA->ODR = seg7_hex[i]; //Truyen ma Hex so 0, 1, ... de bat led 
        delay(100);
        GPIO_SetBits(GPIOA, GPIO_Pin_9); // Sau khi truyen xong thi bat led len 
        }
    }
}