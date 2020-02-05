using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Numerics;
using System.Linq;

namespace JClientBot
{
    public class ClientList : ObservableCollection<Client> { }
    public enum PACKET_COMMAND
    {
        PACKET_CS_LOGIN = 0,
        PACKET_SC_LOGIN_ACK,
        PACKET_CS_LOGOUT,
        PACKET_CS_CHAT,
        PACKET_SC_CHAT,
        PACKET_CS_MOVE,
        PACKET_SC_MOVE,
        PACKET_SC_VIEW
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PACKET_HEADER
    {
        [FieldOffset(0)] public PACKET_COMMAND command;
        [FieldOffset(4)] public uint size;
        public static byte[] Serialize(Object data)
        {
            try
            {
                int bufSize = Marshal.SizeOf(data);
                IntPtr unmanagedPointer = Marshal.AllocHGlobal(bufSize);
                Marshal.StructureToPtr(data, unmanagedPointer, false);
                byte[] managedArray = new byte[bufSize];
                Marshal.Copy(unmanagedPointer, managedArray, 0, bufSize);
                Marshal.FreeHGlobal(unmanagedPointer);
                
                /*
                var managedArray = new byte[((PACKET_HEADER)data).size];
                var gch = GCHandle.Alloc(managedArray, GCHandleType.Pinned);
                var pBuffer = gch.AddrOfPinnedObject();
                Marshal.StructureToPtr(data, pBuffer, false);
                gch.Free();
                */
                return managedArray;
            }
            catch (Exception e)
            {
                Console.WriteLine("Failed to serialize. Reason: " + e.Message);
                return null;
            }
        }
        public static Object Deserialize(byte[] data, Type dataType)
        {
            int classSize = Marshal.SizeOf(dataType);
            if (classSize > data.Length)
                return null;

            IntPtr buffer = Marshal.AllocHGlobal(classSize);

            Marshal.Copy(data, 0, buffer, classSize);
            object objData = Marshal.PtrToStructure(buffer, dataType);
            Marshal.FreeHGlobal(buffer);
            return objData;
        }
    };
    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_CS_LOGIN : PACKET_HEADER
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(0)] public char[] commanderID = new char[32];
    };
    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_SC_LOGIN_ACK : PACKET_HEADER
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(0)] public char[] commanderID = new char[32];
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_CS_LOGOUT: PACKET_HEADER
    {
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_CS_CHAT : PACKET_HEADER
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
        [FieldOffset(0)] public char[] chat = new char[128];
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_SC_CHAT : PACKET_HEADER
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
        [FieldOffset(0)] public char[] chat = new char[128];
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_CS_MOVE : PACKET_HEADER
    {
        [FieldOffset(0)] public Vector3 dest;
    };
    
    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_SC_MOVE : PACKET_HEADER
    {
        [FieldOffset(0)] public Vector3 dest;
    };

    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    public class PKS_SC_VIEW : PACKET_HEADER
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(0)] public char[] commanderID1 = new char[32];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(32)] public char[] commanderID2 = new char[32];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(64)] public char[] commanderID3 = new char[32];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(96)] public char[] commanderID4 = new char[32];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        [FieldOffset(128)] public char[] commanderID5 = new char[32];
        [FieldOffset(160)] public Vector3 dest1;
        [FieldOffset(172)] public Vector3 dest2;
        [FieldOffset(184)] public Vector3 dest3;
        [FieldOffset(196)] public Vector3 dest4;
        [FieldOffset(208)] public Vector3 dest5;
    };
    public class Client : INotifyPropertyChanged
    {
        public Client()
        {
            Random r = new Random();
            position.X = r.Next(0, 300);
            position.Y = r.Next(0, 300);
        }
        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
        private string name;
        private string recv;
        private string send;
        private string error;
        private string chat;
        private int chatcount = 0;
        private Vector3 position;
        private ClientViewList clientViewList;

        public ClientViewList ViewList
        {
            get { return clientViewList; }
            set
            {
                if (value == clientViewList)
                    return;
                clientViewList = value;
                OnPropertyChanged("ViewList");
            }
        }

        public string Name
        {
            get { return name; }
            set
            {
                if (value == name)
                    return;
                name = value;
                OnPropertyChanged("Name");
            }
        }

        public string RecvMessage
        {
            get { return recv; }
            set
            {
                if (value == recv)
                    return;
                recv = value;
                OnPropertyChanged("RecvMessage");
            }
        }

        public string SendMessage
        {
            get { return send; }
            set
            {
                if (value == send)
                    return;
                send = value;
                OnPropertyChanged("SendMessage");
            }
        }

        public string Error
        {
            get { return error; }
            set
            {
                if (value == error)
                    return;
                error = value;
                OnPropertyChanged("Error");
            }
        }

        public string Chat
        {
            get { return chat; }
            set
            {
                if (value == chat)
                    return;
                chat = value;
                OnPropertyChanged("Chat");
            }
        }
        public int ChatCount
        {
            get { return chatcount; }
            set
            {
                if (value == chatcount)
                    return;
                chatcount = value;
                OnPropertyChanged("ChatCount");
            }
        }

        public Vector3 Position
        {
            get { return position; }
            set
            {
                if (value == position)
                    return;
                position = value;
                OnPropertyChanged("Position");
                OnPropertyChanged("PositionX");
                OnPropertyChanged("PositionY");
            }
        }

        public float PositionX
        {
            get { return position.X; }
            set
            {
                if (value == position.X)
                    return;
                position.X = value;
                OnPropertyChanged("PositionX");
            }
        }

        public float PositionY
        {
            get { return position.Y; }
            set
            {
                if (value == position.Y)
                    return;
                position.Y = value;
                OnPropertyChanged("PositionY");
            }
        }
        private Socket socket;
        private byte[] data = new byte[2048];
        private int size = 2048;
        public object lockObject = new object();
        public void Connect(string _ID)
        {
            Socket newSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9001);
            newSocket.BeginConnect(ipep, new AsyncCallback(Connected), newSocket);
        }
        public void Send(PACKET_HEADER packet)
        {
            if (socket == null)
                return;
            try
            {
                this.SendMessage = packet.command.ToString();
                byte[] message = PACKET_HEADER.Serialize(packet);
                socket.BeginSend(message, 0, message.Length, SocketFlags.None, new AsyncCallback(SendData), socket);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                this.Error = "socket closed";
            }
        }
        private void Connected(IAsyncResult result)
        {
            socket = (Socket)result.AsyncState;
            try
            {
                socket.EndConnect(result);
                PKS_CS_LOGIN loginPacket = new PKS_CS_LOGIN();
                loginPacket.command = PACKET_COMMAND.PACKET_CS_LOGIN;
                loginPacket.size = (uint)Marshal.SizeOf<PKS_CS_LOGIN>(loginPacket);
                Array.Copy(Name.ToCharArray(), loginPacket.commanderID, Name.Length);
                Send(loginPacket);
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
                this.Error = "Connect Fail";
            }
        }
        private void ReceiveData(IAsyncResult result)
        {
            try
            {
                Socket remote = (Socket)result.AsyncState;
                int recvbyte = remote.EndReceive(result);
                if(recvbyte > 0)
                {
                    ProcessPacket();
                }
                remote.BeginReceive(data, 0, size, SocketFlags.None, new AsyncCallback(ReceiveData), remote);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                this.Error = "read Fail";
            }
            
        }
        private void ProcessPacket()
        {
            var header = (PACKET_HEADER)PACKET_HEADER.Deserialize(data, typeof(PACKET_HEADER));

            this.RecvMessage = header.command.ToString();
            switch (header.command)
            {
                case PACKET_COMMAND.PACKET_SC_LOGIN_ACK:
                    {
                        var packet = (PKS_SC_LOGIN_ACK)PACKET_HEADER.Deserialize(data, typeof(PKS_SC_LOGIN_ACK));
                    }
                    break;
                case PACKET_COMMAND.PACKET_SC_CHAT:
                    {
                        var packet = (PKS_SC_CHAT)PACKET_HEADER.Deserialize(data, typeof(PKS_SC_CHAT));
                        this.Chat = new string(packet.chat).Split('\0')[0];
                        this.ChatCount++;
                    }
                    break;
                case PACKET_COMMAND.PACKET_SC_MOVE:
                    {
                        var packet = (PKS_SC_MOVE)PACKET_HEADER.Deserialize(data, typeof(PKS_SC_MOVE));
                        this.Position = packet.dest;
                    }
                    break;
                case PACKET_COMMAND.PACKET_SC_VIEW:
                    {
                        var packet = (PKS_SC_VIEW)PACKET_HEADER.Deserialize(data, typeof(PKS_SC_VIEW));
                        lock (lockObject)
                        {
                            if (ViewList == null)
                                break;
                            foreach (var view in ViewList)
                            {
                                view.IsUpdate = false;
                            }
                            var name = new string(packet.commanderID1).Split('\0')[0];
                            if (name != "None")
                            {
                                bool isFind = false;
                                foreach (var view in ViewList)
                                {
                                    if (view.Name == name)
                                    {
                                        view.PositionX = packet.dest1.X;
                                        view.PositionY = packet.dest1.Y;
                                        view.IsUpdate = true;
                                        isFind = true;
                                        break;
                                    }
                                }
                                if (!isFind)
                                {
                                    var view = new ClientView(packet.dest1.X, packet.dest1.Y, name);
                                    ViewList.Add(view);
                                }
                            }


                            name = new string(packet.commanderID2).Split('\0')[0];
                            if (name != "None")
                            {
                                bool isFind = false;
                                foreach (var view in ViewList)
                                {
                                    if (view.Name == name)
                                    {
                                        view.PositionX = packet.dest2.X;
                                        view.PositionY = packet.dest2.Y;
                                        view.IsUpdate = true;
                                        isFind = true;
                                        break;
                                    }
                                }
                                if (!isFind)
                                {
                                    ViewList.Add(new ClientView(packet.dest2.X, packet.dest2.Y, name));
                                }
                            }
                            name = new string(packet.commanderID3).Split('\0')[0];
                            if (name != "None")
                            {
                                bool isFind = false;
                                foreach (var view in ViewList)
                                {
                                    if (view.Name == name)
                                    {
                                        view.PositionX = packet.dest3.X;
                                        view.PositionY = packet.dest3.Y;
                                        view.IsUpdate = true;
                                        isFind = true;
                                        break;
                                    }
                                }
                                if (!isFind)
                                {
                                    ViewList.Add(new ClientView(packet.dest3.X, packet.dest3.Y, name));
                                }
                            }

                            name = new string(packet.commanderID4).Split('\0')[0];
                            if (name != "None")
                            {
                                bool isFind = false;
                                foreach (var view in ViewList)
                                {
                                    if (view.Name == name)
                                    {
                                        view.PositionX = packet.dest4.X;
                                        view.PositionY = packet.dest4.Y;
                                        view.IsUpdate = true;
                                        isFind = true;
                                        break;
                                    }
                                }
                                if (!isFind)
                                {
                                    ViewList.Add(new ClientView(packet.dest4.X, packet.dest4.Y, name));
                                }
                            }

                            name = new string(packet.commanderID5).Split('\0')[0];
                            if (name != "None")
                            {
                                bool isFind = false;
                                foreach (var view in ViewList)
                                {
                                    if (view.Name == name)
                                    {
                                        view.PositionX = packet.dest5.X;
                                        view.PositionY = packet.dest5.Y;
                                        view.IsUpdate = true;
                                        isFind = true;
                                        break;
                                    }
                                }
                                if (!isFind)
                                {
                                    ViewList.Add(new ClientView(packet.dest5.X, packet.dest5.Y, name));
                                }
                            }
                            foreach (var view in ViewList.ToList())
                            {
                                if (!view.IsUpdate)
                                {
                                    ViewList.Remove(view);
                                }
                            }
                        }
                       
                        /*

                        if (new string(packet.commanderID2).Split('\0')[0] == "")
                        {
                            break;
                        }
                        clientViewList.Add(new ClientView(packet.dest2.X, packet.dest2.Y, new string(packet.commanderID2).Split('\0')[0]));


                        if (new string(packet.commanderID3).Split('\0')[0] == "")
                        {
                            break;
                        }
                        clientViewList.Add(new ClientView(packet.dest3.X, packet.dest3.Y, new string(packet.commanderID3).Split('\0')[0]));
                        */


                    }
                    break;

                default:
                    this.Error = "wrong packet command";
                    break;
            }
        }
        private void SendData(IAsyncResult iar)
        {
            try
            {
                Socket remote = (Socket)iar.AsyncState;
                int sent = remote.EndSend(iar);
                remote.BeginReceive(data, 0, size, SocketFlags.None, new AsyncCallback(ReceiveData), remote);
            }
            catch
            { }
        }
        public void Close()
        {
            socket.Shutdown(SocketShutdown.Both);
            socket.BeginDisconnect(true, new AsyncCallback(DisconnectCallback), socket);
        }

        private static void DisconnectCallback(IAsyncResult ar)
        {
            // Complete the disconnect request.
            Socket client = (Socket)ar.AsyncState;
            client.EndDisconnect(ar);
        }
    }
    
}
