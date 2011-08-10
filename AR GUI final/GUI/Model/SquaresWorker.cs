using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace GUI.Model
{
    class SquaresWorker : IDisposable
    {
        private SquaresLibWrapper squaresLib;
        private BackgroundWorker backWorker;
        private System.Drawing.Bitmap image;
        private bool pause = true;
        IntPtr imgp;
       

        public SquaresWorker()
        {
            backWorker = new BackgroundWorker();
            backWorker.DoWork += AsyncExecution;
            backWorker.RunWorkerCompleted += SyncExecution;
            imgp = new IntPtr();
        }


        public SquaresLibWrapper SquaresLib
        {
            get { return squaresLib; }
        }

        public System.Drawing.Bitmap Image
        {
            get
            {
                if (squaresLib == null || squaresLib.Initialized == false || squaresLib.Started == false)
                    return image;
                lock (squaresLib)
                {
                    image = new System.Drawing.Bitmap(squaresLib.GetImageWidth(),
                    squaresLib.GetImageHeight(), squaresLib.GetImageStride(),
                    System.Drawing.Imaging.PixelFormat.Format24bppRgb, imgp);
                }
                return image;
            }
        }

        public bool Processing
        {
            get
            {
                if (squaresLib != null)
                    return squaresLib.Initialized;
                else 
                    return false;
            }
        }

        private void AsyncExecution(object sender, DoWorkEventArgs Ex)
        {
            try
            {
                using (squaresLib = new SquaresLibWrapper())
                {
                    while (pause == false)
                    {
                        lock (squaresLib)
                        {
                            squaresLib.Extract();
                            imgp = squaresLib.GetImage();
                        }
                    }
                }
            }
            catch (ArgumentException argEx)
            {
                Ex.Result = argEx;
            }
            catch (Exception eEx)
            {
                Ex.Result = eEx;
            }
        }

        private void SyncExecution(object sender, RunWorkerCompletedEventArgs Ex)
        {
            if (!pause)
                pause = true;
            if (Ex.Result != null)
            {
                object result = Ex.Result;
                Type type = result.GetType();
                if (type == typeof(Exception))
                {
                    throw (Exception)result;
                }
            }
        }

        public void StartExtraction()
        {
            pause = false;
            backWorker.RunWorkerAsync();
        }

        public void StopExtraction()
        {
            pause = true;
            while (this.Processing)
            {
                Thread.Sleep(100);
            }
        }

        public void Dispose()
        {
            this.StopExtraction();
            if (squaresLib != null)
            {
                if (squaresLib.Initialized)
                    squaresLib.Dispose();
            }
            GC.SuppressFinalize(this);
        }
    }
}
