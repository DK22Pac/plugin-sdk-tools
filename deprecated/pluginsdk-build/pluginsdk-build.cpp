#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

using namespace std;
using namespace experimental::filesystem;

#define _PLUGIN_SDK_BUILD_VER_ "1.1.2"

int main(int argc, char *argv[]) {
    cout << ">>> Plugin-SDK Build Tool v." << _PLUGIN_SDK_BUILD_VER_ << " <<<" << endl;

    enum BuildToolError {
        ERROR_NOT_ENOUGH_PARAMETERS = 1,
        ERROR_UNKNOWN_BUILD_COMMAND = 2,
        ERROR_BUILD_TYPE_NOT_SET = 3,
        ERROR_UNKNOWN_BUILD_TYPE = 4,
        ERROR_PROJECT_DIR_NOT_SET = 5,
        ERROR_PROJECT_NAME_NOT_SET = 6,
        ERROR_TARGET_NAME_NOT_SET = 7,
        ERROR_INT_DIR_NOT_SET = 8,
        ERROR_CANT_OPEN_PROJECT_FILE = 9,
        ERROR_CANT_OPEN_INT_DIR = 10,
        ERROR_CANT_COMPILE_FILE = 11,
        ERROR_CANT_OPEN_OUTPUT_DIRECTORY = 12,
        ERROR_FAILED_TO_LINK_OBJECTS = 13
    };

    if (argc < 2) {
        cout << "Error: not enought parameters" << endl;
        return ERROR_NOT_ENOUGH_PARAMETERS;
    }

    enum BuildCommand {
        COMMAND_BUILD,
        COMMAND_REBUILD,
        COMMAND_CLEAN
    } buildCommand;

    string buildCommandTypeStr = argv[1];

    if (buildCommandTypeStr == "build")
        buildCommand = COMMAND_BUILD;
    else if (buildCommandTypeStr == "rebuild")
        buildCommand = COMMAND_REBUILD;
    else if (buildCommandTypeStr == "clean")
        buildCommand = COMMAND_CLEAN;
    else {
        cout << "Error: unknown build command" << endl;
        return ERROR_UNKNOWN_BUILD_COMMAND;
    }

    struct BuildParameters {
        bool fulllog = false;
        string buildtype;
        string projectdir;
        string projectname;
        string targetname;
        string outdir;
        string intdir;
        vector<string> definitions;
        vector<string> includeDirs;
        vector<string> libraryDirs;
        vector<string> libraries;
        string additional;
        string linkadditional;
    } parameters;

    auto readOneParameter = [](string const &arg) {
        if (arg.length() < 2 || arg.front() != '(' || arg.back() != ')')
            cout << "Warning: incorrect parameter " << arg << endl;
        string line = arg.substr(1, arg.length() - 2);
        size_t comma = line.find_first_of(';');
        if (comma != string::npos)
            return line.substr(0, comma);
        return line;
    };

    auto readParameters = [](vector<string> &vec, string const &arg) {
        if (arg.length() < 2 || arg.front() != '(' || arg.back() != ')')
            cout << "Warning: incorrect parameter " << arg << endl;
        string line = arg.substr(1, arg.length() - 2);
        size_t currPos = 0;
        size_t comma = line.find_first_of(';');
        while (comma != string::npos) {
            vec.push_back(line.substr(currPos, comma - currPos));
            currPos = comma + 1;
            comma = line.find_first_of(';', currPos);
        }
        vec.push_back(line.substr(currPos));
    };

    auto isCleanMode = [=]() {
        return buildCommand == COMMAND_CLEAN;
    };

    auto isBuildMode = [=]() {
        return buildCommand == COMMAND_BUILD;
    };

    auto isRebuildMode = [=]() {
        return buildCommand == COMMAND_REBUILD;
    };

    auto isBuildOrRebuildMode = [=]() {
        return buildCommand == COMMAND_BUILD || buildCommand == COMMAND_REBUILD;
    };

    auto executeCommand = [](string const &file, string const &command) {
        STARTUPINFO startInf;
        memset(&startInf, 0, sizeof(STARTUPINFO));
        startInf.cb = sizeof(STARTUPINFO);
        PROCESS_INFORMATION procInf;
        memset(&procInf, 0, sizeof(PROCESS_INFORMATION));
        string cmdline = file + " " + command;
        BOOL b = CreateProcess(NULL, const_cast<char *>(cmdline.c_str()), NULL, NULL, FALSE,
            0, NULL, NULL, &startInf, &procInf);
        int commandResult = 0;
        if (b) {
            WaitForSingleObject(procInf.hProcess, INFINITE);
            GetExitCodeProcess(procInf.hProcess, reinterpret_cast<DWORD *>(&commandResult));
            CloseHandle(procInf.hProcess);
            CloseHandle(procInf.hThread);
        }
        else
            commandResult = GetLastError();
        return commandResult;
    };

    for (int i = 2; i < argc; i++) {

        // read parameters and fill BuildParameters structure

        string strarg = argv[i];
        if (!strarg.compare(0, 10, "buildtype:"))
            parameters.buildtype = readOneParameter(strarg.substr(10));
        else if (!strarg.compare(0, 11, "projectdir:"))
            parameters.projectdir = readOneParameter(strarg.substr(11));
        else if (!strarg.compare(0, 12, "projectname:"))
            parameters.projectname = readOneParameter(strarg.substr(12));
        else if (!strarg.compare(0, 11, "targetname:"))
            parameters.targetname = readOneParameter(strarg.substr(11));
        else if (!strarg.compare(0, 7, "outdir:"))
            parameters.outdir = readOneParameter(strarg.substr(7));
        else if (!strarg.compare(0, 7, "intdir:"))
            parameters.intdir = readOneParameter(strarg.substr(7));
        else if (!strarg.compare(0, 12, "definitions:")) {
            readParameters(parameters.definitions, strarg.substr(12));
            for (auto &def : parameters.definitions) {
                std::replace(def.begin(), def.end(), '<', '\\');
                std::replace(def.begin(), def.end(), '>', '"');
            }
        }
        else if (!strarg.compare(0, 12, "includeDirs:"))
            readParameters(parameters.includeDirs, strarg.substr(12));
        else if (!strarg.compare(0, 12, "libraryDirs:"))
            readParameters(parameters.libraryDirs, strarg.substr(12));
        else if (!strarg.compare(0, 10, "libraries:"))
            readParameters(parameters.libraries, strarg.substr(10));
        else if (!strarg.compare(0, 11, "additional:"))
            parameters.additional = readOneParameter(strarg.substr(11));
        else if (!strarg.compare(0, 15, "linkadditional:"))
            parameters.linkadditional = readOneParameter(strarg.substr(15));
        else if (!strarg.compare(0, 7, "fulllog"))
            parameters.fulllog = true;
    }

    // check if all required parameters are set

    if (isBuildOrRebuildMode() && parameters.buildtype.empty()) {
        cout << "Error: buildtype is not set" << endl;
        return ERROR_BUILD_TYPE_NOT_SET;
    }

    if (isBuildOrRebuildMode() && parameters.buildtype.compare("DLL") && parameters.buildtype.compare("LIB")) {
        cout << "Error: unknown buildtype" << endl;
        return ERROR_UNKNOWN_BUILD_TYPE;
    }

    if (parameters.projectdir.empty()) {
        cout << "Error: projectdir is not set" << endl;
        return ERROR_PROJECT_DIR_NOT_SET;
    }

    if (parameters.projectname.empty()) {
        cout << "Error: projectname is not set" << endl;
        return ERROR_PROJECT_NAME_NOT_SET;
    }

    if (parameters.targetname.empty()) {
        cout << "Error: targetname is not set" << endl;
        return ERROR_TARGET_NAME_NOT_SET;
    }

    if (parameters.outdir.empty() || (parameters.outdir.length() == 1 && (parameters.outdir[0] == '\\' || parameters.outdir[0] == '/'))) {
        parameters.outdir = parameters.projectdir;
    }

    if (parameters.intdir.empty()) {
        cout << "Error: intdir is not set" << endl;
        return ERROR_INT_DIR_NOT_SET;
    }

    // everything ok, now parse .vcxproj file

    vector<string> cppFiles;

    ifstream file(parameters.projectdir + parameters.projectname + ".vcxproj");
    if (file.is_open()) {
        for (string line; getline(file, line); ) {
            size_t lineBegin = line.find_first_not_of(" \t");
            if (lineBegin != string::npos) {
                if (!line.compare(lineBegin, 20, "<ClCompile Include=\"")) {
                    size_t pathEnd = line.find_first_of('"', lineBegin + 20);
                    if (pathEnd != string::npos) {
                        string cppFileProjPath = line.substr(lineBegin + 20, pathEnd - 20 - lineBegin);
                        cppFiles.push_back(cppFileProjPath);
                    }
                }
            }
        }
    }
    else {
        cout << "Error: can't open project file" << " " << parameters.projectdir << endl;
        return ERROR_CANT_OPEN_PROJECT_FILE;
    }

    // check for clean

    if (isCleanMode()) {

        // clean and return

        // delete object files

        for (auto const &cppFile : cppFiles) {
            path cppFilePath = canonical(cppFile, parameters.projectdir);
            path objFilePath = path(canonical(parameters.intdir) / cppFilePath.filename()).replace_extension("o");
            if (exists(objFilePath)) {
                remove(objFilePath);
                if (parameters.fulllog)
                    cout << "Deleting: " << objFilePath << endl;
            }
        }

        // delete output file

        path outputPath = canonical(parameters.outdir) / parameters.targetname;
        if (exists(outputPath)) {
            remove(outputPath);
            if (parameters.fulllog)
                cout << "Deleting: " << outputPath << endl;
        }

        return 0;
    }

    if (cppFiles.size() == 0) {
        cout << "Warning: no files for compilation" << endl;
        return 0;
    }

    // start compilation - generate object files

    // re-create object directory

    path objectsPath = path(parameters.intdir) / parameters.projectname;
    if (exists(objectsPath)) {
        std::vector<path> oldObjectFiles;
        for (const auto& p : directory_iterator(objectsPath)) {
            if (p.path().extension() == ".o")
                oldObjectFiles.push_back(p.path());
        }
        for (const auto& p : oldObjectFiles)
            remove(p);
    }
    else {
        create_directories(objectsPath);

        // validate object directory
        if (!exists(objectsPath)) {
            cout << "Error: can't open intdir directory" << endl;
            return ERROR_CANT_OPEN_INT_DIR;
        }
    }

    cout << "Compiling..." << endl;

    for (auto const &cppFile : cppFiles) {
        stringstream command;
        // relative path -> canonical path
        path cppFilePath = canonical(cppFile, parameters.projectdir);
        // create object path
        path objFilePath = path(canonical(parameters.intdir) / parameters.projectname / cppFilePath.filename()).replace_extension("o");

        if (parameters.fulllog) {
            cout << "Compiling: relative:  " << cppFile << endl;
            cout << "Compiling: canonical: " << cppFilePath << endl;
            cout << "Compiling: object:    " << objFilePath << endl;
        }

        // additional commands
        if (!parameters.additional.empty())
            command << parameters.additional << " ";
        // defines
        for (auto const &def : parameters.definitions)
            command << "-D" << '"' << def << '"' << " ";
        // include dirs
        for (auto const &includeDir : parameters.includeDirs)
            command << "-I" << '"' << canonical(includeDir) << '"' << " ";
        // compile
        command << "-c " << '"' << cppFilePath << '"' << " ";
        // output
        command << "-o " << '"' << objFilePath << '"';

        if (parameters.fulllog)
            cout << "Compiling: " << cppFilePath.filename() << " : " << command.str() << endl;
        else
            cout << cppFilePath.filename() << endl;

        if (executeCommand("g++", command.str().c_str()) != 0) {
            cout << "Error: can't compile " << cppFilePath.filename() << " (" << cppFilePath << ")" << endl;
            return ERROR_CANT_COMPILE_FILE;
        }
    }

    // generate library

    // create output directories, if they are not exist
    if (!exists(parameters.outdir)) {
        create_directories(parameters.outdir);
        // validate output directory
        if (!exists(parameters.outdir)) {
            cout << "Error: can't open outdir directory" << endl;
            return ERROR_CANT_OPEN_OUTPUT_DIRECTORY;
        }
    }

    cout << "Linking..." << endl;

    {
        stringstream command;
        if (!parameters.buildtype.compare("LIB"))
            command << "-r -s ";
        else
            command << "-shared -Wl,--dll ";
        if (!parameters.linkadditional.empty())
            command << parameters.linkadditional << " ";
        if (parameters.buildtype.compare("LIB"))
            command << "-o ";
        path outputPath = canonical(parameters.outdir) / parameters.targetname;
        command << '"' << outputPath << '"' << " ";
        // print all object files
        for (auto const &f : cppFiles) {
            path filePath = f;
            path objFileName = filePath.stem().string() + ".o";
            path lnkObjPath = canonical(parameters.intdir) / parameters.projectname / objFileName;
            command << '"' << lnkObjPath << '"' << " ";
        }
        // library dirs
        for (auto const &libraryDir : parameters.libraryDirs)
            command << "-L" << '"' << canonical(libraryDir) << '"' << " ";
        // libraries
        for (auto const &library : parameters.libraries)
            command << "-l" << '"' << library << '"' << " ";

        if (parameters.fulllog)
            cout << "Linking: " << command.str() << endl;

        if (executeCommand(!parameters.buildtype.compare("LIB") ? "ar" : "g++", command.str().c_str()) != 0) {
            cout << "Error: failed to link objects (command: " << command.str() << ")" << endl;
            return ERROR_FAILED_TO_LINK_OBJECTS;
        }
        else {
            static char sizestr[32];
            sprintf_s(sizestr, "%.2f KB", static_cast<float>(file_size(outputPath))/1024.0f);
            cout << parameters.projectname << ".vcxproj -> " << outputPath << " (" << sizestr << ")" << endl;
        }
    }

    return 0;
}