#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"
#define CBTX1_BUFFER_SIZE 128
int cbTx1Head;
int cbTx1Tail;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;

void SendMessage(unsigned char* message, int length) {
    unsigned char i = 0;
    if (CB_TX1_GetRemainingSize() > length) {
        //On peut écrire le message
        for (i = 0; i < length; i++)
            CB_TX1_Add(message[i]);
        if (!CB_TX1_IsTranmitting())
            SendOne();
    }
}

void CB_TX1_Add(unsigned char value) {
    //on stocke la valeur dans le buffer
    cbTx1Buffer[cbTx1Head] = value;
    // La position de Head augmente de 1
    cbTx1Head++;
    // Si le Head arrive au maximum du buffer (128), le head retourne à la position 0 (pour créer le buffer en boucle)
    if (cbTx1Head >= CBTX1_BUFFER_SIZE)
        cbTx1Head = 0;
}

unsigned char CB_TX1_Get(void) {
    unsigned char get;
    //get est la valeur à ressortir de Tail (la fin de la boucle) pour être traitée
    get = cbTx1Buffer[cbTx1Tail];
    // La position de la queue augmente de 1 
    cbTx1Tail++;
    // Si le Head arrive au maximum du buffer (128), le head retourne à la position 0 (pour créer le buffer en boucle)
    if (cbTx1Tail >= CBTX1_BUFFER_SIZE)
        cbTx1Tail = 0;
    return get;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // clear TX interrupt flag
    if (cbTx1Tail != cbTx1Head) {
        SendOne();
    } else
        isTransmitting = 0;
}

void SendOne() {
    isTransmitting = 1;
    unsigned char value = CB_TX1_Get();
    U1TXREG = value; // Transmit one character
}

unsigned char CB_TX1_IsTranmitting(void) {
    return isTransmitting;
}

int CB_TX1_GetDataSize(void) {
    //return size of data stored in circular buffer
    int dataSize;
    if (cbTx1Tail <= cbTx1Head) //On vérifie que la queue de la boucle soit inférieure à la position de la tête de la boucle pour calculer la taille des données stockées
        dataSize = cbTx1Head - cbTx1Tail; // La queue est inférieure ==> la taille est simplement la tête moins la queue
    else
        dataSize = CBTX1_BUFFER_SIZE - (cbTx1Tail - cbTx1Head);
    return dataSize;
}

int CB_TX1_GetRemainingSize(void) {
    //return size of remaining size in circular buffer
    return CBTX1_BUFFER_SIZE - CB_TX1_GetDataSize(); //La taille restante de sotckage dispo est égale à la taille du buffer moins la taille stockée

}