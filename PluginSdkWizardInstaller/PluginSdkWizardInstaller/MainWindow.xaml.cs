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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Diagnostics;
using Microsoft.WindowsAPICodePack.Dialogs;
using System.ComponentModel;

namespace PluginSdkWizardInstaller {
    public partial class MainWindow : Window {
        private BitmapImage iconError;
        private BitmapImage iconOk;
        private BitmapImage iconNothing;
        private BitmapImage iconNotSet;

        public MainWindow() {
            InitializeComponent();
            iconError = GetIcon("error.png");
            iconOk = GetIcon("ok.png");
            iconNothing = GetIcon("nothing.png");
            iconNotSet = GetIcon("notset.png");
            SetTextBoxTextToOsVariable(tbxSDK);
            SetTextBoxTextToOsVariable(tbxSA);
            SetTextBoxTextToOsVariable(tbxVC);
            SetTextBoxTextToOsVariable(tbxIII);
            SetTextBoxTextToOsVariable(tbxDX9);
            SetTextBoxTextToOsVariable(tbxRWD3D9);
            SetTextBoxTextToOsVariable(tbxCLEOSA);
            SetTextBoxTextToOsVariable(tbxCLEOVC);
            SetTextBoxTextToOsVariable(tbxCLEOIII);
            SetTextBoxTextToOsVariable(tbxMOONSDK);
        }

        private string GetPluginSdkDir() {
            string sdkDir = GetOsVariable("PLUGIN_SDK_DIR");
            if (sdkDir == "")
                MessageBox.Show("Can't find plugin-sdk directory (PLUGIN_SDK_DIR)");
            return sdkDir;
        }

        private bool CompareLowerCase(string strA, string strB) {
            return strA.ToLower() == strB.ToLower();
        }

        private BitmapImage GetIcon(string iconName) {
            return new BitmapImage(new Uri("pack://application:,,,/PluginSdkWizardInstaller;component/Icons/" + iconName, UriKind.Absolute));
        }

        private string GetOsVariable(string varName) {
            string envVar = Environment.GetEnvironmentVariable(varName);
            if (envVar != null)
                return envVar;
            return "";
        }

        private object GetElement(object baseElement, string name) {
            FrameworkElement frameworkElement = baseElement as FrameworkElement;
            FrameworkElement parent = frameworkElement.Parent as FrameworkElement;
            string newName = name + frameworkElement.Name.Substring(3);
            return parent.FindName(newName);
        }

        private void SetTextBoxTextToOsVariable(TextBox tbx) {
            bool isSdk = tbx.Name == "tbxSDK";
            Image img = GetElement(tbx, "img") as Image;
            string envVar = GetOsVariable(tbx.Tag.ToString());
            tbx.Tag = envVar;
            tbx.Text = envVar;
            if (envVar == "") {
                if (isSdk)
                    img.Source = iconError;
                else
                    img.Source = iconNothing;
            }
            else {
                if (isSdk) {
                    grdWizTmplButtons.IsEnabled = true;
                    grdGenerateSolution.IsEnabled = true;
                }
                img.Source = iconOk;
            }

        }

        private void tbx_TextChanged(object sender, TextChangedEventArgs e) {
            TextBox tbx = sender as TextBox;
            Button setButton = GetElement(sender, "set") as Button;
            Image img = GetElement(sender, "img") as Image;
            string envVar = tbx.Tag.ToString();
            if (!String.IsNullOrEmpty(tbx.Text) && !CompareLowerCase(tbx.Text, envVar)) {
                setButton.IsEnabled = true;
                img.Source = iconNotSet;
            }
            else {
                setButton.IsEnabled = false;
                if (envVar != "")
                    img.Source = iconOk;
                else {
                    if (tbx.Name == "tbxSDK")
                        img.Source = iconError;
                    else
                        img.Source = iconNothing;
                }
            }
        }

        private void inf_Click(object sender, RoutedEventArgs e) {
            Button inf = sender as Button;
            Process.Start(inf.Tag.ToString());
        }

        private void installVsWizard_Click(object sender, RoutedEventArgs e) {
            string sdkDir = GetPluginSdkDir();
            if (sdkDir != "") {
                string vsixPath = sdkDir + "\\tools\\general\\PluginSdkVsTools.vsix";
                if (File.Exists(vsixPath))
                    Process.Start(vsixPath);
                else
                    MessageBox.Show(String.Format("Can't find '{0}'", vsixPath));
            }
        }

        private void installCB_Click(object sender, RoutedEventArgs e) {
            FolderInputWindow dlg = new FolderInputWindow("Select Code::Blocks folder");
            dlg.Owner = this;
            dlg.ShowDialog();
            if (dlg.DialogResult == true) {
                string sdkDir = GetPluginSdkDir();
                if (sdkDir != "") {
                    string installerPath = sdkDir + "\\tools\\general\\code-blocks-wizard-installer.exe";
                    if (File.Exists(installerPath)) {
                        ProcessStartInfo info = new ProcessStartInfo(installerPath);
                        info.Arguments = "\"" + dlg.txbFolder.Text + "\"";
                        info.UseShellExecute = true;
                        info.Verb = "runas";
                        try {
                            Process.Start(info);
                        }
                        catch (Win32Exception) {
                        }
                    }
                    else
                        MessageBox.Show(String.Format("Can't find '{0}'", installerPath));
                }
            }
        }
    }
}
