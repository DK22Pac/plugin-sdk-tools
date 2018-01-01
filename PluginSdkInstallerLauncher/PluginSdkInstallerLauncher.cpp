#include <Windows.h>
#include <experimental\filesystem>

using namespace std;
using namespace std::experimental::filesystem;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    path installerPath = current_path() / "tools" / "installer" / "PluginSdkWizardInstaller.exe";
    if (!ShellExecuteA(NULL, "runas", installerPath.string().c_str(), NULL, NULL, SW_SHOWNORMAL)) {
        std::string errMessage = "Unable to launch '" + installerPath.string() + "'";
        MessageBox(NULL, errMessage.c_str(), "Plugin-SDK Installer Launcher - Error", 0);
    }
    return 0;
}
