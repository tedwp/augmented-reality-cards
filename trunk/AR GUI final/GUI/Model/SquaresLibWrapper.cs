using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

namespace GUI.Model
{
    class SquaresLibWrapper : IDisposable
    {
        private bool initialized;
        private bool started = false;

        public bool Initialized
        {
            get { return initialized; }
        }

        public bool Started
        {
            get { return started; }
        }

        #region Imports


        // Library Dependencies
        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Initialize();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool Finalize();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Main();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GetImg();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Width();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Height();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Stride();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetImg(string p);

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetVid(string p);

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetMark(string p);

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void TogMode();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void TogVideoPause();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void RewVideo();

        [DllImport(@"SquaresLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern double GetFPS();

        #endregion 

        #region Public Methods

        public SquaresLibWrapper()
        {
            if (initialized == true)
                return;
            initialized = false;
            try
            {
                if (Initialize()== 1)
                {
                    throw new Exception("Could not initialize camera capture. Please check if camera is connected properly.");
                }
                if (Initialize() == 2)
                {
                    throw new Exception("Could not load default images/videos. Pattern files were not found.");
                }
            }
            catch (DllNotFoundException dllEx)
            {
                throw new Exception("Critical Error. SquaresLib.dll was not found.", dllEx);
            }
            initialized = true;
        }

        public IntPtr GetImage()
        {
            return GetImg();
        }

        public int GetImageWidth()
        {
            if (started)
                return Width();
            return 0;
        }

        public void SetImage(string path)
        {
            SetImg(path); 
        }

        public void SetVideo(string path)
        {
            SetVid(path);
        }

        public void SetMarker(string path)
        {
            SetMark(path);
        }

        public double GetFramesPerSecond()
        {
            return GetFPS();
        }

        public void ToggleMode()
        {
            TogMode();
        }

        public void ToggleVideoPause()
        {
            TogVideoPause();
        }

        public void RewindVideo()
        {
            RewVideo();
        }

        public int GetImageHeight()
        {
            if (started)
                return Height();
            return 0;
        }

        public int GetImageStride()
        {
            if (started)
                return Stride();
            return 0;
        }

        public void Extract()
        {
            if (initialized)
            {
                Main();
                started = true;
            }
            else
            {
                throw new Exception("SquaresLib was not initialized.");
            }
        }

        public void Dispose()
        {
            if (initialized == false)
                return;
            if (!Finalize())
            {
                throw new Exception("Critical Error. Unable to finalize SquaresLib.");
            }
            initialized = false;
            GC.SuppressFinalize(this);
        }

        #endregion 
    }
}
