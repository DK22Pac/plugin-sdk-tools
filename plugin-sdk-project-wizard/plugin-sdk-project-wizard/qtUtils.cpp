#include "qtUtils.h"
#include "ErrorMessage.h"
#include <QDir>

bool CopyRecursively(QString sourceFolder, QString destFolder)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if (!sourceDir.exists()) {
        return MESSAGE_ERROR("Can't copy folder \"" + sourceFolder + "\" to \"" + destFolder + "\": folder does not exist");
    }

    QDir destDir(destFolder);
    if (!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);
    for (int i = 0; i< files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        if (QFile::exists(destName))
        {
            if (!QFile::remove(destName))
                MESSAGE_ERROR("Can't remove file (\"" + destName + "\")");
        }
        if (!QFile::copy(srcName, destName))
            MESSAGE_ERROR("Can't copy file (\"" + destName + "\")");
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = CopyRecursively(srcName, destName);
        if (!success)
            return false;
    }

    return true;
}
