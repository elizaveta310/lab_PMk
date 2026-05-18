#include "project.h"

static uint8_t LED_NUM[] = {
    0xC0, //0
    0xF9, //1
    0xA4, //2
    0xB0, //3
    0x99, //4
    0x92, //5
    0x82, //6
    0xF8, //7
    0x80, //8
    0x90, //9
};

static void FourDigit74HC595_sendData(uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if(data & (0x80 >> i))
            Pin_DO_Write(1);
        else
            Pin_DO_Write(0);

        Pin_CLK_Write(1);
        Pin_CLK_Write(0);
    }
}

static void FourDigit74HC595_sendOneDigit(uint8_t position,
                                          uint8_t digit,
                                          uint8_t dot)
{
    FourDigit74HC595_sendData(0xFF & ~(1 << position));

    if(dot)
        FourDigit74HC595_sendData(LED_NUM[digit] & 0x7F);
    else
        FourDigit74HC595_sendData(LED_NUM[digit]);

    Pin_Latch_Write(1);
    Pin_Latch_Write(0);
}

uint8_t display_data[8] = {0,0,0,0,0,0,3,9};

uint8_t led_counter = 0;

CY_ISR(Timer_Int_Handler)
{
    FourDigit74HC595_sendOneDigit(
        led_counter,
        display_data[led_counter],
        0
    );

    led_counter++;

    if(led_counter > 7)
        led_counter = 0;
}

int main(void)
{
    CyGlobalIntEnable;

    Timer_Start();

    Timer_Int_StartEx(Timer_Int_Handler);

   for(;;)
{
    CyDelay(500);

    uint8_t temp = display_data[0];

    for(uint8_t i = 0; i < 7; i++)
    {
        display_data[i] = display_data[i + 1];
    }

    display_data[7] = temp;
}
}