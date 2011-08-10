using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;


namespace GUI.Model
{
    static class Singleton
    {
        static SquaresClient client;
        public static void Init()
        {
            client = new SquaresClient();
            client.Start();
        }
        public static SquaresClient Client
        {
            get { return client; }
        }
    }

    class SquareArgs : System.EventArgs
    {
        BitmapImage img;

        public BitmapImage bitmapImg
        {
            get { return img; }
            set { img = value; }
        }
    }
}
