#include "MENU.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stdio.h"
#include "../LCD/LCD.h"
#include "../Teclado/TECLADO.h"
#include "global.h"

static modo_pantalla_t modo_actual = PANTALLA_PRINCIPAL;
static uint8_t opcion_seleccionada = 0;
static uint8_t ventana_inicio = 0;
static uint8_t backlight_encendido = 1;
static uint32_t localSystickContador = 0;

static const char *opciones_menu[] = {
    "1.Tecla Presionada ",
    "2.Backlight        ",
    "3.Contador         ",
    "4.Seguidor ADC-DAC ",
    "5.Voltimetro       "
};

void PERIFERICOS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DAC_InitTypeDef  DAC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div4;
    ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode          = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode    = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion       = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);

    DAC_InitStructure.DAC_Trigger        = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer   = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_2, ENABLE);
}

uint16_t Read_ADC_Value(void)
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
    ADC_SoftwareStartConv(ADC1);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

void MENU_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    backlight_encendido = 1;

    PERIFERICOS_Init();

    LCD_clrscr();
    modo_actual = PANTALLA_PRINCIPAL;
    localSystickContador = 0;
    MENU_MostrarOpciones();
}

void MENU_MostrarOpciones(void)
{
    uint8_t i;
    uint8_t indice_opcion;

    for (i = 0; i < LINEAS_DISPLAY; i++)
    {
        indice_opcion = ventana_inicio + i;
        LCD_gotoxy(0, i);

        if (indice_opcion == opcion_seleccionada)
        {
            LCD_putc('>');
        }
        else
        {
            LCD_putc(' ');
        }

        LCD_puts((char *)opciones_menu[indice_opcion]);
    }
}

void MENU_Update(char tecla)
{
    char buffer[20];
    uint16_t adc_raw = 0;
    uint32_t mv_actual = 0;

    switch (modo_actual)
    {
        case PANTALLA_PRINCIPAL:
            if (tecla == 'A')
            {
                if (opcion_seleccionada > 0)
                {
                    opcion_seleccionada--;
                    if (opcion_seleccionada < ventana_inicio)
                    {
                        ventana_inicio = opcion_seleccionada;
                    }
                    LCD_clrscr();
                    MENU_MostrarOpciones();
                }
            }
            else if (tecla == 'B')
            {
                if (opcion_seleccionada < (TOTAL_OPCIONES - 1))
                {
                    opcion_seleccionada++;
                    if (opcion_seleccionada >= (ventana_inicio + LINEAS_DISPLAY))
                    {
                        ventana_inicio++;
                    }
                    LCD_clrscr();
                    MENU_MostrarOpciones();
                }
            }
            else if (tecla == 'C')
            {
                if (opcion_seleccionada == 0)
                {
                    modo_actual = PANTALLA_TECLA;
                    LCD_clrscr();
                    LCD_WriteString(0, 0, "Tecla: --");
                }
                else if (opcion_seleccionada == 1)
                {
                    modo_actual = PANTALLA_BACKLIGHT;
                    LCD_clrscr();
                    LCD_WriteString(0, 0, "Backlight: ");
                    if (backlight_encendido) { LCD_WriteString(11, 0, "ON "); }
                    else { LCD_WriteString(11, 0, "OFF"); }
                }
                else if (opcion_seleccionada == 2)
                {
                    modo_actual = PANTALLA_CONTADOR;
                    LCD_clrscr();
                    localSystickContador = getSystick();
                    sprintf(buffer, "Contador: %lu s", getSeconds());
                    LCD_WriteString(0, 0, buffer);
                }
                else if (opcion_seleccionada == 3)
                {
                    modo_actual = PANTALLA_SEGUIDOR;
                    LCD_clrscr();
                    localSystickContador = getSystick();
                    LCD_WriteString(0, 0, "AD1:    0 mV");
                    LCD_WriteString(0, 1, "DAC:    0 mV");
                }
                else if (opcion_seleccionada == 4)
                {
                    modo_actual = PANTALLA_VOLTIMETRO;
                    LCD_clrscr();
                    LCD_WriteString(0, 0, "HOLD: 0 mV");
                    LCD_WriteString(0, 1, "AD2: 0 mV");
                }
            }
            else if (tecla == '#')
            {
                backlight_encendido = 1;
                GPIO_SetBits(GPIOD, GPIO_Pin_8);
            }
            else if (tecla == '*')
            {
                backlight_encendido = 0;
                GPIO_ResetBits(GPIOD, GPIO_Pin_8);
            }
            break;

        case PANTALLA_TECLA:
            if (tecla == 'D')
            {
                modo_actual = PANTALLA_PRINCIPAL;
                LCD_clrscr();
                MENU_MostrarOpciones();
            }
            else if (tecla != 0)
            {
                LCD_gotoxy(7, 0);
                LCD_putc(tecla);
                LCD_putc(' ');
            }
            break;

        case PANTALLA_BACKLIGHT:
            if (tecla == 'D')
            {
                modo_actual = PANTALLA_PRINCIPAL;
                LCD_clrscr();
                MENU_MostrarOpciones();
            }
            else if (tecla == '#')
            {
                backlight_encendido = 1;
                GPIO_SetBits(GPIOD, GPIO_Pin_8);
                LCD_WriteString(11, 0, "ON ");
            }
            else if (tecla == '*')
            {
                backlight_encendido = 0;
                GPIO_ResetBits(GPIOD, GPIO_Pin_8);
                LCD_WriteString(11, 0, "OFF");
            }
            break;

        case PANTALLA_CONTADOR:
            if (tecla == 'D')
            {
                modo_actual = PANTALLA_PRINCIPAL;
                LCD_clrscr();
                MENU_MostrarOpciones();
            }
            else if ((getSystick() - localSystickContador) >= 1000)
            {
                localSystickContador = getSystick();
                sprintf(buffer, "Contador: %lu s", getSeconds());
                LCD_WriteString(0, 0, buffer);
            }
            break;

        case PANTALLA_SEGUIDOR:
            if (tecla == 'D')
            {
                modo_actual = PANTALLA_PRINCIPAL;
                LCD_clrscr();
                MENU_MostrarOpciones();
            }
            else
            {
                if ((getSystick() - localSystickContador) >= 150)
                {
                    localSystickContador = getSystick();

                    adc_raw = Read_ADC_Value();

                    if (adc_raw > 4095) {
                        adc_raw = 4095;
                    }

                    DAC_SetChannel2Data(DAC_Align_12b_R, adc_raw);

                    mv_actual = ((uint32_t)adc_raw * 5000) / 4095;

                    sprintf(buffer, "AD1: %4lu mV ", mv_actual);
                    LCD_WriteString(0, 0, buffer);
                    sprintf(buffer, "DAC: %4lu mV ", mv_actual);
                    LCD_WriteString(0, 1, buffer);
                }
            }
            break;

        case PANTALLA_VOLTIMETRO:
            if (tecla == 'D')
            {
                modo_actual = PANTALLA_PRINCIPAL;
                LCD_clrscr();
                MENU_MostrarOpciones();
            }
            break;

        default:
            modo_actual = PANTALLA_PRINCIPAL;
            break;
    }
}
