using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.IO;
using System.Diagnostics;
using System.Security;
using Ookii.Dialogs.Wpf;
using System.ComponentModel;

namespace PluginSdkWizardInstaller {
    public partial class MainWindow : Window {
        private BitmapImage iconError;
        private BitmapImage iconOk;
        private BitmapImage iconNothing;
        private BitmapImage iconNotSet;

        enum DevEnv { NONE = -1, VS2019, VS2017, VS2015, CODEBLOCKS };

        private string[] varIdents =
        {
            "SDK",
            "SA", "VC", "III",
            "DX9", "RWD3D9",
            "CLEOSA", "CLEOVC", "CLEOIII",
            "MOONSDK"
        };

        private class PathConfigRow
        {
            public TextBox tbx;
            public Button aut;
            public Button set;
        };
        private PathConfigRow[] pathConfigs;

        public MainWindow() {
            InitializeComponent();
            iconError = GetIcon("error.png");
            iconOk = GetIcon("ok.png");
            iconNothing = GetIcon("nothing.png");
            iconNotSet = GetIcon("notset.png");
            pathConfigs = new PathConfigRow[varIdents.Length];
            int n = 0;
            foreach ( string ident in varIdents )
            {
                PathConfigRow row = new PathConfigRow();
                row.tbx = FindName( "tbx" + ident ) as TextBox;
                row.aut = FindName( "aut" + ident ) as Button;
                row.set = FindName( "set" + ident ) as Button;
                pathConfigs[ n++ ] = row;
            }
            foreach ( PathConfigRow row in pathConfigs )
            {
                SetTextBoxTextToOsVariable(row.tbx);
            }
        }

        private bool CompareLowerCase(string strA, string strB) {
            return strA.ToLower() == strB.ToLower();
        }

        private string WithoutTrailingSlash(string src) {
            if (src.EndsWith("\\") || src.EndsWith("/"))
                return src.Remove(src.Length - 1);
            return src;
        }

        private BitmapImage GetIcon(string iconName) {
            return new BitmapImage(new Uri("/Icons/" + iconName, UriKind.RelativeOrAbsolute));
        }

        private object GetElement(object baseElement, string name) {
            FrameworkElement frameworkElement = baseElement as FrameworkElement;
            FrameworkElement parent = frameworkElement.Parent as FrameworkElement;
            string newName = name + frameworkElement.Name.Substring(3);
            return parent.FindName(newName);
        }

        private void SetTextBoxTextToOsVariable(TextBox tbx) {
            bool isSdk = tbx.Name == "tbxSDK";
            Button setBtn = GetElement(tbx, "set") as Button;
            Image img = GetElement(tbx, "img") as Image;
            string envVar = PathLogic.GetOsVariable(setBtn.Tag.ToString());
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
            Button vsBtn = sender as Button;
            if (vsBtn.Name == "btnVs2019")
                cmbGenerateSlnFor.SelectedIndex = (int)DevEnv.VS2019;
            else if (vsBtn.Name == "btnVs2017")
                cmbGenerateSlnFor.SelectedIndex = (int)DevEnv.VS2017;
            else if (vsBtn.Name == "btnVs2015")
                cmbGenerateSlnFor.SelectedIndex = (int)DevEnv.VS2015;
            string sdkDir = GetPluginSdkDir();
            if (sdkDir != "") {
                string vsixPath = Path.Combine(sdkDir, "tools\\general\\PluginSdkVsTools.vsix");
                if (File.Exists(vsixPath))
                    Process.Start(vsixPath);
                else
                    MessageBox.Show(String.Format("Can't find '{0}'", vsixPath));
            }
            
        }

        public static void CopyAll(DirectoryInfo source, DirectoryInfo target) {
            Directory.CreateDirectory(target.FullName);
            foreach (FileInfo fi in source.GetFiles())
                fi.CopyTo(Path.Combine(target.FullName, fi.Name), true);
            foreach (DirectoryInfo diSourceSubDir in source.GetDirectories()) {
                DirectoryInfo nextTargetSubDir =
                    target.CreateSubdirectory(diSourceSubDir.Name);
                CopyAll(diSourceSubDir, nextTargetSubDir);
            }
        }

        static private string GetPluginSdkDir()
        {
            string sdkDir = PathLogic.GetOsVariable("PLUGIN_SDK_DIR");
            if (sdkDir == "")
                MessageBox.Show("Can't find plugin-sdk directory (PLUGIN_SDK_DIR)");
            return sdkDir;
        }

        private void installCB_Click(object sender, RoutedEventArgs e) {
            cmbGenerateSlnFor.SelectedIndex = (int)DevEnv.CODEBLOCKS;
            FolderInputWindow dlg = new FolderInputWindow("Select Code::Blocks folder");
            dlg.Owner = this;
            dlg.ShowDialog();
            if (dlg.DialogResult == true) {
                string sdkDir = GetPluginSdkDir();
                if (sdkDir != "") {
                    string installerPath = Path.Combine(sdkDir, "tools\\general\\code-blocks-wizard-installer.exe");
                    if (File.Exists(installerPath)) {
                        ProcessStartInfo info = new ProcessStartInfo(installerPath);
                        info.Arguments = '"' + WithoutTrailingSlash(dlg.txbFolder.Text) + '"' + ' ' + '"' + sdkDir + '"';
                        info.UseShellExecute = false;
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

        private void brwBtn_Click(object sender, RoutedEventArgs e) {
            Button btn = sender as Button;
            TextBox tbx = GetElement(sender, "tbx") as TextBox;
            Label lbl = GetElement(sender, "lbl") as Label;
            VistaFolderBrowserDialog dialog = new VistaFolderBrowserDialog();
            dialog.ShowNewFolderButton = true;
            dialog.Description = String.Format("Select location: {0}", lbl.Content);
            dialog.UseDescriptionForTitle = true;
            if (dialog.ShowDialog() == true && !String.IsNullOrEmpty(dialog.SelectedPath))
                tbx.Text = dialog.SelectedPath;
        }

        private static EnvironmentVariableTarget GetEnvVarRecommendedTarget( string envVarName )
        {
            // We want to target the highest place at which the environment variable is already set at.

            // Try system level.
            {
                string sysValue = Environment.GetEnvironmentVariable( envVarName, EnvironmentVariableTarget.Machine );

                if ( sysValue != null )
                {
                    return EnvironmentVariableTarget.Machine;
                }
            }

            // We just put it at the user level.
            return EnvironmentVariableTarget.User;
        }

        private void setBtn_Click(object sender, RoutedEventArgs e) {
            Button btn = sender as Button;
            TextBox tbx = GetElement(sender, "tbx") as TextBox;
            Image img = GetElement(sender, "img") as Image;
            string envVarName = btn.Tag.ToString();
            EnvironmentVariableTarget envTarget = GetEnvVarRecommendedTarget(envVarName);
            try
            {
                Environment.SetEnvironmentVariable(envVarName, WithoutTrailingSlash(tbx.Text), envTarget);
            }
            catch( SecurityException )
            {
                MessageBox.Show( "Failed to set system env var \"" + envVarName + "\" (requires admin rights)" );
                return;
            }
            tbx.Tag = tbx.Text;
            img.Source = iconOk;
            btn.IsEnabled = false;
            if (tbx.Name == "tbxSDK") {
                grdWizTmplButtons.IsEnabled = true;
                grdGenerateSolution.IsEnabled = true;
            }
        }

        private void UnsetEnvVarAndControls(TextBox tbx) {
            Button setBtn = GetElement(tbx, "set") as Button;
            Image errImg = GetElement(tbx, "img") as Image;
            string envVarName = setBtn.Tag.ToString();
            EnvironmentVariableTarget envTarget = GetEnvVarRecommendedTarget(envVarName);
            try
            {
                Environment.SetEnvironmentVariable(envVarName, null, envTarget);
            }
            catch( SecurityException )
            {
                MessageBox.Show("Failed to set system env var \"" + envVarName + "\" (requires admin rights)" );
                return;
            }
            tbx.Text = "";
            tbx.Tag = "";
            setBtn.IsEnabled = false;
            if (tbx.Name == "tbxSDK") {
                errImg.Source = iconError;
                grdWizTmplButtons.IsEnabled = false;
                grdGenerateSolution.IsEnabled = false;
            }
            else
                errImg.Source = iconNothing;
        }

        private void btnUnsetAll_Click(object sender, RoutedEventArgs e) {
            foreach ( PathConfigRow row in pathConfigs )
            {
                UnsetEnvVarAndControls(row.tbx);
            }
        }

        private void ClickSetButtonIfEnabled(Button btn) {
            if (btn.IsEnabled)
                setBtn_Click(btn, null);
        }

        private void btnSetAll_Click(object sender, RoutedEventArgs e) {
            foreach ( PathConfigRow row in pathConfigs )
            {
                ClickSetButtonIfEnabled(row.set);
            }
        }

        private void chkSlnWinXpSupport_Changed(object sender, RoutedEventArgs e)
        {
            CheckBox chk = sender as CheckBox;
            if (chk.IsChecked == true &&
                (cmbGenerateSlnFor.SelectedIndex == (int)DevEnv.VS2015
                || cmbGenerateSlnFor.SelectedIndex == (int)DevEnv.VS2017
                || cmbGenerateSlnFor.SelectedIndex == (int)DevEnv.VS2019))
            {
                lblSlnNote.Content = "Note: v14" + (cmbGenerateSlnFor.SelectedIndex == (int)DevEnv.VS2015 ? "0" : "1") + "_xp toolset is required";
                lblSlnNote.Visibility = Visibility.Visible;
            }
            else
            {
                lblSlnNote.Content = "";
                lblSlnNote.Visibility = Visibility.Hidden;
            }
        }

        private void cmbGenerateSlnFor_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cmb = sender as ComboBox;
            btnGenerateSln.IsEnabled = cmb.SelectedIndex != (int)DevEnv.NONE;
            if (cmb.SelectedIndex == (int)DevEnv.VS2015 || cmb.SelectedIndex == (int)DevEnv.VS2017 || cmb.SelectedIndex == (int)DevEnv.VS2019)
            {
                chkSlnWinXpSupport.IsEnabled = true;
                if (chkSlnWinXpSupport.IsChecked == true)
                {
                    lblSlnNote.Content = "Note: v14" + (cmbGenerateSlnFor.SelectedIndex == (int)DevEnv.VS2015 ? "0" : "1") + "_xp toolset is required";
                    lblSlnNote.Visibility = Visibility.Visible;
                }
                else
                    chkSlnWinXpSupport.IsChecked = true;
            }
            else
            {
                chkSlnWinXpSupport.IsChecked = cmb.SelectedIndex == (int)DevEnv.CODEBLOCKS;
                chkSlnWinXpSupport.IsEnabled = false;
                lblSlnNote.Content = "";
                lblSlnNote.Visibility = Visibility.Hidden;
            }
        }

        private void btnGenerateSln_Click(object sender, RoutedEventArgs e) {
            string pluginSdkDir = GetPluginSdkDir();
            if (String.IsNullOrEmpty(pluginSdkDir))
                return;
            string premakeExePath = Path.Combine(pluginSdkDir, "tools\\premake\\premake5.exe");
            if (!File.Exists(premakeExePath)) {
                MessageBox.Show(String.Format("Can't find premake tool ('{0}')", premakeExePath));
                return;
            }
            string premakeScriptPath = Path.Combine(pluginSdkDir, "tools\\premake\\premake5.lua");
            if (!File.Exists(premakeScriptPath)) {
                MessageBox.Show(String.Format("Can't find premake script ('{0}')", premakeScriptPath));
                return;
            }
            ProcessStartInfo info = new ProcessStartInfo(premakeExePath);
            switch (cmbGenerateSlnFor.SelectedIndex) {
                case (int)DevEnv.VS2019:
                    info.Arguments = "vs2019";
                    break;
                case (int)DevEnv.VS2017:
                    info.Arguments = "vs2017";
                    break;
                case (int)DevEnv.VS2015:
                    info.Arguments = "vs2015";
                    break;
                case (int)DevEnv.CODEBLOCKS:
                    info.Arguments = "codeblocks";
                    break;
            }
            if (chkSlnWinXpSupport.IsChecked == true)
                info.Arguments += " --winxp";
            info.Arguments += " --file=" + '"' + premakeScriptPath + '"' + " --pluginsdkdir=" + '"' + pluginSdkDir + '"';
            info.UseShellExecute = false;
            info.Verb = "runas";
            try {
                Process.Start(info);
            }
            catch (Win32Exception) {
            }
        }

        private void btnBuildSln_Click(object sender, RoutedEventArgs e)
        {
            //TODO: build logic to find Visual Studio installation folder; then find environment shell script;
            // then execute msbuild.
        }

        static private string FindDirByVariableName(string varName)
        {
            switch( varName )
            {
            case "PLUGIN_SDK_DIR":        return SDKFolders.FindPluginSdkDir();
            case "DIRECTX9_SDK_DIR":      return SDKFolders.FindDirectX9SdkDir();
            case "RWD3D9_DIR":            return SDKFolders.FindRWD3D9SdkDir();
            case "MOONLOADER_SDK_SA_DIR": return SDKFolders.FindMoonLoaderSdkDir();
            case "GTA_SA_DIR":            return PathLogic.ScanGTASAGameDirectory();
            case "GTA_VC_DIR":            return PathLogic.ScanGTAVCGameDirectory();
            case "GTA_III_DIR":           return PathLogic.ScanGTA3GameDirectory();
            }

            return null;
        }

        private void autBtn_Click(object sender, RoutedEventArgs e)
        {
            Button btn = sender as Button;
            Button setBtn = GetElement(sender, "set") as Button;
            string envVarName = setBtn.Tag.ToString();
            string pathToVariable = FindDirByVariableName(envVarName);

            if (pathToVariable != null)
            {
                TextBox tbx = GetElement(sender, "tbx") as TextBox;
                tbx.Text = pathToVariable;
            }
        }

        private void btnAutoDetectAll_Click(object sender, RoutedEventArgs e)
        {
            //TODO: CLEO_SDK variables, MOONLOADER SDK variable.

            foreach ( PathConfigRow row in pathConfigs )
            {
                if (row.aut.IsEnabled)
                {
                    // Perform click.
                    autBtn_Click(row.aut, e);
                }
            }
        }
    }
}
