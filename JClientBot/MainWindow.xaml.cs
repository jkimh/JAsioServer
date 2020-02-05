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
using System.Drawing;
using System.Numerics;

namespace JClientBot
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ClientList clientList;
        ClientViewList viewList;
        Ellipse selectedEllipse;
        public MainWindow()
        {
            InitializeComponent();
            clientList = Resources["MyListKey"] as ClientList;
            viewList = Resources["ViewList"] as ClientViewList;
            this.DataContext = viewList;
        }

        private void AddPointMap(double x, double y, SolidColorBrush color)
        {
            Ellipse tri = new Ellipse()
            {
                Width = 10,
                Height = 10,
                Stroke = color, //Brushes.Red,
                StrokeThickness = 6
            };

            //map.Children.Add(tri);

            //좌표
            tri.SetValue(Canvas.LeftProperty, x);
            tri.SetValue(Canvas.TopProperty, y);
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

        private void RandomMove_Button_Click(object sender, RoutedEventArgs e)
        {
            int clientCount = clientList.Count();
            Random r = new Random();
            foreach (var client in clientList)
            {
                PKS_CS_MOVE packet = new PKS_CS_MOVE();
                packet.command = PACKET_COMMAND.PACKET_CS_MOVE;
                packet.size = (uint)Marshal.SizeOf<PKS_CS_MOVE>();
                packet.dest = new Vector3(r.Next(0, 300), r.Next(0, 300), r.Next(0, 300));
                client.Send(packet);
            }
        }

        private void Ellipse_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if(this.selectedEllipse != null)
            {
                this.selectedEllipse.Fill = new SolidColorBrush(Colors.Gray);
            }
            Ellipse ellipse = (Ellipse)e.Source;
            Client client = (Client)ItemControlAll.ItemContainerGenerator.ItemFromContainer(ellipse.TemplatedParent); //TemplatedParent를 통해 얻을 수 있음
            if (client != null)
            {
                ellipse.Fill = new SolidColorBrush(Colors.Red);
                this.selectedEllipse = ellipse;
                SelectedClientName.Text = client.Name;
                lock(client.lockObject)
                {
                    viewList.Clear();
                    client.ViewList = viewList;
                }
                /*
                foreach (var view in client.ViewList.ToList())
                {
                    viewList.Add(view);
                    view.PositionX = view.PositionX;
                }
                */
                /*
                PKS_CS_MOVE packet = new PKS_CS_MOVE();
                packet.command = PACKET_COMMAND.PACKET_CS_MOVE;
                packet.size = (uint)Marshal.SizeOf<PKS_CS_MOVE>();
                Random r = new Random();
                packet.dest = new Vector3(r.Next(0, 300), r.Next(0, 300), r.Next(0, 300));
                client.Send(packet);
                */
            }
        }
    }
}
