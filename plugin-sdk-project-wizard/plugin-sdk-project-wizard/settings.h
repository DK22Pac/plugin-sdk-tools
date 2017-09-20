#pragma once
#include <QSettings>

class AppSettings {
public:
    QString pluginSdkFolder;
    QString directX8SdkFolder;
    QString directX9SdkFolder;
    QString vsDocumentsFolder;

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