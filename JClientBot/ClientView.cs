using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace JClientBot
{
    public class ClientViewList : ObservableCollection<ClientView> { }
    public class ClientView : INotifyPropertyChanged
    {
        public ClientView(float positionX, float positionY, string name)
        {
            PositionX = positionX;
            PositionY = positionY;
            IsUpdate = true;
            Name = name;
            OnPropertyChanged("PositionX");
        }
        private float x;
        private float y;
        public float PositionX
        {
            get { return x; }
            set
            {
                x = value;
                OnPropertyChanged("PositionX");
            }
        }
        public float PositionY
        {
            get { return y; }
            set
            {
                if (value == y)
                    return;
                y = value;
                OnPropertyChanged("PositionY");
            }
        }
        public string Name { get; set; }
        public bool IsUpdate { get; set; }
        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
