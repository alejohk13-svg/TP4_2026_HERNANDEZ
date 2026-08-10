#ifndef __MENU_H
#define __MENU_H

#include "stdint.h"
#define TOTAL_OPCIONES  5
#define LINEAS_DISPLAY  4

typedef enum {
    PANTALLA_PRINCIPAL,
    PANTALLA_TECLA,
    PANTALLA_BACKLIGHT,
    PANTALLA_CONTADOR,
    PANTALLA_SEGUIDOR,
    PANTALLA_VOLTIMETRO
} modo_pantalla_t;

void MENU_Init(void);
void MENU_MostrarOpciones(void);
void MENU_Update(char tecla);

void PERIFERICOS_Init(void);
uint16_t ADC1_LeerCanal(uint8_t canal);

#endif /* __MENU_H */
