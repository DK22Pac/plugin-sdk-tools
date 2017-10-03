#include "CodeBlocksWizardInstaller.h"
#include "qtUtils.h"
#include "ErrorMessage.h"
#include <QFile>
#include <QTextStream>

bool CodeBlocksWizardInstaller::Install(QString codeBlocksDir, QString wizardDir)
{
    // copy wizard folder to codeblocks wizards folder
    CopyRecursively(wizardDir, codeBlocksDir + "share\\CodeBlocks\\templates\\wizard\\pluginsdk");
    // register wizard
    QFile file(codeBlocksDir + "share\\CodeBlocks\\templates\\wizard\\config.script");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QStringList fileLines;
        bool registerFunctionFound = false;
        bool inRegisterFunction = false;
        bool pluginSdkWizardInstalled = false;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!registerFunctionFound && line.startsWith("function RegisterWizards()")) {
                registerFunctionFound = true;
                inRegisterFunction = true;
            }
            else if (inRegisterFunction) {
                if (line.startsWith("}")) {
                    inRegisterFunction = false;
                    fileLines.append("\n    RegisterWizard(wizProject,     _T(\"pluginsdk\"),    _T(\"Plugin-SDK project\"),    _T(\"Plugin-SDK\"));\n");
                }
                else if (line.contains("RegisterWizard") && line.contains("pluginsdk") && line.contains("Plugin-SDK project")) {
                    pluginSdkWizardInstalled = true;
                    break;
                }
            }
            fileLines.append(line + "\n");
        }
        file.close();
        if (!pluginSdkWizardInstalled) {
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (auto &line : fileLines)
                    out << line;
                file.close();
            }
            else
                MESSAGE_ERROR("Unable to install Code::Blocks Wizard:\ncan't open config.script file for writing");
        }
    }
    else
        MESSAGE_ERROR("Unable to install Code::Blocks Wizard:\ncan't open config.script file for reading");
    return false;
}
