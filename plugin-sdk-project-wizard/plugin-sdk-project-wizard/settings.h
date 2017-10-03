#pragma once
#include <QSettings>

class AppSettings {
public:
    QString pluginSdkFolder;
    QString directX9SdkFolder;
    QString rwd3d9Folder;
    QString vsDocumentsFolder;
    QString codeBlocksFolder;

    QString saAsiOutputFolder;
    QString saCleoOutputFolder;
    QString saCleoSdkFolder;

    QString vcAsiOutputFolder;
    QString vcCleoOutputFolder;
    QString vcCleoSdkFolder;

    QString iiiAsiOutputFolder;
    QString iiiCleoOutputFolder;
    QString iiiCleoSdkFolder;

    void read(QSettings &settingsFile);
    void write(QSettings &settingsFile);
};