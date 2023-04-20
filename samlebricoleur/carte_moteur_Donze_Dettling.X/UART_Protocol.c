#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction >> 0);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }

    return checksum;

}
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction d?encodage et d?envoi d?un message
        unsigned char message[6 + msgPayloadLength];
        unsigned char checksum = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
        int pos = 0;
        message[pos++] = 0xFE;
        message[pos++] = (unsigned char) (msgFunction >> 8);
        message[pos++] = (unsigned char) (msgFunction >> 0);
        message[pos++] = (unsigned char) (msgPayloadLength >> 8);
        message[pos++] = (unsigned char) (msgPayloadLength >> 0);

        for (int i = 0; i < msgPayloadLength; i++) {
            message[pos++] = msgPayload[i];
        }
        message[pos++] = checksum;
        //serialPort1.WriteLine("0xFE");
        SendMessage(message, pos);
        
}
enum StateReception
  {
           Waiting,
           FunctionMSB,
           FunctionLSB,
          PayloadLengthMSB,
          PayloadLengthLSB,
          Payload,
          CheckSum
  };
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
enum StateReception rcvState = Waiting;

void UartDecodeMessage(unsigned char c)
{
switch(rcvState)
            { 
                case Waiting:
                    if (c == 0xFE)
                        rcvState = FunctionMSB;
                    break;
                case FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = FunctionLSB;
                    break;
                case FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState = PayloadLengthMSB;
                    break;
                case PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = PayloadLengthLSB;
                    break;
                case PayloadLengthLSB:
                    msgDecodedPayloadLength += c << 0;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = CheckSum;
                    else
                    {
                        rcvState = Payload;
                        //unsigned char msgDecodedPayload[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                    }
                    break;
                case Payload:                    
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        rcvState = CheckSum;
                    break;
                case CheckSum:
                {
                    unsigned char receivedChecksum = c;
                    unsigned char calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {
                        //UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                    {
                        //SendMessage(" Mange une ATMega 328");
                    }
                    rcvState = Waiting;
                }
                    break;
                default:
                    rcvState = Waiting;
                    break;
            }
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    
}


//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
