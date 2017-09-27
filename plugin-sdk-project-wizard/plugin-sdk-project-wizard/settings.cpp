#include "settings.h"

QString getSettingsString(QSettings &settingsFile, QString parameter, QString defaultVal) {
    return settingsFile.value(parameter, defaultVal).toString();
}

void AppSettings::read(QSettings &settingsFile) {
    pluginSdkFolder = getSettingsString(settingsFile, "PLUGIN_SDK_FOLDER", "");
    directX9SdkFolder = getSettingsString(settingsFile, "DIRECTX9_SDK_FOLDER", "");
    rwd3d9Folder = getSettingsString(settingsFile, "RWD3D9_FOLDER", "");
    vsDocumentsFolder = getSettingsString(settingsFile, "VS_DOCUMENTS_FOLDER", "");
    saAsiOutputFolder = getSettingsString(settingsFile, "SA/ASI_PLUGINS_OUTPUT_FOLDER", "");
    saCleoOutputFolder = getSettingsString(settingsFile, "SA/CLEO_PLUGINS_OUTPUT_FOLDER", "");
    saCleoSdkFolder = getSettingsString(settingsFile, "SA/CLEO_SDK_FOLDER", "");
    vcAsiOutputFolder = getSettingsString(settingsFile, "VC/ASI_PLUGINS_OUTPUT_FOLDER", "");
    vcCleoOutputFolder = getSettingsString(settingsFile, "VC/CLEO_PLUGINS_OUTPUT_FOLDER", "");
    vcCleoSdkFolder = getSettingsString(settingsFile, "VC/CLEO_SDK_FOLDER", "");
    iiiAsiOutputFolder = getSettingsString(settingsFile, "III/ASI_PLUGINS_OUTPUT_FOLDER", "");
    iiiCleoOutputFolder = getSettingsString(settingsFile, "III/CLEO_PLUGINS_OUTPUT_FOLDER", "");
    iiiCleoSdkFolder = getSettingsString(settingsFile, "III/CLEO_SDK_FOLDER", "");
}

void AppSettings::write(QSettings &settingsFile) {
    settingsFile.setValue("PLUGIN_SDK_FOLDER", pluginSdkFolder);
    settingsFile.setValue("DIRECTX9_SDK_FOLDER", directX9SdkFolder);
    settingsFile.setValue("RWD3D9_FOLDER", rwd3d9Folder);
    settingsFile.setValue("VS_DOCUMENTS_FOLDER", vsDocumentsFolder);
    settingsFile.setValue("SA/ASI_PLUGINS_OUTPUT_FOLDER", saAsiOutputFolder);
    settingsFile.setValue("SA/CLEO_PLUGINS_OUTPUT_FOLDER", saCleoOutputFolder);
    settingsFile.setValue("SA/CLEO_SDK_FOLDER", saCleoSdkFolder);
    settingsFile.setValue("VC/ASI_PLUGINS_OUTPUT_FOLDER", vcAsiOutputFolder);
    settingsFile.setValue("VC/CLEO_PLUGINS_OUTPUT_FOLDER", vcCleoOutputFolder);
    settingsFile.setValue("VC/CLEO_SDK_FOLDER", vcCleoSdkFolder);
    settingsFile.setValue("III/ASI_PLUGINS_OUTPUT_FOLDER", iiiAsiOutputFolder);
    settingsFile.setValue("III/CLEO_PLUGINS_OUTPUT_FOLDER", iiiCleoOutputFolder);
    settingsFile.setValue("III/CLEO_SDK_FOLDER", iiiCleoSdkFolder);
}