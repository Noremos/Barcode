using System;
using System.Collections.Generic;
using System.Text;

namespace BarUI.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        public string Greeting => "Welcome to Avalonia!";

        public string[] MyItems => new string[2] { "1", "2" };
    }
}
