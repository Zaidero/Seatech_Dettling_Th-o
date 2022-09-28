#ifndef PWH_H
#define PWH_H

#define MOTEUR_DROIT 1
#define MOTEUR_GAUCHE 0

void InitPWM(void);
//void PWMSetSpeed(float vitesseEnPourcents, unsigned char motorNumber);
void PWMUpdateSpeed(void);
void PWMSetSpeedConsigne(float vitesseEnPourcents, unsigned char motorNumber);
#endif 

