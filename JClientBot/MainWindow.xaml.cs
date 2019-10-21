using System;
using System.Collections.Generic;
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
using System.Runtime.InteropServices;

namespace JClientBot
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ClientList clientList;
        public MainWindow()
        {
            InitializeComponent();
            clientList = Resources["MyListKey"] as ClientList;
        }

        private void ID_Button_Click(object sender, RoutedEventArgs e)
        {
            clientList.Clear();
            int reqClientCount = int.Parse(Count_Text.Text);
            for (int i=0; i< reqClientCount; i++)
            {
                Client client = new Client();
                client.Name = ID_Text.Text + i.ToString();
                client.Connect(ID_Text.Text);
                client.Error = "-";
                client.Chat = "1";
                clientList.Add(client);
            }
        }

        private void Count_Text_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            int i = 0;
            e.Handled = !int.TryParse(e.Text, out i);
        }

        private void Chat_Button_Click(object sender, RoutedEventArgs e)
        {
            foreach (var client in clientList)
            {
                PKS_CS_CHAT packet = new PKS_CS_CHAT();
                packet.command = PACKET_COMMAND.PACKET_CS_CHAT;
                packet.size = (uint)Marshal.SizeOf<PKS_CS_CHAT>();
                Array.Copy(Chat_Text.Text.ToCharArray(), packet.chat, Chat_Text.Text.Length);
                client.Send(packet); 
            }
        }

        private void Logout_Button_Click(object sender, RoutedEventArgs e)
        {
            foreach (var client in clientList)
            {
                PKS_CS_LOGOUT packet = new PKS_CS_LOGOUT();
                packet.command = PACKET_COMMAND.PACKET_CS_LOGOUT;
                packet.size = (uint)Marshal.SizeOf<PKS_CS_LOGOUT>();
                client.Send(packet);
                client.Close();
            }
        }
    }
}
