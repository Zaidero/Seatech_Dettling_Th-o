#ifndef ROBOT_H
#define ROBOT_H

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float distanceTelemetreUNI;//Extreme-Droite
            float distanceTelemetreFACE06; //Droite
            float distanceTelemetreFAGE;//Centre
            float distanceTelemetreFNAEL;//Gauche
            float distanceTelemetreUNEF;//Extreme-Gauche
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;
#endif 


