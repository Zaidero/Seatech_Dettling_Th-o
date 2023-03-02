/* 
 * File:   CB_RX1.h
 * Author: TP_EO_6
 *
 * Created on 3 février 2023, 16:06
 */

#ifndef CB_RX1_H
#define	CB_RX1_H
int CB_RX1_GetRemainingSize(void);
int CB_RX1_GetDataSize(void);
unsigned char CB_RX1_IsDataAvailable(void);
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void);
unsigned char CB_RX1_Get(void);
void CB_RX1_Add(unsigned char value);

#ifdef	__cplusplus
}
#endif

#endif	/* CB_RX1_H */

