using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Windows;
using System.Security.Principal;

namespace PluginSdkWizardInstaller {
    public partial class App : Application {
        protected override void OnStartup(StartupEventArgs e) {
            base.OnStartup(e);
            bool isAdmin = false;
            try {
                WindowsIdentity user = WindowsIdentity.GetCurrent();
                WindowsPrincipal principal = new WindowsPrincipal(user);
                isAdmin = principal.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch (UnauthorizedAccessException) {
                isAdmin = false;
            }
            catch (Exception) {
                isAdmin = false;
            }
            if (!isAdmin) {
                MessageBox.Show("This application requires admin rights to run");
                Shutdown();
            }
        }
    }
}
