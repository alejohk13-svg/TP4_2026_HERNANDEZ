#include "MENU.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdio.h"
#include "../LCD/LCD.h"
#include "../Teclado/TECLADO.h"
#include "global.h"

static modo_pantalla_t modo_actual = PANTALLA_PRINCIPAL;
static uint8_t opcion_seleccionada = 0;
static uint8_t ventana_inicio = 0;
static uint8_t backlight_encendido = 1;
static uint32_t localSystickContador = 0;

// Reemplazados los menúes 4 y 5 en la lista principal
static const char *opciones_menu[] = {
    "1.Tecla Presionada ",
    "2.Backlight        ",
    "3.Contador         ",
    "4.Seguidor ADC-DAC ", // Menú 4 modificado
    "5.Voltimetro       "  // Menú 5 modificado
};

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
                else if (opcion_seleccionada == 3) // Caso 4: Seguidor ADC-DAC
                {
                    modo_actual = PANTALLA_SEGUIDOR;
                    LCD_clrscr();
                    LCD_WriteString(0, 0, "AD1: 0 mV");
                    LCD_WriteString(0, 1, "DAC: 0 mV");
                }
                else if (opcion_seleccionada == 4) // Caso 5: Voltímetro
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
