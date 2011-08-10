using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.IO;
using System.Windows.Media.Imaging;

namespace GUI.Model
{
    class SquaresClient
    {
        #region Event

        // delegate declaration 
        public delegate void ChangingHandler(object sender, SquareArgs sArgs);

        // event declaration 
        public event ChangingHandler Change;

        #endregion // Event

        #region Fields

        SquaresWorker sWorker;
        DispatcherTimer timer;
        System.Drawing.Bitmap bitmap;
        bool paused = true;
        string path;

        #endregion // Fields

        #region Properties

        public BitmapImage Image
        {
            get
            {
                if (bitmap != null)
                {
                    MemoryStream ms = new MemoryStream();
                    bitmap.Save(ms, System.Drawing.Imaging.ImageFormat.Png);
                    ms.Position = 0;
                    BitmapImage img = new BitmapImage();
                    img.BeginInit();
                    img.StreamSource = ms;
                    img.EndInit();
                    return img;
                }

                return null;
            }
        }

        public string ImgPath
        {
            get { return path; }
            set
            {
                if (File.Exists(value))
                {
                    path = value;
                    sWorker.SquaresLib.SetImage(path);
                }
            }
        }

        public string VidPath
        {
            get { return path; }
            set
            {
                if (File.Exists(value))
                {
                    path = value;
                    sWorker.SquaresLib.SetVideo(path);
                }
            }
        }

        public string MarkPath
        {
            get { return path; }
            set
            {
                if (File.Exists(value))
                {
                    path = value;
                    sWorker.SquaresLib.SetMarker(path);
                }
            }
        }

        public double GetFPS()
        {
            if (sWorker.Processing == true)
                return sWorker.SquaresLib.GetFramesPerSecond();
            else
                return 0;
        }

        public void ToggleMode()
        {
            sWorker.SquaresLib.ToggleMode();
        }

        public void ToggleVideoPause()
        {
            sWorker.SquaresLib.ToggleVideoPause();
        }

        public void RewindVideo()
        {
            sWorker.SquaresLib.RewindVideo();
        }

        #endregion // Properties

        #region Contructors

        public SquaresClient()
        {
            sWorker = new SquaresWorker();
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromMilliseconds(15);
            timer.Tick += new EventHandler(tick);
        }

        #endregion // Contructors

        #region Public Methods

        public void Start()
        {
            if (paused == true)
            {
                sWorker.StartExtraction();
                timer.Start();
                paused = false;
            }
        }

        public void Stop()
        {
            timer.Stop();
            sWorker.StopExtraction();
        }

        #endregion // Public Methods

        #region Private Helpers

        private void tick(object sender, EventArgs eArgs)
        {
            bitmap = sWorker.Image;
            SquareArgs bitmapImage = new SquareArgs() { bitmapImg = this.Image };
            Change(this, bitmapImage);
        }

        #endregion // Private Helpers
    }
}
