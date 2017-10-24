using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Ookii.Dialogs.Wpf;

namespace PluginSdkWizardInstaller {
    public partial class FolderInputWindow : Window {
        public FolderInputWindow(string title) {
            InitializeComponent();
            Title = title;
            lblSelectFolder.Content = title + ":";
        }

        private void btnOK_Click(object sender, RoutedEventArgs e) {
            DialogResult = true;
        }

        private void btnBrowse_Click(object sender, RoutedEventArgs e) {
            VistaFolderBrowserDialog dialog = new VistaFolderBrowserDialog();
            dialog.ShowNewFolderButton = true;
            dialog.Description = Title;
            dialog.UseDescriptionForTitle = true;
            if (dialog.ShowDialog() == true)
                txbFolder.Text = dialog.SelectedPath;
        }

        private void txbFolder_TextChanged(object sender, TextChangedEventArgs e) {
            TextBox txb = sender as TextBox;
            btnOK.IsEnabled = !String.IsNullOrEmpty(txb.Text);
        }
    }
}
