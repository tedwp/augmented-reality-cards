using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using GUI.Model;
using Microsoft.Win32;

namespace GUI.ViewModel
{
    enum BrowseOption
    {
        VidOption, ImgOption, MarkOption
    }

    class CommandViewModel : WorkspaceViewModel
    {
        BitmapImage image;
        RelayCommand browseImgCmd;
        RelayCommand browseVidCmd;
        RelayCommand browseMarkCmd;
        RelayCommand toggleModeCmd;
        RelayCommand toggleVidPauseCmd;
        RelayCommand rewindVidCmd;
        string fps;

        public CommandViewModel()
        {
            image = new BitmapImage();
            Singleton.Client.Change += new SquaresClient.ChangingHandler(change);
        }

        public BitmapImage Image
        {
            get { return image; }
            set
            {
                image = value;

                base.OnPropertyChanged("Image");
            }
        }

        public string GetFPS
        {
            get
            {
                return getFPS().ToString("0.00");
            }
            set
            {
                base.OnPropertyChanged("GetFPS");
            }
        }

        public ICommand BrowseImg
        {
            get
            {
                if (browseImgCmd == null)
                    browseImgCmd = new RelayCommand(param => this.browsePath(BrowseOption.ImgOption));
                return browseImgCmd;
            }
        }

        public ICommand BrowseVid
        {
            get
            {
                if (browseVidCmd == null)
                    browseVidCmd = new RelayCommand(param => this.browsePath(BrowseOption.VidOption));
                return browseVidCmd;
            }
        }

        public ICommand BrowseMark
        {
            get
            {
                if (browseMarkCmd == null)
                    browseMarkCmd = new RelayCommand(param => this.browsePath(BrowseOption.MarkOption));
                return browseMarkCmd;
            }
        }

        public ICommand ToggleMode
        {
            get
            {
                if (toggleModeCmd == null)
                    toggleModeCmd = new RelayCommand(param => this.toggleMode());
                return toggleModeCmd;
            }
        }

        public ICommand ToggleVideoPause
        {
            get
            {
                if (toggleVidPauseCmd == null)
                    toggleVidPauseCmd = new RelayCommand(param => this.toggleVideoPause());
                return toggleVidPauseCmd;
            }
        }

        public ICommand RewindVideo
        {
            get
            {
                if (rewindVidCmd == null)
                    rewindVidCmd = new RelayCommand(param => this.rewindVideo());
                return rewindVidCmd;
            }
        }

        public void change(object sender, SquareArgs sArg)
        {
            this.Image = sArg.bitmapImg;
            this.GetFPS = string.Empty;
        }

        private double getFPS()
        {
            return Singleton.Client.GetFPS();
        }

        private void toggleMode()
        {
            Singleton.Client.ToggleMode();
        }

        private void toggleVideoPause()
        {
            Singleton.Client.ToggleVideoPause();
        }

        private void rewindVideo()
        {
            Singleton.Client.RewindVideo();
        }

        private string browsePath(BrowseOption option)
        {
            string filter = String.Empty;
           
             OpenFileDialog ofd = new OpenFileDialog();
            switch (option)
            {
                case BrowseOption.ImgOption:
                    filter = "JPG Files|*.jpg| PNG Files|*.png|BMP Files|*.bmp";
                    ofd.Filter = filter;
                    if (ofd.ShowDialog() == true)
                        Singleton.Client.ImgPath = ofd.FileName;  
                    break;
                case BrowseOption.VidOption:
                    filter = "AVI Files|*.avi|MPG Files|*.mpg|WMV Files|*.wmv|MOV Files|*.mov";
                    ofd.Filter = filter;
                    if (ofd.ShowDialog() == true)
                        Singleton.Client.VidPath = ofd.FileName;
                    break;
                case BrowseOption.MarkOption:
                    filter = "JPG Files|*.jpg| PNG Files|*.png|BMP Files|*.bmp";
                    ofd.Filter = filter;
                    if (ofd.ShowDialog() == true)
                        Singleton.Client.MarkPath = ofd.FileName;
                    break;
            }
            return null;
        }

    }
}
