#include <filesystem>
#include <fstream>
#include "Utility.h"

using namespace std;
using namespace experimental::filesystem;

int main(int argc, char *argv[]) {
    if (argc < 2)
        return ErrorCode(1, "Error: Not enough parameters (%d, expected 3)", argc);
    path codeBlocksDir = argv[1]; // Code::Blocks folder
    path pluginSdkDir = argv[2]; // plugin-sdk folder;
    if (!is_directory(codeBlocksDir))
        return ErrorCode(1, "Error: Code::Blocks directory ('%s') does not exist", codeBlocksDir.string().c_str());
    path codeBlocksScriptFile = codeBlocksDir / "share" / "CodeBlocks" / "templates" / "wizard" / "config.script";
    if (!is_regular_file(codeBlocksScriptFile))
        return ErrorCode(1, "Error: Code::Blocks script file ('%s') does not exist", codeBlocksScriptFile.string().c_str());
    path wizardDir = pluginSdkDir / "tools" / "templates" / "codeblocks" / "pluginsdk";
    if (!is_directory(wizardDir))
        return ErrorCode(1, "Error: Wizard directory ('%s') does not exist", wizardDir.string().c_str());
    path codeBlocksPluginSdkDir = codeBlocksDir / "share" / "CodeBlocks" / "templates" / "wizard" / "pluginsdk";
    error_code ec;
    copy(wizardDir, codeBlocksPluginSdkDir, copy_options::recursive | copy_options::overwrite_existing, ec);
    if (ec)
        return ErrorCode(1, "Error: Failed to copy Wizard directory:\n%s", ec.message().c_str());
    ifstream inFile(codeBlocksScriptFile);
    bool pluginSdkWizardInstalled = false;
    if (inFile) {
        list<string> fileLines;
        bool registerFunctionFound = false;
        bool inRegisterFunction = false;
        for (string line; getline(inFile, line); ) {
            string funcBegin = "function RegisterWizards()";
            if (!registerFunctionFound && !line.compare(0, funcBegin.length(), funcBegin)) {
                registerFunctionFound = true;
                inRegisterFunction = true;
            }
            else if (inRegisterFunction) {
                if (line.length() > 0 && line[0] == '}') {
                    inRegisterFunction = false;
                    fileLines.push_back("\n    RegisterWizard(wizProject,     _T(\"pluginsdk\"),    _T(\"Plugin-SDK project\"),    _T(\"Plugin-SDK\"));\n");
                }
                else if (line.find("RegisterWizard")  != string::npos
                      && line.find("pluginsdk") != string::npos
                      && line.find("Plugin-SDK project") != string::npos)
                {
                    pluginSdkWizardInstalled = true;
                    break;
                }
            }
            fileLines.push_back(line + "\n");
        }
        inFile.close();
        if (!pluginSdkWizardInstalled) {
            ofstream outFile(codeBlocksScriptFile);
            if (outFile) {
                for (auto &line : fileLines)
                    outFile << line;
                outFile.close();
            }
            else
                return ErrorCode(1, "Error: Can't open config.script file for writing");
        }
    }
    else
        return ErrorCode(1, "Error: Can't open config.script file for reading");
    Message("Successfully %s Plugin-SDK Project Wizard for Code::Blocks", pluginSdkWizardInstalled? "updated" : "installed");
    return 0;
}