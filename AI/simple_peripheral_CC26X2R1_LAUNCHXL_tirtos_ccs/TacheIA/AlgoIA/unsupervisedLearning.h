/*
 * unsupervisedLearning.h
 *
 *  Created on: 25 nov. 2020
 *      Author: sebas
 */

#ifndef TACHEIA_ALGORITHMSIA_UNSUPERVISEDLEARNING_H_
#define TACHEIA_ALGORITHMSIA_UNSUPERVISEDLEARNING_H_

#define DistanceMaximumConfigured           1.15
#define OffsetConfigured                    1
#define AlphaConfigured                     0.05

#define NeuronsInputLayerC                  3
#define MaxQClustersC                       25
#define ActualNumberOfClustersC             1
#define NumberSamplesTrainingSetC           25
//Minimum number of samples by cluster
#define minimumNumberOfSamplesInCluster     4
#define NumberOfSamplesForInitialization    6


typedef enum{
    First_Sample,
    Clusters_Comparison,
}StateTraining;


void InitUnsupervisedKMeansModif(float DistanceMax, float alphaLPF, float clusterOffset);
int Training(float sample[]);
int AnomalyDetection(float sample[]);
int GetNumberOfClusters(void);
int GetClusterDimension(void);
float LPFKMeans(float xi, float yi_1, float alpha);
void ResetAnomaliesCounter(void);
int GetLastClusterDetected(void);

#endif /* TACHEIA_ALGORITHMSIA_UNSUPERVISEDLEARNING_H_ */
