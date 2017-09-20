#include "projectwizard.h"
#include "VS_TemplateGenerator.h"
#include "BorderlessStyle.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <Windows.h>

#define DEFAULT_VS_VERSION 2017

ProjectWizard::ProjectWizard(QWidget *parent)
    : QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint)
{
    bInsideWindowAreaWhenMousePressed = false;
    SetWidgetBorderless(this);
    ui.setupUi(this);
    connect(ui.btn_close, &QPushButton::clicked, this, &ProjectWizard::on_closeButtonClicked);
    connect(ui.btn_minimize, &QPushButton::clicked, this, &ProjectWizard::on_minimizeButtonClicked);

    connect(ui.btn_browsePluginSdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browsePluginSdkFolderButtonClicked);
    connect(ui.btn_browseDirectX8SdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseDirectX8SdkFolderButtonClicked);
    connect(ui.btn_browseDirectX9SdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseDirectXS9dkFolderButtonClicked);
    connect(ui.btn_browseVisualStudioDocumentsFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseVisualStudioDocumentsFolderButtonClicked);
    connect(ui.btn_browseSaAsiOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseSaAsiPluginsFolderButtonClicked);
    connect(ui.btn_browseSaCleoSdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseSaCleoSdkFolderButtonClicked);
    connect(ui.btn_browseSaCleoOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseSaCleoPluginsFolderButtonClicked);
    connect(ui.btn_browseVcAsiOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseVcAsiPluginsFolderButtonClicked);
    connect(ui.btn_browseVcCleoSdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseVcCleoSdkFolderButtonClicked);
    connect(ui.btn_browseVcCleoOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseVcCleoPluginsFolderButtonClicked);
    connect(ui.btn_browseIIIAsiOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseIIIAsiPluginsFolderButtonClicked);
    connect(ui.btn_browseIIICleoSdkFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseIIICleoSdkFolderButtonClicked);
    connect(ui.btn_browseIIICleoOutputFolder, &QPushButton::clicked, this, &ProjectWizard::on_browseIIICleoPluginsFolderButtonClicked);

    connect(ui.btn_generate, &QPushButton::clicked, this, &ProjectWizard::on_generateButtonClicked);

    QSettings settingsFile(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.read(settingsFile);

    if (!settings.pluginSdkFolder.isEmpty())
        ui.le_pluginSdkFolder->setText(settings.pluginSdkFolder);
    if (!settings.directX8SdkFolder.isEmpty())
        ui.le_directX8SdkFolder->setText(settings.directX8SdkFolder);
    if (!settings.directX9SdkFolder.isEmpty())
        ui.le_directX9SdkFolder->setText(settings.directX9SdkFolder);
    if (!settings.vsDocumentsFolder.isEmpty())
        ui.le_visualStudioDocumentsFolder->setText(settings.vsDocumentsFolder);
    if (!settings.saAsiOutputFolder.isEmpty())
        ui.le_saAsiOutputFolder->setText(settings.saAsiOutputFolder);
    if (!settings.saCleoOutputFolder.isEmpty())
        ui.le_saCleoOutputFolder->setText(settings.saCleoOutputFolder);
    if (!settings.saCleoSdkFolder.isEmpty())
        ui.le_saCleoSdkFolder->setText(settings.saCleoSdkFolder);
    if (!settings.vcAsiOutputFolder.isEmpty())
        ui.le_vcAsiOutputFolder->setText(settings.vcAsiOutputFolder);
    if (!settings.vcCleoOutputFolder.isEmpty())
        ui.le_vcCleoOutputFolder->setText(settings.vcCleoOutputFolder);
    if (!settings.vcCleoSdkFolder.isEmpty())
        ui.le_vcCleoSdkFolder->setText(settings.vcCleoSdkFolder);
    if (!settings.iiiAsiOutputFolder.isEmpty())
        ui.le_iiiAsiOutputFolder->setText(settings.iiiAsiOutputFolder);
    if (!settings.iiiCleoOutputFolder.isEmpty())
        ui.le_iiiCleoOutputFolder->setText(settings.iiiCleoOutputFolder);
    if (!settings.iiiCleoSdkFolder.isEmpty())
        ui.le_iiiCleoSdkFolder->setText(settings.iiiCleoSdkFolder);
}

ProjectWizard::~ProjectWizard()
{

}

void MakePathLine(QString &str) {
    if (!str.endsWith('/') && !str.endsWith('\\'))
        str.append('/');
}

unsigned int GetVsVersionFromVSDocumentsPath(QString path) {
    if (path.length() > 4) {
        QString tmpVersion = path.mid(path.length() - 5, 4);
        bool bConvertedToInt = false;
        unsigned int intVersion = tmpVersion.toInt(&bConvertedToInt);
        if (bConvertedToInt)
            return intVersion;
    }
    return 0;
}

void ProjectWizard::on_generateButtonClicked(bool checked)
{
    QSettings settingsFile(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.pluginSdkFolder = ui.le_pluginSdkFolder->text();
    if (!ui.le_directX8SdkFolder->text().isEmpty())
        settings.directX8SdkFolder = ui.le_directX8SdkFolder->text();
    if (!ui.le_directX9SdkFolder->text().isEmpty())
        settings.directX9SdkFolder = ui.le_directX9SdkFolder->text();
    if (!ui.le_visualStudioDocumentsFolder->text().isEmpty() && QDir(ui.le_visualStudioDocumentsFolder->text()).exists())
        settings.vsDocumentsFolder = ui.le_visualStudioDocumentsFolder->text();
    if (!ui.le_saAsiOutputFolder->text().isEmpty())
        settings.saAsiOutputFolder = ui.le_saAsiOutputFolder->text();
    if (!ui.le_saCleoSdkFolder->text().isEmpty())
        settings.saCleoSdkFolder = ui.le_saCleoSdkFolder->text();
    if (!ui.le_saCleoOutputFolder->text().isEmpty())
        settings.saCleoOutputFolder = ui.le_saCleoOutputFolder->text();
    if (!ui.le_vcAsiOutputFolder->text().isEmpty())
        settings.vcAsiOutputFolder = ui.le_vcAsiOutputFolder->text();
    if (!ui.le_vcCleoSdkFolder->text().isEmpty())
        settings.vcCleoSdkFolder = ui.le_vcCleoSdkFolder->text();
    if (!ui.le_vcCleoOutputFolder->text().isEmpty())
        settings.vcCleoOutputFolder = ui.le_vcCleoOutputFolder->text();
    if (!ui.le_iiiAsiOutputFolder->text().isEmpty())
        settings.iiiAsiOutputFolder = ui.le_iiiAsiOutputFolder->text();
    if (!ui.le_iiiCleoSdkFolder->text().isEmpty())
        settings.iiiCleoSdkFolder = ui.le_iiiCleoSdkFolder->text();
    if (!ui.le_iiiCleoOutputFolder->text().isEmpty())
        settings.iiiCleoOutputFolder = ui.le_iiiCleoOutputFolder->text();
    settings.write(settingsFile);

    if (ui.le_pluginSdkFolder->text().isEmpty())
        ui.le_pluginSdkFolder->setText("$(PLUGIN_SDK_DIR)");
    if (!ui.le_visualStudioDocumentsFolder->text().isEmpty() && QDir(ui.le_visualStudioDocumentsFolder->text()).exists()) {
        VS_ProjectGenerationSettings genSettings;
        genSettings.vsDocumentsPath = ui.le_visualStudioDocumentsFolder->text();
        MakePathLine(genSettings.vsDocumentsPath);
        genSettings.pluginSdkPath = ui.le_pluginSdkFolder->text();
        MakePathLine(genSettings.pluginSdkPath);
        bool d3d8available = false;
        bool d3d9available = false;
        if (!ui.le_directX8SdkFolder->text().isEmpty()) {
            genSettings.d3d8SdkPath = ui.le_directX8SdkFolder->text();
            MakePathLine(genSettings.d3d8SdkPath);
            d3d8available = true;
        }
        if (!ui.le_directX9SdkFolder->text().isEmpty()) {
            genSettings.d3d9SdkPath = ui.le_directX9SdkFolder->text();
            MakePathLine(genSettings.d3d9SdkPath);
            d3d9available = true;
        }

        unsigned int intVersion = GetVsVersionFromVSDocumentsPath(genSettings.vsDocumentsPath);
        if (!intVersion) {
            MESSAGE_WARNING("Can't determine Visual Studio version (path: \"" + genSettings.vsDocumentsPath +
                "\"). Settings VS version to default (" + QString::number(DEFAULT_VS_VERSION) + ")");
            intVersion = DEFAULT_VS_VERSION;
        }

        if (!QDir("templates\\vs" + QString::number(intVersion)).exists()) {
            MESSAGE_ERROR("Directory for VS version \"" + QString::number(intVersion) + "\" does not exist (\"tepmplates\\vs" +
                QString::number(intVersion) + "\"). Settings VS version to default (" + QString::number(DEFAULT_VS_VERSION) + ")");
            intVersion = DEFAULT_VS_VERSION;
        }

        QString vsVersion = "vs" + QString::number(intVersion);

        if (!ui.le_saAsiOutputFolder->text().isEmpty()) {
            genSettings.saAsiOutputPath = ui.le_saAsiOutputFolder->text();
            MakePathLine(genSettings.saAsiOutputPath);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTASA", vsVersion, GTA_GAME_SA, 0, genSettings);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_1LA", vsVersion, GTA_GAME_SA, VSGEN_LA, genSettings);
            if (d3d9available) {
                VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_D3D9", vsVersion, GTA_GAME_SA, VSGEN_D3D9, genSettings);
                VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_D3D9_1LA", vsVersion, GTA_GAME_SA, VSGEN_D3D9 | VSGEN_LA, genSettings);
            }
            if (!ui.le_saCleoSdkFolder->text().isEmpty()) {
                genSettings.saCleoSdkPath = ui.le_saCleoSdkFolder->text();
                MakePathLine(genSettings.saCleoSdkPath);
                if (!ui.le_saCleoOutputFolder->text().isEmpty()) {
                    genSettings.saCleoOutputPath = ui.le_saCleoOutputFolder->text();
                    MakePathLine(genSettings.saCleoOutputPath);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_CLEO", vsVersion, GTA_GAME_SA, VSGEN_CLEO, genSettings);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_CLEO_1LA", vsVersion, GTA_GAME_SA, VSGEN_CLEO | VSGEN_LA, genSettings);
                    if (d3d9available) {
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_CLEO_D3D9", vsVersion, GTA_GAME_SA, VSGEN_CLEO | VSGEN_D3D9, genSettings);
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTASA_CLEO_D3D9_1LA", vsVersion, GTA_GAME_SA, VSGEN_CLEO | VSGEN_D3D9 | VSGEN_LA, genSettings);
                    }
                }
            }
        }

        if (!ui.le_vcAsiOutputFolder->text().isEmpty()) {
            genSettings.vcAsiOutputPath = ui.le_vcAsiOutputFolder->text();
            MakePathLine(genSettings.vcAsiOutputPath);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC", vsVersion, GTA_GAME_VC, 0, genSettings);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_1LA", vsVersion, GTA_GAME_VC, VSGEN_LA, genSettings);
            if (d3d8available) {
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_D3D8", vsVersion, GTA_GAME_VC, VSGEN_D3D8, genSettings);
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_D3D8_1LA", vsVersion, GTA_GAME_VC, VSGEN_D3D8 | VSGEN_LA, genSettings);
            }
            if (d3d9available) {
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_D3D8toD3D9", vsVersion, GTA_GAME_VC, VSGEN_D3D9, genSettings);
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_D3D8toD3D9_1LA", vsVersion, GTA_GAME_VC, VSGEN_D3D9 | VSGEN_LA, genSettings);
            }
            if (!ui.le_vcCleoSdkFolder->text().isEmpty()) {
                genSettings.vcCleoSdkPath = ui.le_vcCleoSdkFolder->text();
                MakePathLine(genSettings.vcCleoSdkPath);
                if (!ui.le_vcCleoOutputFolder->text().isEmpty()) {
                    genSettings.vcCleoOutputPath = ui.le_vcCleoOutputFolder->text();
                    MakePathLine(genSettings.vcCleoOutputPath);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO", vsVersion, GTA_GAME_VC, VSGEN_CLEO, genSettings);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO_1LA", vsVersion, GTA_GAME_VC, VSGEN_CLEO | VSGEN_LA, genSettings);
                    if (d3d8available) {
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO_D3D8", vsVersion, GTA_GAME_VC, VSGEN_CLEO | VSGEN_D3D8, genSettings);
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO_D3D8_1LA", vsVersion, GTA_GAME_VC, VSGEN_CLEO | VSGEN_D3D8 | VSGEN_LA, genSettings);
                    }
                    if (d3d9available) {
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO_D3D8toD3D9", vsVersion, GTA_GAME_VC, VSGEN_CLEO | VSGEN_D3D9, genSettings);
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAVC_CLEO_D3D8toD3D9_1LA", vsVersion, GTA_GAME_VC, VSGEN_CLEO | VSGEN_D3D9 | VSGEN_LA, genSettings);
                    }
                }
            }
        }

        if (!ui.le_iiiAsiOutputFolder->text().isEmpty()) {
            genSettings.iiiAsiOutputPath = ui.le_iiiAsiOutputFolder->text();
            MakePathLine(genSettings.iiiAsiOutputPath);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII", vsVersion, GTA_GAME_III, 0, genSettings);
            VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_1LA", vsVersion, GTA_GAME_III, VSGEN_LA, genSettings);
            if (d3d8available) {
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_D3D8", vsVersion, GTA_GAME_III, VSGEN_D3D8, genSettings);
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_D3D8_1LA", vsVersion, GTA_GAME_III, VSGEN_D3D8 | VSGEN_LA, genSettings);
            }
            if (d3d9available) {
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_D3D8toD3D9", vsVersion, GTA_GAME_III, VSGEN_D3D9, genSettings);
                VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_D3D8toD3D9_1LA", vsVersion, GTA_GAME_III, VSGEN_D3D9 | VSGEN_LA, genSettings);
            }
            if (!ui.le_iiiCleoSdkFolder->text().isEmpty()) {
                genSettings.iiiCleoSdkPath = ui.le_iiiCleoSdkFolder->text();
                MakePathLine(genSettings.iiiCleoSdkPath);
                if (!ui.le_iiiCleoOutputFolder->text().isEmpty()) {
                    genSettings.iiiCleoOutputPath = ui.le_iiiCleoOutputFolder->text();
                    MakePathLine(genSettings.iiiCleoOutputPath);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO", vsVersion, GTA_GAME_III, VSGEN_CLEO, genSettings);
                    VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO_1LA", vsVersion, GTA_GAME_III, VSGEN_CLEO | VSGEN_LA, genSettings);
                    if (d3d8available) {
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO_D3D8", vsVersion, GTA_GAME_III, VSGEN_CLEO | VSGEN_D3D8, genSettings);
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO_D3D8_1LA", vsVersion, GTA_GAME_III, VSGEN_CLEO | VSGEN_D3D8 | VSGEN_LA, genSettings);
                    }
                    if (d3d9available) {
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO_D3D8toD3D9", vsVersion, GTA_GAME_III, VSGEN_CLEO | VSGEN_D3D9, genSettings);
                        VS_ProjectTemplateGenerator::GenerateTemplate("GTAIII_CLEO_D3D8toD3D9_1LA", vsVersion, GTA_GAME_III, VSGEN_CLEO | VSGEN_D3D9 | VSGEN_LA, genSettings);
                    }
                }
            }
        }
    }
}

bool ProjectWizard::isPointInsideWindowArea(QPoint point)
{
    return point.x() > 7 && point.y() > 7 && point.x() < 446 && point.y() < 692;
}

void ProjectWizard::mousePressEvent(QMouseEvent* event)
{
    //if (event->button() == Qt::LeftButton && isPointInsideWindowArea(event->pos())) {
    //    dragPosition = event->globalPos() - frameGeometry().topLeft();
    //    event->accept();
    //    bInsideWindowAreaWhenMousePressed = true;
    //}
    QWidget::mousePressEvent(event);
    if (!event->isAccepted()) {
#ifdef Q_OS_WIN
        ReleaseCapture();
        SendMessage(reinterpret_cast<HWND>(winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
    }
#endif
}

void ProjectWizard::mouseMoveEvent(QMouseEvent* event) {
    if (bInsideWindowAreaWhenMousePressed && event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void ProjectWizard::mouseReleaseEvent(QMouseEvent* event) {
    bInsideWindowAreaWhenMousePressed = false;
}

void ProjectWizard::changeEvent(QEvent * event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange) {
    }
}

void ProjectWizard::on_closeButtonClicked(bool checked) {
    close();
}

void ProjectWizard::on_minimizeButtonClicked(bool checked) {
    showMinimized();
}

void ProjectWizard::on_browsePluginSdkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_pluginSdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_pluginSdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseDirectX8SdkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_directX8SdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_directX8SdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseDirectXS9dkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_directX9SdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_directX9SdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseVisualStudioDocumentsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_visualStudioDocumentsFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_visualStudioDocumentsFolder->setText(dir);
    }
}

void ProjectWizard::on_browseSaAsiPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_saAsiOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_saAsiOutputFolder->setText(dir);
    }
}

void ProjectWizard::on_browseSaCleoSdkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_saCleoSdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_saCleoSdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseSaCleoPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_saCleoOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_saCleoOutputFolder->setText(dir);
    }
}

void ProjectWizard::on_browseVcAsiPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_vcAsiOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_vcAsiOutputFolder->setText(dir);
    }
}

void ProjectWizard::on_browseVcCleoSdkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_vcCleoSdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_vcCleoSdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseVcCleoPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_vcCleoOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_vcCleoOutputFolder->setText(dir);
    }
}

void ProjectWizard::on_browseIIIAsiPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_iiiAsiOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_iiiAsiOutputFolder->setText(dir);
    }
}

void ProjectWizard::on_browseIIICleoSdkFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_iiiCleoSdkFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_iiiCleoSdkFolder->setText(dir);
    }
}

void ProjectWizard::on_browseIIICleoPluginsFolderButtonClicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        ui.le_iiiCleoOutputFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui.le_iiiCleoOutputFolder->setText(dir);
    }
}