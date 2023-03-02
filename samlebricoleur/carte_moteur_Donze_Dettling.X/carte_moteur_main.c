/* 
 * File:   carte_moteur_main.c
 * Author: TP_EO_6
 *
 * Created on 21 septembre 2022, 14:54
 */
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "carte_moteur_main.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"

#define FCY 40000000


unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;
unsigned int ADCValue3;
unsigned int ADCValue4;

unsigned char sensorState;

int main(void) {
    InitUART();
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();

    InitTimer1(250);
    InitTimer4(1000);
    //Moteurs
    InitPWM();
    InitADC1();




    //PWMSetSpeed(10, MOTEUR_GAUCHE);
    //PWMSetSpeed(10, MOTEUR_DROIT);




    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {

        //SendMessage((unsigned char*) "Bonjour", 7);
        int i;
        for (i = 0; i < CB_RX1_GetDataSize(); i++) {
            unsigned char c = CB_RX1_Get();
            SendMessage(&c, 1);
        }

        if (ADCIsConversionFinished()) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            ADCValue0 = result[0];
            ADCValue1 = result[1];
            ADCValue2 = result[2];
            ADCValue3 = result[3];
            ADCValue4 = result[4];
            float volts = ((float) result[0])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreUNI = 34 / volts - 5;
            volts = ((float) result[1])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreFACE06 = 34 / volts - 5;
            volts = ((float) result[2])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreFAGE = 34 / volts - 5;
            volts = ((float) result[4])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreFNAEL = 34 / volts - 5;
            volts = ((float) result[3])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreUNEF = 34 / volts - 5;

            sensorState = 0;
            if (robotState.distanceTelemetreUNEF <= 25)
                sensorState = sensorState | 0b10000;
            if (robotState.distanceTelemetreFNAEL <= 30)
                sensorState = sensorState | 0b01000;
            if (robotState.distanceTelemetreFAGE <= 30)
                sensorState = sensorState | 0b00100;
            if (robotState.distanceTelemetreFACE06 <= 30)
                sensorState = sensorState | 0b00010;
            if (robotState.distanceTelemetreUNI <= 25)
                sensorState = sensorState | 0b00001;

        }
    }
}


// fin main
unsigned char stateRobot;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(28, MOTEUR_DROIT);
            PWMSetSpeedConsigne(28, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_VICTORY:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_VICTORY_EN_COURS;
            break;
        case STATE_VICTORY_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(22, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-3, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE_LEGER:
            PWMSetSpeedConsigne(12, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-3, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_LEGER_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-3, MOTEUR_DROIT);
            PWMSetSpeedConsigne(22, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE_LEGER:
            PWMSetSpeedConsigne(-3, MOTEUR_DROIT);
            PWMSetSpeedConsigne(12, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_LEGER_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_LEGER:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_LEGER_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_LEGER:
            PWMSetSpeedConsigne(-10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_LEGER_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_180:
            PWMSetSpeedConsigne(-30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_180_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_180_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {

    switch (sensorState) {
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b00001:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b00010:
            nextStateRobot = STATE_TOURNE_GAUCHE_LEGER;
            break;
        case 0b00011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_LEGER;
            break;
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_LEGER;
            break;
        case 0b00101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01000:
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
        case 0b01001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b01011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b01111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10000:
            nextStateRobot = STATE_TOURNE_DROITE_LEGER;
            break;
        case 0b10001:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b10010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b10100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b10110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11000:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE_LEGER;
            break;
        case 0b11001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;
        case 0b11111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_180;
            break;


    }
    //    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //    
    //    
    //    //Détermination de la position des obstacles en fonction des télémètres
    //    if (robotState.distanceTelemetreFACE06 < 30 &&
    //            robotState.distanceTelemetreFAGE > 20 &&
    //            robotState.distanceTelemetreFNAEL > 30) //Obstacle à droite
    //        positionObstacle = OBSTACLE_A_DROITE;
    //    else if (robotState.distanceTelemetreFACE06 > 30 &&
    //            robotState.distanceTelemetreFAGE > 20 &&
    //            robotState.distanceTelemetreFNAEL < 30) //Obstacle à gauche
    //        positionObstacle = OBSTACLE_A_GAUCHE;
    //    else if (robotState.distanceTelemetreFAGE < 20) //Obstacle en face
    //        positionObstacle = OBSTACLE_EN_FACE;
    //    else if (robotState.distanceTelemetreFACE06 > 30 &&
    //            robotState.distanceTelemetreFAGE > 20 &&
    //            robotState.distanceTelemetreFNAEL > 30) //pas d?obstacle
    //        positionObstacle = PAS_D_OBSTACLE;
    //
    //    //Détermination de l?état à venir du robot
    //    if (positionObstacle == PAS_D_OBSTACLE)
    //        nextStateRobot = STATE_AVANCE;
    //    else if (positionObstacle == OBSTACLE_A_DROITE)
    //        nextStateRobot = STATE_TOURNE_GAUCHE;
    //    else if (positionObstacle == OBSTACLE_A_GAUCHE)
    //        nextStateRobot = STATE_TOURNE_DROITE;
    //    else if (positionObstacle == OBSTACLE_EN_FACE)
    //        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}
