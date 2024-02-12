/*
 * TacheIA.c
 *
 *  Created on: 24 nov. 2020
 *      Author: sebas
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include <ti/drivers/GPIO.h>
#include <TacheIA/AlgoIA/unsupervisedLearning.h>

#include <TacheLCD/TacheLCD.h>

#include <TacheIA/TacheIA.h>
#include "ti_drivers_config.h"

#define TacheIA_TASK_PRIORITY                     3

#ifndef TacheIA_TASK_STACK_SIZE
#define TacheIA_TASK_STACK_SIZE                   1024
#endif

// Task configuration
Task_Struct TacheIA;

uint8_t TacheIAStack[TacheIA_TASK_STACK_SIZE];

Semaphore_Struct semIAStruct;
Semaphore_Handle semIAHandle;

//static Clock_Struct myClock;

float features[6];

bool systemTrained = false;


extern void TacheIA_init(void);

//void myClockSwiFxn(uintptr_t arg0)
//{
//    Semaphore_post(semIAHandle);
//}


int clustNumber;
int anomNumber;
static void TacheIA_taskFxn(UArg a0, UArg a1)
{
    // Initialize application
    TacheIA_init();
    for (;;)
    {
        Semaphore_pend(semIAHandle, BIOS_WAIT_FOREVER);

        uint_fast8_t buttonS2 = GPIO_read(S2);
        if(buttonS2 == 0){
            //Training
            Training(features);
            int clustNumber = GetNumberOfClusters();
            int clusterDetected = GetLastClusterDetected();
            LCD_PrintState(1,clustNumber,clusterDetected,anomNumber,features,6);
            systemTrained = true;
        }
        else
        {
            if(systemTrained)
            {
                    //Anomaly Detection
                    anomNumber = AnomalyDetection(features);
                    clustNumber = GetNumberOfClusters();
                    int clusterDetected = GetLastClusterDetected();
                    LCD_PrintState(0,clustNumber,clusterDetected,anomNumber,features,6);
            }
        }

    }
}

void TacheIA_CreateTask(void){

    Semaphore_Params semParams;
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = TacheIAStack;
    taskParams.stackSize = TacheIA_TASK_STACK_SIZE;
    taskParams.priority = TacheIA_TASK_PRIORITY;

    Task_construct(&TacheIA, TacheIA_taskFxn, &taskParams, NULL);

    /* Construct a Semaphore object to be used as a resource lock, inital count 0 */
        Semaphore_Params_init(&semParams);
        Semaphore_construct(&semIAStruct, 0, &semParams);
    /* Obtain instance handle */
        semIAHandle = Semaphore_handle(&semIAStruct);
}

void TacheIA_init(void){
    InitUnsupervisedKMeansModif(DistanceMaximumConfigured, AlphaConfigured, OffsetConfigured);
}

void StartUnsupervisedClassificationOrDetection(float data[], int sizeData){

    for(int i=0; i<sizeData;i++)
        features[i] = data[i];

    Semaphore_post(semIAHandle);
}
