using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Windows.Data;

namespace GUI.ViewModel
{
    class MainWindowViewModel : WorkspaceViewModel
    {
        #region Fields

        CommandViewModel _display;

        #endregion // Fields

        #region Constructor

        public MainWindowViewModel()
        {
            _display = new CommandViewModel();
        }

        #endregion // Constructor

        #region Workspaces

        public CommandViewModel Display
        {
            get
            {
                return _display;
            }
        }

        #endregion // Workspaces

        #region Private Helpers

        #endregion // Private Helpers
    }
}
