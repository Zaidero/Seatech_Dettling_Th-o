using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;


namespace RobotInterfaceTheo_Sam
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Robot robot = new Robot();

        ReliableSerialPort serialPort1;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM6" +
                "", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += serialPort1_DataReceived;
            serialPort1.Open();
            DispatcherTimer timerAffichage;
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; 
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if(robot.receivedText!="")
            //{
            //textBoxReception.Text += robot.receivedText;
            //robot.receivedText = "";
            //}
            while (robot.byteListReceived.Count > 0)
            {
                var c = robot.byteListReceived.Dequeue();
                DecodeMessage(c);
                //textBoxReception.Text += " "+ c.ToString("X2");

            }

        }

       
        public void serialPort1_DataReceived(object sender, DataReceivedArgs e)
        {

            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (int i = 0; i < e.Data.Length; i++) {

                robot.byteListReceived.Enqueue(e.Data[i]);
                
            }
        }
      

        //bool toggle = false;
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            SendMessage();
        }
        //{
        //if(toggle)
        //buttonEnvoyer.Background = Brushes.RoyalBlue;
        //else
        //buttonEnvoyer.Background = Brushes.Beige;
        //toggle = !toggle;
        //}
        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text= "";
        }





        private void SendMessage()
        {
            serialPort1.WriteLine(textBoxEmission.Text);
            //textBoxReception.Text += "Reçu : " + textBoxEmission.Text + "\r";
            textBoxEmission.Text = "";
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        byte[] byteList = new byte[20];
        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //for (int i=0; i<=19; i++)
            //{
            //  byteList[i] = (byte)(2 * i);
            //}
            //serialPort1.Write(byteList, 0, byteList.Length);
            int msgFunction = 0x0080;
            string s = "Bonjour";
            byte[] Payload = Encoding.ASCII.GetBytes(s);
            UartEncodeAndSendMessage( msgFunction,  Payload.Length, Payload);

            UartEncodeAndSendMessage(0x0020, 2, new byte[] { 1, 1 });
            UartEncodeAndSendMessage(0x0030, 3, new byte[] { 50, 40, 25 });
            UartEncodeAndSendMessage(0x0040, 2, new byte[] { 15, 15 });
            UartEncodeAndSendMessage(0x0080, 2, new byte[] { 15, 15 });

        }
       
        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload) {
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8); 
            checksum ^= (byte)(msgFunction>>0);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++) {
                checksum ^= (byte)(msgPayload[i]);
            }
            return checksum;
        }
        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload) {
            byte[] frame = new byte[6+msgPayloadLength];
            frame[0] = 0xFE;
            frame[1] = (byte)(msgFunction >> 8);
            frame[2] = (byte)(msgFunction >> 0);
            frame[3] = (byte)(msgPayloadLength >> 8);
            frame[4] = (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                frame[5+i]= (byte)(msgPayload[i]);
            }
            byte checksum = CalculateChecksum( msgFunction, msgPayloadLength, msgPayload);
            frame[5 + msgPayloadLength] = checksum;
            serialPort1.Write(frame, 0, frame.Length);
        }
        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }
        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        private void DecodeMessage(byte c)
        {
            switch(rcvState)
            { 
                case StateReception.Waiting:
                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c << 0;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else
                    {
                        rcvState = StateReception.Payload;
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                    }
                    break;
                case StateReception.Payload:                    
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;
                case StateReception.CheckSum:
                    byte receivedChecksum = c;
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {
                        Console.WriteLine(" T'as gagné une glace");
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                    {
                        Console.WriteLine(" Mange une ATMega 328");
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch(msgFunction)
            {
                case 0x0020:

                    switch (msgPayload[0]) { 
                        case  0 :
                            if (msgPayload[1] == 1) { labelCheckBoxLed1.IsChecked = true; }
                            else { labelCheckBoxLed1.IsChecked = false; };
                        break;
                        case 1:
                            if (msgPayload[1] == 1) { labelCheckBoxLed2.IsChecked = true; }
                            else { labelCheckBoxLed2.IsChecked = false; };
                            break;
                        case 2:
                            if (msgPayload[1] == 1) { labelCheckBoxLed3.IsChecked = true; }
                            else { labelCheckBoxLed3.IsChecked = false; };
                            break;
                    }
                    break;
                case 0x0030:
                    labelIrGauche.Content = "IR Gauche = " + msgPayload[0].ToString() + "cm";
                    labelIrCentre.Content = "IR Centre = " + msgPayload[1].ToString() + "cm";
                    labelIrDroit.Content = "IR Droit = " + msgPayload[2].ToString() + "cm";
                    break;
                case 0x0040:
                    labelVitesseDroit.Content = "Vitesse moteur Droit = " + msgPayload[0].ToString() + "%";
                    labelVitesseGauche.Content = "Vitesse moteur Gauche = " + msgPayload[1].ToString() + "%";
                    break;
                case 0x0080:
                        labelTransmissionDeTexte.Content = System.Text.Encoding.ASCII.GetString(msgPayload);
                    break;
            }
        }

    }
}
