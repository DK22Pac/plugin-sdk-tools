using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System;
using System.IO;

namespace PluginSdkWizard {
    public partial class WizardMainWindow : Microsoft.VisualStudio.PlatformUI.DialogWindow {
        public string safeProjectName;
        public bool finished;
        private int lastSelectedProjectType;

        public WizardMainWindow(string projectName) {
            finished = false;
            safeProjectName = projectName;
            InitializeComponent();

            cmbProjectType.Items.Add(Properties.Resources.strTypeAsi);
            cmbProjectType.Items.Add(Properties.Resources.strTypeCleo);
            cmbProjectType.Items.Add(Properties.Resources.strTypeMoonLoader);
            cmbProjectType.Items.Add(Properties.Resources.strTypeDll);
            cmbProjectType.SelectedIndex = 0;

            try
            {
                string sdkDir = Environment.GetEnvironmentVariable("PLUGIN_SDK_DIR");
                if (!string.IsNullOrEmpty(sdkDir))
                {
                    string projectFilePath = Path.Combine(sdkDir, @"plugin_sa\plugin_sa.vcxproj");
                    if (File.Exists(projectFilePath))
                    {
                        string fileContent = File.ReadAllText(projectFilePath);
                        if (fileContent.Contains("_xp</PlatformToolset>"))
                            cbWinXpSupport.IsChecked = true;
                    }
                }
            }
            catch (Exception) { }

            // Navigation buttons

            btnPrev.Click += new RoutedEventHandler(btnPrev_Click);
            btnNext.Click += new RoutedEventHandler(btnNext_Click);
            btnFinish.Click += new RoutedEventHandler(btnFinish_Click);

            // Page 1 - Basic setup

            cbSA.Checked += new RoutedEventHandler(cbSA_Checked);
            cbSA.Unchecked += new RoutedEventHandler(cbSA_Unchecked);
            cbVC.Checked += new RoutedEventHandler(cbVC_Checked);
            cbVC.Unchecked += new RoutedEventHandler(cbVC_Unchecked);
            cbIII.Checked += new RoutedEventHandler(cbIII_Checked);
            cbIII.Unchecked += new RoutedEventHandler(cbIII_Unchecked);
            cmbProjectType.SelectionChanged += new SelectionChangedEventHandler(cmbProjectType_SelectionChanged);
            cbUseDirectXSdk.Checked += new RoutedEventHandler(cbUseDirectXSdk_Checked);
            cbUseDirectXSdk.Unchecked += new RoutedEventHandler(cbUseDirectXSdk_Unchecked);
            btnOutputGameDir.Click += new RoutedEventHandler(btnOutputGameDir_Click);
            btnOutputProjectDir.Click += new RoutedEventHandler(btnOutputProjectDir_Click);
            btnBrowseOutputFolder.Click += new RoutedEventHandler(btnBrowseOutputFolder_Click);
            tbOutputFolder.TextChanged += new TextChangedEventHandler(tbOutputFolder_TextChanged);
            tbTargetName.TextChanged += new TextChangedEventHandler(tbTargetName_TextChanged);
            tbBaseClassName.TextChanged += new TextChangedEventHandler(tbBaseClassName_TextChanged);

            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbBaseClassName, errBaseClassName, true, false, false, false);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);

            // Page 2 - Release configuration settings

            btnRelAddAdditionalIncDir.Click += new RoutedEventHandler(btnRelAddAdditionalIncDir_Click);
            btnRelAddAdditionalLibDir.Click += new RoutedEventHandler(btnRelAddAdditionalLibDir_Click);
            tbRelAdditionalIncDirs.TextChanged += new TextChangedEventHandler(tbRelAdditionalIncDirs_TextChanged);
            tbRelAdditionalLibDirs.TextChanged += new TextChangedEventHandler(tbRelAdditionalLibDirs_TextChanged);
            tbRelAdditionalLibs.TextChanged += new TextChangedEventHandler(tbRelAdditionalLibs_TextChanged);
            tbRelAdditionalDefs.TextChanged += new TextChangedEventHandler(tbRelAdditionalDefs_TextChanged);

            // Page 3 - Debug configuration settings

            cbDbgConfigSameAsRel.Checked += new RoutedEventHandler(cbDbgConfigSameAsRel_Checked);
            cbDbgConfigSameAsRel.Unchecked += new RoutedEventHandler(cbDbgConfigSameAsRel_Unchecked);
            btnDbgAddAdditionalIncDir.Click += new RoutedEventHandler(btnDbgAddAdditionalIncDir_Click);
            btnDbgAddAdditionalLibDir.Click += new RoutedEventHandler(btnDbgAddAdditionalLibDir_Click);

            // Page 4 - Setup paths

            btnBrowsePluginSdkDir.Click += new RoutedEventHandler(btnBrowsePluginSdkDir_Click);
            btnBrowseSaDir.Click += new RoutedEventHandler(btnBrowseSaDir_Click);
            btnBrowseSaCleoSdkDir.Click += new RoutedEventHandler(btnBrowseSaCleoSdkDir_Click);
            btnBrowseVcDir.Click += new RoutedEventHandler(btnBrowseVcDir_Click);
            btnBrowseVcCleoSdkDir.Click += new RoutedEventHandler(btnBrowseVcCleoSdkDir_Click);
            btnBrowseIIIDir.Click += new RoutedEventHandler(btnBrowseIIIDir_Click);
            btnBrowseIIICleoSdkDir.Click += new RoutedEventHandler(btnBrowseIIICleoSdkDir_Click);
            btnBrowseMoonLoaderSdkDir.Click += new RoutedEventHandler(btnBrowseMoonLoaderSdkDir_Click);
            btnBrowseDirectX9SdkDir.Click += new RoutedEventHandler(btnBrowseDirectX9SdkDir_Click);
            btnBrowseRwD3d9Dir.Click += new RoutedEventHandler(btnBrowseRwD3d9Dir_Click);

            btnUseEnvVars.Click += new RoutedEventHandler(btnUseEnvVars_Click);

            tbPluginSdkDir.TextChanged += new TextChangedEventHandler(tbPluginSdkDir_TextChanged);
            tbSaDir.TextChanged += new TextChangedEventHandler(tbSaDir_TextChanged);
            tbSaCleoSdkDir.TextChanged += new TextChangedEventHandler(tbSaCleoSdkDir_TextChanged);
            tbVcDir.TextChanged += new TextChangedEventHandler(tbVcDir_TextChanged);
            tbVcCleoSdkDir.TextChanged += new TextChangedEventHandler(tbVcCleoSdkDir_TextChanged);
            tbIIIDir.TextChanged += new TextChangedEventHandler(tbIIIDir_TextChanged);
            tbIIICleoSdkDir.TextChanged += new TextChangedEventHandler(tbIIICleoSdkDir_TextChanged);
            tbMoonLoaderSdkDir.TextChanged += new TextChangedEventHandler(tbMoonLoaderSdkDir_TextChanged);
            tbDirectX9SdkDir.TextChanged += new TextChangedEventHandler(tbDirectX9SdkDir_TextChanged);
            tbRwD3d9Dir.TextChanged += new TextChangedEventHandler(tbRwD3d9Dir_TextChanged);

            UpdatePathTextBoxState(tbPluginSdkDir, errPluginSdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbSaDir, errSaDir, false, false, false, false);
            UpdatePathTextBoxState(tbSaCleoSdkDir, errSaCleoSdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbVcDir, errVcDir, false, false, false, false);
            UpdatePathTextBoxState(tbVcCleoSdkDir, errVcCleoSdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbIIIDir, errIIIDir, false, false, false, false);
            UpdatePathTextBoxState(tbIIICleoSdkDir, errIIICleoSdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbMoonLoaderSdkDir, errMoonLoaderSdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbDirectX9SdkDir, errDirectX9SdkDir, false, false, false, false);
            UpdatePathTextBoxState(tbRwD3d9Dir, errRwD3d9Dir, false, false, false, false);
        }

        public void SetCurrentPage(int pageId) {
            btnPrev.IsEnabled = pageId != 0;
            btnNext.IsEnabled = pageId != tcTabs.Items.Count - 1;
            tcTabs.SelectedIndex = pageId;
        }

        public int GetCurrentPage() {
            return tcTabs.SelectedIndex;
        }

        private void btnNext_Click(object sender, RoutedEventArgs e) {
            SetCurrentPage(GetCurrentPage() + 1);
        }

        private void btnPrev_Click(object sender, RoutedEventArgs e) {
            SetCurrentPage(GetCurrentPage() - 1);
        }

        private void btnFinish_Click(object sender, RoutedEventArgs e) {
            DialogResult = true;
            finished = true;
            Close();
        }

        public string GetDirectoryViaBrowseDlg(string strDlgTitle, string strInitialDir) {
            IntPtr windowHandle = new WindowInteropHelper(this).Handle;
            return VsUtility.BrowseForDirectory(windowHandle, strInitialDir, strDlgTitle);
        }

        public bool IsAnyGameSelected() {
            return cbSA.IsChecked == true || cbVC.IsChecked == true || cbIII.IsChecked == true;
        }

        public bool IsSaSelected() {
            return cbSA.IsChecked == true;
        }

        public bool IsVcSelected() {
            return cbVC.IsChecked == true;
        }

        public bool IsIIISelected() {
            return cbIII.IsChecked == true;
        }

        public bool IsVcOrIIISelected() {
            return cbVC.IsChecked == true || cbIII.IsChecked == true;
        }

        public int GetNumGamesSelected() {
            int count = 0;
            if (IsSaSelected())
                count += 1;
            if (IsVcSelected())
                count += 1;
            if (IsIIISelected())
                count += 1;
            return count;
        }

        private void EnableNextFinishButtons(bool enable) {
            btnNext.IsEnabled = enable;
            btnFinish.IsEnabled = enable;
        }

        // Page 1

        private void cbSA_Checked(object sender, RoutedEventArgs e) {
            EnableNextFinishButtons(true);
            grdBasicOptions.IsEnabled = true;
            dpVersionsSA.IsEnabled = true;
            UpdatePathsState();
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }

        private void cbSA_Unchecked(object sender, RoutedEventArgs e) {
            if (!IsAnyGameSelected()) {
                EnableNextFinishButtons(false);
                grdBasicOptions.IsEnabled = false;
            }
            dpVersionsSA.IsEnabled = false;
            UpdatePathsState();
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }

        private void cbVC_Checked(object sender, RoutedEventArgs e) {
            if (cmbProjectType.SelectedIndex == 2) { // MoonLoader Module
                if (!IsSaSelected()) {
                    cmbProjectType.SelectedIndex = 0; // ASI Plugin
                    lastSelectedProjectType = 0; // ASI Plugin
                }
                else {
                    cbVC.IsChecked = false;
                    MessageBox.Show(Properties.Resources.strMoonLoaderWarn3, Properties.Resources.strWarning);
                }
            }
            if (cbVC.IsChecked == true) {
                EnableNextFinishButtons(true);
                grdBasicOptions.IsEnabled = true;
                dpVersionsVC.IsEnabled = true;
                UpdatePathsState();
                UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
                UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
            }
        }

        private void cbVC_Unchecked(object sender, RoutedEventArgs e) {
            if (!IsAnyGameSelected()) {
                EnableNextFinishButtons(false);
                grdBasicOptions.IsEnabled = false;
            }
            dpVersionsVC.IsEnabled = false;
            UpdatePathsState();
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }

        private void cbIII_Checked(object sender, RoutedEventArgs e) {
            if (cmbProjectType.SelectedIndex == 2) { // MoonLoad Module
                if (!IsSaSelected()) {
                    cmbProjectType.SelectedIndex = 0; // ASI Plugin
                    lastSelectedProjectType = 0; // ASI Plugin
                }
                else {
                    cbIII.IsChecked = false;
                    MessageBox.Show(Properties.Resources.strMoonLoaderWarn2, Properties.Resources.strWarning);
                }
            }
            if (cbIII.IsChecked == true) {
                EnableNextFinishButtons(true);
                grdBasicOptions.IsEnabled = true;
                dpVersionsIII.IsEnabled = true;
                UpdatePathsState();
                UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
                UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
            }
        }

        private void cbIII_Unchecked(object sender, RoutedEventArgs e) {
            if (!IsAnyGameSelected()) {
                EnableNextFinishButtons(false);
                grdBasicOptions.IsEnabled = false;
            }
            dpVersionsIII.IsEnabled = false;
            UpdatePathsState();
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }

        private void cbUseDirectXSdk_Checked(object sender, RoutedEventArgs e) {
            UpdatePathsState();
        }

        private void cbUseDirectXSdk_Unchecked(object sender, RoutedEventArgs e) {
            UpdatePathsState();
        }

        private void btnBrowseOutputFolder_Click(object sender, RoutedEventArgs e) {
            string folder = GetDirectoryViaBrowseDlg(Properties.Resources.strSelectFolder, tbOutputFolder.Text);
            if (folder != null && folder != "")
                tbOutputFolder.Text = folder;
        }

        private void btnOutputProjectDir_Click(object sender, RoutedEventArgs e) {
            tbOutputFolder.Text = "$solutiondir$\\output";
        }

        private void btnOutputGameDir_Click(object sender, RoutedEventArgs e) {
            tbOutputFolder.Text = "$gamedir$\\$outdirname$";
        }

        private void tbOutputFolder_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathsState();
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }

        private void tbTargetName_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
        }

        private void tbBaseClassName_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbBaseClassName, errBaseClassName, true, false, false, false);
        }

        private void cmbProjectType_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            if (cmbProjectType.SelectedIndex == 2 && !IsSaSelected()) {
                cmbProjectType.SelectedIndex = lastSelectedProjectType;
                MessageBox.Show(Properties.Resources.strMoonLoaderWarn1, Properties.Resources.strWarning);
            }
            else if (cmbProjectType.SelectedIndex == 2 && IsVcOrIIISelected()) {
                cmbProjectType.SelectedIndex = lastSelectedProjectType;
                string errMessage;
                if (IsIIISelected() && IsVcSelected())
                    errMessage = Properties.Resources.strMoonLoaderWarn4;
                else if (IsIIISelected())
                    errMessage = Properties.Resources.strMoonLoaderWarn2;
                else
                    errMessage = Properties.Resources.strMoonLoaderWarn3;
                MessageBox.Show(errMessage, Properties.Resources.strWarning);
            }
            else {
                lastSelectedProjectType = cmbProjectType.SelectedIndex;
                UpdatePathsState();
                UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
                // + Additional
            }
        }

        // Page 2

        private void tbRelAdditionalIncDirs_TextChanged(object sender, TextChangedEventArgs e) {
            if (cbDbgConfigSameAsRel.IsChecked == true)
                tbDbgAdditionalIncDirs.Text = tbRelAdditionalIncDirs.Text;
        }

        private void tbRelAdditionalLibDirs_TextChanged(object sender, TextChangedEventArgs e) {
            if (cbDbgConfigSameAsRel.IsChecked == true)
                tbDbgAdditionalLibDirs.Text = tbRelAdditionalLibDirs.Text;
        }

        private void tbRelAdditionalLibs_TextChanged(object sender, TextChangedEventArgs e) {
            if (cbDbgConfigSameAsRel.IsChecked == true)
                tbDbgAdditionalLibs.Text = tbRelAdditionalLibs.Text;
        }

        private void tbRelAdditionalDefs_TextChanged(object sender, TextChangedEventArgs e) {
            if (cbDbgConfigSameAsRel.IsChecked == true)
                tbDbgAdditionalDefs.Text = tbRelAdditionalDefs.Text;
        }

        private void AddDirectoryClicked(TextBox tb) {
            string folder = GetDirectoryViaBrowseDlg(Properties.Resources.strSelectFolder, "");
            if (folder != null && folder != "") {
                if (!String.IsNullOrWhiteSpace(tb.Text)) {
                    if (tb.Text[tb.Text.Length - 1] != ';')
                        tb.Text += ";";
                    tb.Text += folder;
                }
                else
                    tb.Text = folder;
            }
        }

        private void btnRelAddAdditionalIncDir_Click(object sender, RoutedEventArgs e) {
            AddDirectoryClicked(tbRelAdditionalIncDirs);
        }

        private void btnRelAddAdditionalLibDir_Click(object sender, RoutedEventArgs e) {
            AddDirectoryClicked(tbRelAdditionalLibDirs);
        }

        // Page 3

        private void EnableDebugOptions(bool enable) {
            grdDebugOptions.IsEnabled = !enable;
            tbDbgAdditionalIncDirs.Text = tbRelAdditionalIncDirs.Text;
            tbDbgAdditionalLibDirs.Text = tbRelAdditionalLibDirs.Text;
            tbDbgAdditionalLibs.Text = tbRelAdditionalLibs.Text;
            tbDbgAdditionalDefs.Text = tbRelAdditionalDefs.Text;
        }

        private void cbDbgConfigSameAsRel_Checked(object sender, RoutedEventArgs e) {
            EnableDebugOptions(true);
        }

        private void cbDbgConfigSameAsRel_Unchecked(object sender, RoutedEventArgs e) {
            EnableDebugOptions(false);
        }

        private void btnDbgAddAdditionalIncDir_Click(object sender, RoutedEventArgs e) {
            AddDirectoryClicked(tbDbgAdditionalIncDirs);
        }

        private void btnDbgAddAdditionalLibDir_Click(object sender, RoutedEventArgs e) {
            AddDirectoryClicked(tbDbgAdditionalLibDirs);
        }

        // Page 4

        private void EnablePaths(bool enable, Label lbl, Grid grdTbErr, Button btn) {
            lbl.IsEnabled = enable;
            grdTbErr.IsEnabled = enable;
            btn.IsEnabled = enable;
        }

        private void UpdatePathsState() {
            bool requireOutputPaths = tbOutputFolder.Text.Contains("$gamedir$");
            EnablePaths(cbUseDirectXSdk.IsChecked == true, lblDirectX9SdkDir, grdDirectX9SdkDir, btnBrowseDirectX9SdkDir);
            EnablePaths(cbUseDirectXSdk.IsChecked == true && IsVcOrIIISelected(), lblRwD3d9Dir, grdRwD3d9Dir, btnBrowseRwD3d9Dir);
            EnablePaths(cmbProjectType.SelectedIndex == 1 && IsSaSelected(), lblSaCleoSdkDir, grdSaCleoSdkDir, btnBrowseSaCleoSdkDir);
            EnablePaths(cmbProjectType.SelectedIndex == 1 && IsVcSelected(), lblVcCleoSdkDir, grdVcCleoSdkDir, btnBrowseVcCleoSdkDir);
            EnablePaths(cmbProjectType.SelectedIndex == 1 && IsIIISelected(), lblIIICleoSdkDir, grdIIICleoSdkDir, btnBrowseIIICleoSdkDir);
            EnablePaths(cmbProjectType.SelectedIndex == 2, lblMoonLoaderSdkDir, grdMoonLoaderSdkDir, btnBrowseMoonLoaderSdkDir);
            EnablePaths(requireOutputPaths && IsSaSelected(), lblSaDir, grdSaDir, btnBrowseSaDir);
            EnablePaths(requireOutputPaths && IsVcSelected(), lblVcDir, grdVcDir, btnBrowseVcDir);
            EnablePaths(requireOutputPaths && IsIIISelected(), lblIIIDir, grdIIIDir, btnBrowseIIIDir);
        }

        private void BrowsePathClicked(TextBox tb) {
            string folder = GetDirectoryViaBrowseDlg(Properties.Resources.strSelectFolder, tb.Text);
            if (folder != null && folder != "")
                tb.Text = folder;
        }

        private void btnBrowsePluginSdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbPluginSdkDir); }
        private void btnBrowseSaDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbSaDir); }
        private void btnBrowseVcDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbVcDir); }
        private void btnBrowseIIIDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbIIIDir); }
        private void btnBrowseSaCleoSdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbSaCleoSdkDir); }
        private void btnBrowseVcCleoSdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbVcCleoSdkDir); }
        private void btnBrowseIIICleoSdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbIIICleoSdkDir); }
        private void btnBrowseMoonLoaderSdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbMoonLoaderSdkDir); }
        private void btnBrowseDirectX9SdkDir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbDirectX9SdkDir); }
        private void btnBrowseRwD3d9Dir_Click(object sender, RoutedEventArgs e) { BrowsePathClicked(tbRwD3d9Dir); }

        private void btnUseEnvVars_Click(object sender, RoutedEventArgs e) {
            tbPluginSdkDir.Text = tbPluginSdkDir.Tag.ToString();
            tbSaDir.Text = tbSaDir.Tag.ToString();
            tbSaCleoSdkDir.Text = tbSaCleoSdkDir.Tag.ToString();
            tbVcDir.Text = tbVcDir.Tag.ToString();
            tbVcCleoSdkDir.Text = tbVcCleoSdkDir.Tag.ToString();
            tbIIIDir.Text = tbIIIDir.Tag.ToString();
            tbIIICleoSdkDir.Text = tbIIICleoSdkDir.Tag.ToString();
            tbMoonLoaderSdkDir.Text = tbMoonLoaderSdkDir.Tag.ToString();
            tbDirectX9SdkDir.Text = tbDirectX9SdkDir.Tag.ToString();
            tbRwD3d9Dir.Text = tbRwD3d9Dir.Tag.ToString();
        }

        private bool ResolveEnvVars(string inputText, ref string outputText, ref string errorsStr) {
            outputText = "";
            List<string> errors = VsUtility.ResolveEnvironmentVariables(inputText, ref outputText);
            if (errors.Count > 0) {
                foreach (string error in errors) {
                    VsUtility.AddNewLineIfNeeded(ref errorsStr);
                    errorsStr += error;
                }
                return false;
            }
            return true;
        }

        public string GetOutputDirName() {
            if (cmbProjectType.SelectedIndex == 0)
                return "scripts";
            if (cmbProjectType.SelectedIndex == 1)
                return "cleo";
            if (cmbProjectType.SelectedIndex == 2)
                return "moonloader\\lib";
            return "";
        }

        private void UpdatePathTextBoxState(TextBox tb, Image errImg, bool allowUserVars, bool isDir, bool isMultiGame, bool hasConfig) {
            string strToolTip = "";
            bool hasErrors = false;
            string errorsStr = "";
            List<KeyValuePair<string, string>> toolTips = new List<KeyValuePair<string, string>> { };
            string inputText = tb.Text;
            if (String.IsNullOrEmpty(tb.Text))
                inputText = tb.Tag.ToString();
            if (!String.IsNullOrEmpty(inputText)) {
                if (String.IsNullOrEmpty(tb.Text))
                    strToolTip = String.Format(Properties.Resources.strEmptyPathNote, tb.Tag.ToString());
                if (allowUserVars && isMultiGame) {
                    if (IsSaSelected()) {
                        string text = VsUtility.FormatName(inputText, "", safeProjectName, isDir ? "$(ProjectDir)" : "",
                            isDir ? "$(SolutionDir)" : "", "SA", isDir ? VsUtility.GetName(tbSaDir.Text, tbSaDir.Tag.ToString()) : "",
                            isDir ? GetOutputDirName() : "", hasConfig ? "Release" : "");
                        if (!ResolveEnvVars(text, ref text, ref errorsStr))
                            hasErrors = true;
                        toolTips.Add(new KeyValuePair<string, string>("GTA SA", text));
                    }
                    if (IsVcSelected()) {
                        string text = VsUtility.FormatName(inputText, "", safeProjectName, isDir ? "$(ProjectDir)" : "",
                            isDir ? "$(SolutionDir)" : "", "VC", isDir ? VsUtility.GetName(tbVcDir.Text, tbVcDir.Tag.ToString()) : "",
                            isDir ? GetOutputDirName() : "", hasConfig ? "Release" : "");
                        if (!ResolveEnvVars(text, ref text, ref errorsStr))
                            hasErrors = true;
                        toolTips.Add(new KeyValuePair<string, string>("GTA VC", text));
                    }
                    if (IsIIISelected()) {
                        string text = VsUtility.FormatName(inputText, "", safeProjectName, isDir ? "$(ProjectDir)" : "",
                            isDir ? "$(SolutionDir)" : "", "III", isDir ? VsUtility.GetName(tbIIIDir.Text, tbIIIDir.Tag.ToString()) : "",
                            isDir ? GetOutputDirName() : "", hasConfig ? "Release" : "");
                        if (!ResolveEnvVars(text, ref text, ref errorsStr))
                            hasErrors = true;
                        toolTips.Add(new KeyValuePair<string, string>("GTA 3", text));
                    }
                }
                else {
                    string text = inputText;
                    if (allowUserVars)
                        text = VsUtility.FormatName(inputText, "", safeProjectName, isDir ? "$(ProjectDir)" : "",
                            isDir ? "$(SolutionDir)" : "", "", "", "", hasConfig ? "Release" : "");
                    if (!ResolveEnvVars(text, ref text, ref errorsStr))
                        hasErrors = true;
                    toolTips.Add(new KeyValuePair<string, string>("", text));
                }
                foreach (var item in toolTips) {
                    if (!String.IsNullOrEmpty(item.Key) &&  GetNumGamesSelected() > 1) {
                        VsUtility.AddNewLineIfNeeded(ref strToolTip);
                        strToolTip += item.Key;
                    }
                    VsUtility.AddNewLineIfNeeded(ref strToolTip);
                    strToolTip += item.Value;
                }
            }
            tb.ToolTip = strToolTip;
            errImg.IsEnabled = hasErrors;
            if (hasErrors)
                errImg.ToolTip = errorsStr;
        } 

        private void tbPluginSdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbPluginSdkDir, errPluginSdkDir, false, false, false, false);
        }
        private void tbSaDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbSaDir, errSaDir, false, false, false, false);
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }
        private void tbSaCleoSdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbSaCleoSdkDir, errSaCleoSdkDir, false, false, false, false);
        }
        private void tbVcDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbVcDir, errVcDir, false, false, false, false);
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }
        private void tbVcCleoSdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbVcCleoSdkDir, errVcCleoSdkDir, false, false, false, false);
        }
        private void tbIIIDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbIIIDir, errIIIDir, false, false, false, false);
            UpdatePathTextBoxState(tbTargetName, errTargetName, true, false, true, true);
            UpdatePathTextBoxState(tbOutputFolder, errOutputFolder, true, true, true, true);
        }
        private void tbIIICleoSdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbIIICleoSdkDir, errIIICleoSdkDir, false, false, false, false);
        }
        private void tbMoonLoaderSdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbMoonLoaderSdkDir, errMoonLoaderSdkDir, false, false, false, false);
        }
        private void tbDirectX9SdkDir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbDirectX9SdkDir, errDirectX9SdkDir, false, false, false, false);
        }
        private void tbRwD3d9Dir_TextChanged(object sender, TextChangedEventArgs e) {
            UpdatePathTextBoxState(tbRwD3d9Dir, errRwD3d9Dir, false, false, false, false);
        }
    }
}
