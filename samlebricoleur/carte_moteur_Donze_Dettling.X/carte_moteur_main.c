 /* 
 * File:   carte_moteur_main.c
 * Author: TP_EO_6
 *
 * Created on 21 septembre 2022, 14:54
 */
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    
    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();
    
    InitTimer1();
    InitTimer23();
    //Moteurs
    InitPWM();
    InitADC1();


    

    //PWMSetSpeed(20, MOTEUR_GAUCHE);
    //PWMSetSpeed(20, MOTEUR_DROIT);

 
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE  = 1;

    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
    } 
    // fin main
}
