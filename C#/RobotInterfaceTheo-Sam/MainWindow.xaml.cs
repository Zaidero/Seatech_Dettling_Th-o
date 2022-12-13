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

namespace RobotInterfaceTheo_Sam
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += serialPort1_DataReceived;
            serialPort1.Open();
        }

        public void serialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            textBoxReception.Text += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        //bool toggle = false;
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        //{
        //if(toggle)
        //buttonEnvoyer.Background = Brushes.RoyalBlue;
        //else
        //buttonEnvoyer.Background = Brushes.Beige;
        //toggle = !toggle;
        //}
        {

            SendMessage();

        }


       private void SendMessage()
        {
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxReception.Text += "Reçu : " + textBoxEmission.Text + "\r";
            textBoxEmission.Text = "";
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }
    }
}
