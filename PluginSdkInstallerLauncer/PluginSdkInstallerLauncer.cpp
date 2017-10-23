#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    memset(&startupInfo, 0, sizeof(startupInfo));
    memset(&processInfo, 0, sizeof(processInfo));
    if (!CreateProcessA("tools/installer/PluginSdkWizardInstaller.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
        MessageBox(NULL, "Unable to launch 'tools/installer/PluginSdkWizardInstaller.exe'", "Plugin-SDK Installer Launcher - Error", 0);
    return 0;
}