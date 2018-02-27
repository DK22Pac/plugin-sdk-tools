/********************************************************************************
** Form generated from reading UI file 'projectwizard.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJECTWIZARD_H
#define UI_PROJECTWIZARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProjectWizardClass
{
public:
    QWidget *centralWidget;
    QLabel *lbl_rwd3d9Folder;
    QLabel *lbl_title;
    QLabel *lbl_visualStudioDocumentsFolder;
    QLineEdit *le_directX9SdkFolder;
    QLineEdit *le_visualStudioDocumentsFolder;
    QPushButton *btn_browseDirectX9SdkFolder;
    QPushButton *btn_restore;
    QPushButton *btn_close;
    QLineEdit *le_rwd3d9Folder;
    QLabel *lbl_icon;
    QLabel *lbl_optionalRwD3d9;
    QLineEdit *le_pluginSdkFolder;
    QPushButton *btn_browseRwD3d9Folder;
    QLabel *lbl_directX9SdkFolder;
    QPushButton *btn_browsePluginSdkFolder;
    QPushButton *btn_browseVisualStudioDocumentsFolder;
    QPushButton *btn_generate;
    QLabel *lbl_pluginSdkFolder;
    QPushButton *btn_minimize;
    QLabel *lbl_optionalDx9Sdk;
    QTabWidget *tabGames;
    QWidget *tabSa;
    QLabel *lbl_saAsiOutputFolder;
    QLabel *lbl_saCleoOutputFolder;
    QLabel *lbl_optionalSaCleoSdk;
    QPushButton *btn_browseSaCleoOutputFolder;
    QPushButton *btn_browseSaCleoSdkFolder;
    QLabel *lbl_saCleoSdkFolder;
    QPushButton *btn_browseSaAsiOutputFolder;
    QLabel *lbl_optionalSaCleoOutput;
    QLineEdit *le_saCleoSdkFolder;
    QLineEdit *le_saCleoOutputFolder;
    QLineEdit *le_saAsiOutputFolder;
    QWidget *tabVc;
    QPushButton *btn_browseVcCleoSdkFolder;
    QLineEdit *le_vcCleoOutputFolder;
    QPushButton *btn_browseVcCleoOutputFolder;
    QLabel *lbl_vcAsiOutputFolder;
    QLineEdit *le_vcCleoSdkFolder;
    QLabel *lbl_optionalVcCleoOutput;
    QLabel *lbl_optionalVcCleoSdk;
    QLabel *lbl_vcCleoSdkFolder;
    QLineEdit *le_vcAsiOutputFolder;
    QLabel *lbl_vcCleoOutputFolder;
    QPushButton *btn_browseVcAsiOutputFolder;
    QWidget *tabIII;
    QLabel *lbl_iiiAsiOutputFolder;
    QLineEdit *le_iiiCleoSdkFolder;
    QPushButton *btn_browseIIICleoOutputFolder;
    QPushButton *btn_browseIIIAsiOutputFolder;
    QLineEdit *le_iiiCleoOutputFolder;
    QLineEdit *le_iiiAsiOutputFolder;
    QLabel *lbl_iiiCleoSdkFolder;
    QLabel *lbl_iiiCleoOutputFolder;
    QLabel *lbl_optionalIIICleoOutput;
    QLabel *lbl_optionalIIICleoSdk;
    QPushButton *btn_browseIIICleoSdkFolder;
    QPushButton *btn_useEnvironmentVariables;

    void setupUi(QMainWindow *ProjectWizardClass)
    {
        if (ProjectWizardClass->objectName().isEmpty())
            ProjectWizardClass->setObjectName(QStringLiteral("ProjectWizardClass"));
        ProjectWizardClass->resize(454, 530);
        ProjectWizardClass->setMinimumSize(QSize(454, 530));
        ProjectWizardClass->setMaximumSize(QSize(500, 530));
        QIcon icon;
        icon.addFile(QStringLiteral("resources/icon_w.png"), QSize(), QIcon::Normal, QIcon::Off);
        ProjectWizardClass->setWindowIcon(icon);
        ProjectWizardClass->setWindowOpacity(1);
        ProjectWizardClass->setAutoFillBackground(true);
        ProjectWizardClass->setStyleSheet(QLatin1String("QMenu {\n"
"    color: white;\n"
"    background-color: black;\n"
"    border: 1px solid rgb(51,153,255);\n"
"}\n"
"\n"
"QMenu::item {\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: rgb(45,45,48);\n"
"}\n"
"\n"
"QWidget#centralWidget, QWidget#dialogWidget {\n"
"   background: rgba(45,45,48, 255);\n"
"}\n"
"\n"
"QLabel {\n"
"   color: white;\n"
"}\n"
"\n"
"QLabel#lbl_title {\n"
"    color: rgb(160,160,165);\n"
"}\n"
"\n"
"QLabel#lbl_title:!active {\n"
"    color: rgb(100,100,105);\n"
"}\n"
"\n"
"QLabel#lbl_icon {\n"
"    background-image: url(resources/icon_small.png);\n"
"}\n"
"\n"
"QLabel#lbl_icon:!active {\n"
"    background-image: url(resources/icon_small_inactive.png);\n"
"}\n"
"\n"
"QLineEdit {\n"
"   color: white;\n"
"   border: 1px solid rgb(63,63,70);\n"
"   background: rgb(63,63,70);\n"
"   selection-color: rgb(165,206,247);\n"
"   selection-background-color: rgb(58,99,144);\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"   border: 1px solid rgb(51,153,255"
                        ");\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"   color: rgb(81,101,92);\n"
"   border: 1px solid rgb(67,67,70);\n"
"   background: rgb(45,45,48);\n"
"}\n"
"\n"
"QCheckBox {\n"
"   color: white;\n"
"}\n"
"\n"
"QCheckBox:disabled {\n"
"   color: rgb(150,150,150);\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 13px;\n"
"    height: 13px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    image: url(resources/cb_icon.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:hover {\n"
"    image: url(resources/cb_icon_active.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    image: url(resources/cb_icon_checked.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:hover {\n"
"    image: url(resources/cb_icon_checked_active.png);\n"
"}\n"
"\n"
"QGroupBox {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 0px;\n"
"    margin-top: 0.5em;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    color: white;\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 3px 0 3px;\n"
"}\n"
"\n"
"QPushButton"
                        " {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 1px solid rgb(100,100,110);\n"
"    color: rgb(100,100,110);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 1px solid rgb(51,153,255);\n"
"    color: rgb(51,153,255);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(51,153,255);\n"
"    border: 1px solid rgb(51,153,255);\n"
"    color: rgb(37,37,38);\n"
"}\n"
"\n"
"QPushButton#btn_close {\n"
"    border: 0px;\n"
"    background-image: url(resources/btn_close.png);\n"
"}\n"
"\n"
"QPushButton#btn_close:disabled {\n"
"    background-image: url(resources/btn_close_inactive.png);\n"
"}\n"
"\n"
"QPushButton#btn_close:hover {\n"
"    background-image: url(resources/btn_close_hover.png);\n"
"}\n"
"\n"
"QPushButton#btn_close:pressed {\n"
"    background-image: url(resources/btn_close_pressed.png);\n"
"}\n"
"\n"
"QPushButton#btn_restore {\n"
"    border: 0px;\n"
"    background-image: url(resources/btn_restore.png);\n"
"}\n"
"\n"
"QPushButton#btn_re"
                        "store:disabled {\n"
"    background-image: url(resources/btn_restore_inactive.png);\n"
"}\n"
"\n"
"QPushButton#btn_restore:hover {\n"
"    background-image: url(resources/btn_restore_hover.png);\n"
"}\n"
"\n"
"QPushButton#btn_restore:pressed {\n"
"    background-image: url(resources/btn_restore_pressed.png);\n"
"}\n"
"\n"
"QPushButton#btn_minimize {\n"
"    border: 0px;\n"
"    background-image: url(resources/btn_minimize.png);\n"
"}\n"
"\n"
"QPushButton#btn_minimize:disabled {\n"
"    background-image: url(resources/btn_minimize_inactive.png);\n"
"}\n"
"\n"
"QPushButton#btn_minimize:hover {\n"
"    background-image: url(resources/btn_minimize_hover.png);\n"
"}\n"
"\n"
"QPushButton#btn_minimize:pressed {\n"
"    background-image: url(resources/btn_minimize_pressed.png);\n"
"}\n"
"\n"
"QToolTip {\n"
"    border: 0px;\n"
"    background: rgb(14,14,15);\n"
"    color: rgb(180,180,180);\n"
"}\n"
"\n"
"QTabWidget QStackedWidget > QWidget {\n"
"    background-color: rgb(37,37,38);\n"
"}\n"
"\n"
"QTabWidget::pane {}\n"
""
                        "\n"
"QTabBar::tab {\n"
"    color: white;\n"
"    background: rgb(37,37,38);\n"
"    border: 1px solid rgb(37,37,38);\n"
"    min-width: 60px;\n"
"    padding: 6px;\n"
"}\n"
"\n"
"QTabBar::tab:!selected {\n"
"    color: rgb(160,160,165);\n"
"    background: rgb(45,45,48);\n"
"}\n"
"\n"
"QTabBar::tab::selected {\n"
"    color: rgb(255,67,209);\n"
"}\n"
"\n"
"QTabBar::tab::first::selected {\n"
"    color: rgb(230,170,0);\n"
"}\n"
"\n"
"QTabBar::tab::last::selected {\n"
"    color: rgb(206,185,128);\n"
"}"));
        centralWidget = new QWidget(ProjectWizardClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setEnabled(true);
        centralWidget->setLayoutDirection(Qt::LeftToRight);
        centralWidget->setStyleSheet(QStringLiteral(""));
        lbl_rwd3d9Folder = new QLabel(centralWidget);
        lbl_rwd3d9Folder->setObjectName(QStringLiteral("lbl_rwd3d9Folder"));
        lbl_rwd3d9Folder->setGeometry(QRect(30, 140, 281, 16));
        lbl_title = new QLabel(centralWidget);
        lbl_title->setObjectName(QStringLiteral("lbl_title"));
        lbl_title->setGeometry(QRect(45, 7, 291, 16));
        lbl_visualStudioDocumentsFolder = new QLabel(centralWidget);
        lbl_visualStudioDocumentsFolder->setObjectName(QStringLiteral("lbl_visualStudioDocumentsFolder"));
        lbl_visualStudioDocumentsFolder->setGeometry(QRect(30, 410, 361, 20));
        le_directX9SdkFolder = new QLineEdit(centralWidget);
        le_directX9SdkFolder->setObjectName(QStringLiteral("le_directX9SdkFolder"));
        le_directX9SdkFolder->setGeometry(QRect(30, 110, 361, 20));
        le_visualStudioDocumentsFolder = new QLineEdit(centralWidget);
        le_visualStudioDocumentsFolder->setObjectName(QStringLiteral("le_visualStudioDocumentsFolder"));
        le_visualStudioDocumentsFolder->setGeometry(QRect(30, 430, 361, 21));
        btn_browseDirectX9SdkFolder = new QPushButton(centralWidget);
        btn_browseDirectX9SdkFolder->setObjectName(QStringLiteral("btn_browseDirectX9SdkFolder"));
        btn_browseDirectX9SdkFolder->setGeometry(QRect(400, 110, 33, 21));
        btn_restore = new QPushButton(centralWidget);
        btn_restore->setObjectName(QStringLiteral("btn_restore"));
        btn_restore->setEnabled(false);
        btn_restore->setGeometry(QRect(386, 0, 34, 26));
        btn_close = new QPushButton(centralWidget);
        btn_close->setObjectName(QStringLiteral("btn_close"));
        btn_close->setGeometry(QRect(420, 0, 34, 26));
        le_rwd3d9Folder = new QLineEdit(centralWidget);
        le_rwd3d9Folder->setObjectName(QStringLiteral("le_rwd3d9Folder"));
        le_rwd3d9Folder->setGeometry(QRect(30, 160, 361, 20));
        lbl_icon = new QLabel(centralWidget);
        lbl_icon->setObjectName(QStringLiteral("lbl_icon"));
        lbl_icon->setGeometry(QRect(10, 6, 22, 22));
        lbl_optionalRwD3d9 = new QLabel(centralWidget);
        lbl_optionalRwD3d9->setObjectName(QStringLiteral("lbl_optionalRwD3d9"));
        lbl_optionalRwD3d9->setGeometry(QRect(330, 140, 61, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lbl_optionalRwD3d9->setFont(font);
        lbl_optionalRwD3d9->setStyleSheet(QStringLiteral("color: grey"));
        le_pluginSdkFolder = new QLineEdit(centralWidget);
        le_pluginSdkFolder->setObjectName(QStringLiteral("le_pluginSdkFolder"));
        le_pluginSdkFolder->setGeometry(QRect(30, 60, 361, 21));
        btn_browseRwD3d9Folder = new QPushButton(centralWidget);
        btn_browseRwD3d9Folder->setObjectName(QStringLiteral("btn_browseRwD3d9Folder"));
        btn_browseRwD3d9Folder->setGeometry(QRect(400, 160, 33, 21));
        lbl_directX9SdkFolder = new QLabel(centralWidget);
        lbl_directX9SdkFolder->setObjectName(QStringLiteral("lbl_directX9SdkFolder"));
        lbl_directX9SdkFolder->setGeometry(QRect(30, 90, 281, 16));
        btn_browsePluginSdkFolder = new QPushButton(centralWidget);
        btn_browsePluginSdkFolder->setObjectName(QStringLiteral("btn_browsePluginSdkFolder"));
        btn_browsePluginSdkFolder->setGeometry(QRect(400, 60, 33, 21));
        btn_browseVisualStudioDocumentsFolder = new QPushButton(centralWidget);
        btn_browseVisualStudioDocumentsFolder->setObjectName(QStringLiteral("btn_browseVisualStudioDocumentsFolder"));
        btn_browseVisualStudioDocumentsFolder->setGeometry(QRect(400, 430, 33, 21));
        btn_generate = new QPushButton(centralWidget);
        btn_generate->setObjectName(QStringLiteral("btn_generate"));
        btn_generate->setGeometry(QRect(293, 479, 141, 26));
        lbl_pluginSdkFolder = new QLabel(centralWidget);
        lbl_pluginSdkFolder->setObjectName(QStringLiteral("lbl_pluginSdkFolder"));
        lbl_pluginSdkFolder->setGeometry(QRect(30, 40, 361, 20));
        btn_minimize = new QPushButton(centralWidget);
        btn_minimize->setObjectName(QStringLiteral("btn_minimize"));
        btn_minimize->setGeometry(QRect(352, 0, 34, 26));
        lbl_optionalDx9Sdk = new QLabel(centralWidget);
        lbl_optionalDx9Sdk->setObjectName(QStringLiteral("lbl_optionalDx9Sdk"));
        lbl_optionalDx9Sdk->setGeometry(QRect(330, 90, 61, 20));
        lbl_optionalDx9Sdk->setFont(font);
        lbl_optionalDx9Sdk->setStyleSheet(QStringLiteral("color: grey"));
        tabGames = new QTabWidget(centralWidget);
        tabGames->setObjectName(QStringLiteral("tabGames"));
        tabGames->setGeometry(QRect(30, 200, 401, 201));
        tabGames->setStyleSheet(QStringLiteral(""));
        tabGames->setTabPosition(QTabWidget::North);
        tabSa = new QWidget();
        tabSa->setObjectName(QStringLiteral("tabSa"));
        tabSa->setAutoFillBackground(false);
        tabSa->setStyleSheet(QLatin1String("QLineEdit {\n"
"   selection-color: rgb(255,215,118);\n"
"   selection-background-color: rgb(134,114,58);\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"   border: 1px solid rgb(230,170,0);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 1px solid rgb(230,170,0);\n"
"    color: rgb(230,170,0);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(230,170,0);\n"
"    border: 1px solid rgb(230,170,0);\n"
"    color: rgb(37,37,38);\n"
"}"));
        lbl_saAsiOutputFolder = new QLabel(tabSa);
        lbl_saAsiOutputFolder->setObjectName(QStringLiteral("lbl_saAsiOutputFolder"));
        lbl_saAsiOutputFolder->setGeometry(QRect(20, 10, 361, 20));
        lbl_saCleoOutputFolder = new QLabel(tabSa);
        lbl_saCleoOutputFolder->setObjectName(QStringLiteral("lbl_saCleoOutputFolder"));
        lbl_saCleoOutputFolder->setGeometry(QRect(20, 110, 361, 20));
        lbl_optionalSaCleoSdk = new QLabel(tabSa);
        lbl_optionalSaCleoSdk->setObjectName(QStringLiteral("lbl_optionalSaCleoSdk"));
        lbl_optionalSaCleoSdk->setGeometry(QRect(280, 60, 61, 20));
        lbl_optionalSaCleoSdk->setFont(font);
        lbl_optionalSaCleoSdk->setStyleSheet(QStringLiteral("color: grey"));
        btn_browseSaCleoOutputFolder = new QPushButton(tabSa);
        btn_browseSaCleoOutputFolder->setObjectName(QStringLiteral("btn_browseSaCleoOutputFolder"));
        btn_browseSaCleoOutputFolder->setGeometry(QRect(350, 130, 33, 21));
        btn_browseSaCleoSdkFolder = new QPushButton(tabSa);
        btn_browseSaCleoSdkFolder->setObjectName(QStringLiteral("btn_browseSaCleoSdkFolder"));
        btn_browseSaCleoSdkFolder->setGeometry(QRect(350, 80, 33, 21));
        lbl_saCleoSdkFolder = new QLabel(tabSa);
        lbl_saCleoSdkFolder->setObjectName(QStringLiteral("lbl_saCleoSdkFolder"));
        lbl_saCleoSdkFolder->setGeometry(QRect(20, 60, 361, 20));
        btn_browseSaAsiOutputFolder = new QPushButton(tabSa);
        btn_browseSaAsiOutputFolder->setObjectName(QStringLiteral("btn_browseSaAsiOutputFolder"));
        btn_browseSaAsiOutputFolder->setGeometry(QRect(350, 30, 33, 21));
        lbl_optionalSaCleoOutput = new QLabel(tabSa);
        lbl_optionalSaCleoOutput->setObjectName(QStringLiteral("lbl_optionalSaCleoOutput"));
        lbl_optionalSaCleoOutput->setGeometry(QRect(280, 110, 61, 20));
        lbl_optionalSaCleoOutput->setFont(font);
        lbl_optionalSaCleoOutput->setStyleSheet(QStringLiteral("color: grey"));
        le_saCleoSdkFolder = new QLineEdit(tabSa);
        le_saCleoSdkFolder->setObjectName(QStringLiteral("le_saCleoSdkFolder"));
        le_saCleoSdkFolder->setGeometry(QRect(20, 80, 321, 20));
        le_saCleoOutputFolder = new QLineEdit(tabSa);
        le_saCleoOutputFolder->setObjectName(QStringLiteral("le_saCleoOutputFolder"));
        le_saCleoOutputFolder->setGeometry(QRect(20, 130, 321, 20));
        le_saAsiOutputFolder = new QLineEdit(tabSa);
        le_saAsiOutputFolder->setObjectName(QStringLiteral("le_saAsiOutputFolder"));
        le_saAsiOutputFolder->setGeometry(QRect(20, 30, 321, 20));
        tabGames->addTab(tabSa, QString());
        tabVc = new QWidget();
        tabVc->setObjectName(QStringLiteral("tabVc"));
        tabVc->setAutoFillBackground(false);
        tabVc->setStyleSheet(QLatin1String("QLineEdit {\n"
"   selection-color: rgb(255,151,230);\n"
"   selection-background-color: rgb(132,66,116);\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"   border: 1px solid rgb(255,67,209);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 1px solid rgb(255,67,209);\n"
"    color: rgb(255,67,209);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(255,67,209);\n"
"    border: 1px solid rgb(255,67,209);\n"
"    color: rgb(37,37,38);\n"
"}"));
        btn_browseVcCleoSdkFolder = new QPushButton(tabVc);
        btn_browseVcCleoSdkFolder->setObjectName(QStringLiteral("btn_browseVcCleoSdkFolder"));
        btn_browseVcCleoSdkFolder->setGeometry(QRect(350, 80, 33, 21));
        le_vcCleoOutputFolder = new QLineEdit(tabVc);
        le_vcCleoOutputFolder->setObjectName(QStringLiteral("le_vcCleoOutputFolder"));
        le_vcCleoOutputFolder->setGeometry(QRect(20, 130, 321, 20));
        btn_browseVcCleoOutputFolder = new QPushButton(tabVc);
        btn_browseVcCleoOutputFolder->setObjectName(QStringLiteral("btn_browseVcCleoOutputFolder"));
        btn_browseVcCleoOutputFolder->setGeometry(QRect(350, 130, 33, 21));
        lbl_vcAsiOutputFolder = new QLabel(tabVc);
        lbl_vcAsiOutputFolder->setObjectName(QStringLiteral("lbl_vcAsiOutputFolder"));
        lbl_vcAsiOutputFolder->setGeometry(QRect(20, 10, 361, 20));
        le_vcCleoSdkFolder = new QLineEdit(tabVc);
        le_vcCleoSdkFolder->setObjectName(QStringLiteral("le_vcCleoSdkFolder"));
        le_vcCleoSdkFolder->setGeometry(QRect(20, 80, 321, 20));
        lbl_optionalVcCleoOutput = new QLabel(tabVc);
        lbl_optionalVcCleoOutput->setObjectName(QStringLiteral("lbl_optionalVcCleoOutput"));
        lbl_optionalVcCleoOutput->setGeometry(QRect(280, 110, 61, 20));
        lbl_optionalVcCleoOutput->setFont(font);
        lbl_optionalVcCleoOutput->setStyleSheet(QStringLiteral("color: grey"));
        lbl_optionalVcCleoSdk = new QLabel(tabVc);
        lbl_optionalVcCleoSdk->setObjectName(QStringLiteral("lbl_optionalVcCleoSdk"));
        lbl_optionalVcCleoSdk->setGeometry(QRect(280, 60, 61, 20));
        lbl_optionalVcCleoSdk->setFont(font);
        lbl_optionalVcCleoSdk->setStyleSheet(QStringLiteral("color: grey"));
        lbl_vcCleoSdkFolder = new QLabel(tabVc);
        lbl_vcCleoSdkFolder->setObjectName(QStringLiteral("lbl_vcCleoSdkFolder"));
        lbl_vcCleoSdkFolder->setGeometry(QRect(20, 60, 361, 20));
        le_vcAsiOutputFolder = new QLineEdit(tabVc);
        le_vcAsiOutputFolder->setObjectName(QStringLiteral("le_vcAsiOutputFolder"));
        le_vcAsiOutputFolder->setGeometry(QRect(20, 30, 321, 20));
        lbl_vcCleoOutputFolder = new QLabel(tabVc);
        lbl_vcCleoOutputFolder->setObjectName(QStringLiteral("lbl_vcCleoOutputFolder"));
        lbl_vcCleoOutputFolder->setGeometry(QRect(20, 110, 361, 20));
        btn_browseVcAsiOutputFolder = new QPushButton(tabVc);
        btn_browseVcAsiOutputFolder->setObjectName(QStringLiteral("btn_browseVcAsiOutputFolder"));
        btn_browseVcAsiOutputFolder->setGeometry(QRect(350, 30, 33, 21));
        tabGames->addTab(tabVc, QString());
        tabIII = new QWidget();
        tabIII->setObjectName(QStringLiteral("tabIII"));
        tabIII->setAutoFillBackground(false);
        tabIII->setStyleSheet(QLatin1String("QLineEdit {\n"
"   selection-color: rgb(230,209,151);\n"
"   selection-background-color: rgb(125,113,80);\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"   border: 1px solid rgb(206,185,128);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(37,37,38);\n"
"    border: 1px solid rgb(206,185,128);\n"
"    color: rgb(206,185,128);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(206,185,128);\n"
"    border: 1px solid rgb(206,185,128);\n"
"    color: rgb(37,37,38);\n"
"}"));
        lbl_iiiAsiOutputFolder = new QLabel(tabIII);
        lbl_iiiAsiOutputFolder->setObjectName(QStringLiteral("lbl_iiiAsiOutputFolder"));
        lbl_iiiAsiOutputFolder->setGeometry(QRect(20, 10, 361, 20));
        le_iiiCleoSdkFolder = new QLineEdit(tabIII);
        le_iiiCleoSdkFolder->setObjectName(QStringLiteral("le_iiiCleoSdkFolder"));
        le_iiiCleoSdkFolder->setGeometry(QRect(20, 80, 321, 20));
        btn_browseIIICleoOutputFolder = new QPushButton(tabIII);
        btn_browseIIICleoOutputFolder->setObjectName(QStringLiteral("btn_browseIIICleoOutputFolder"));
        btn_browseIIICleoOutputFolder->setGeometry(QRect(350, 130, 33, 21));
        btn_browseIIIAsiOutputFolder = new QPushButton(tabIII);
        btn_browseIIIAsiOutputFolder->setObjectName(QStringLiteral("btn_browseIIIAsiOutputFolder"));
        btn_browseIIIAsiOutputFolder->setGeometry(QRect(350, 30, 33, 21));
        le_iiiCleoOutputFolder = new QLineEdit(tabIII);
        le_iiiCleoOutputFolder->setObjectName(QStringLiteral("le_iiiCleoOutputFolder"));
        le_iiiCleoOutputFolder->setGeometry(QRect(20, 130, 321, 20));
        le_iiiAsiOutputFolder = new QLineEdit(tabIII);
        le_iiiAsiOutputFolder->setObjectName(QStringLiteral("le_iiiAsiOutputFolder"));
        le_iiiAsiOutputFolder->setGeometry(QRect(20, 30, 321, 20));
        lbl_iiiCleoSdkFolder = new QLabel(tabIII);
        lbl_iiiCleoSdkFolder->setObjectName(QStringLiteral("lbl_iiiCleoSdkFolder"));
        lbl_iiiCleoSdkFolder->setGeometry(QRect(20, 60, 361, 20));
        lbl_iiiCleoOutputFolder = new QLabel(tabIII);
        lbl_iiiCleoOutputFolder->setObjectName(QStringLiteral("lbl_iiiCleoOutputFolder"));
        lbl_iiiCleoOutputFolder->setGeometry(QRect(20, 110, 361, 20));
        lbl_optionalIIICleoOutput = new QLabel(tabIII);
        lbl_optionalIIICleoOutput->setObjectName(QStringLiteral("lbl_optionalIIICleoOutput"));
        lbl_optionalIIICleoOutput->setGeometry(QRect(280, 110, 61, 20));
        lbl_optionalIIICleoOutput->setFont(font);
        lbl_optionalIIICleoOutput->setStyleSheet(QStringLiteral("color: grey"));
        lbl_optionalIIICleoSdk = new QLabel(tabIII);
        lbl_optionalIIICleoSdk->setObjectName(QStringLiteral("lbl_optionalIIICleoSdk"));
        lbl_optionalIIICleoSdk->setGeometry(QRect(280, 60, 61, 20));
        lbl_optionalIIICleoSdk->setFont(font);
        lbl_optionalIIICleoSdk->setStyleSheet(QStringLiteral("color: grey"));
        btn_browseIIICleoSdkFolder = new QPushButton(tabIII);
        btn_browseIIICleoSdkFolder->setObjectName(QStringLiteral("btn_browseIIICleoSdkFolder"));
        btn_browseIIICleoSdkFolder->setGeometry(QRect(350, 80, 33, 21));
        tabGames->addTab(tabIII, QString());
        btn_useEnvironmentVariables = new QPushButton(centralWidget);
        btn_useEnvironmentVariables->setObjectName(QStringLiteral("btn_useEnvironmentVariables"));
        btn_useEnvironmentVariables->setGeometry(QRect(30, 480, 141, 26));
        ProjectWizardClass->setCentralWidget(centralWidget);
        QWidget::setTabOrder(btn_minimize, btn_restore);
        QWidget::setTabOrder(btn_restore, btn_close);
        QWidget::setTabOrder(btn_close, le_pluginSdkFolder);
        QWidget::setTabOrder(le_pluginSdkFolder, btn_browsePluginSdkFolder);
        QWidget::setTabOrder(btn_browsePluginSdkFolder, le_directX9SdkFolder);
        QWidget::setTabOrder(le_directX9SdkFolder, btn_browseDirectX9SdkFolder);
        QWidget::setTabOrder(btn_browseDirectX9SdkFolder, le_rwd3d9Folder);
        QWidget::setTabOrder(le_rwd3d9Folder, btn_browseRwD3d9Folder);
        QWidget::setTabOrder(btn_browseRwD3d9Folder, tabGames);
        QWidget::setTabOrder(tabGames, le_saAsiOutputFolder);
        QWidget::setTabOrder(le_saAsiOutputFolder, btn_browseSaAsiOutputFolder);
        QWidget::setTabOrder(btn_browseSaAsiOutputFolder, le_saCleoSdkFolder);
        QWidget::setTabOrder(le_saCleoSdkFolder, btn_browseSaCleoSdkFolder);
        QWidget::setTabOrder(btn_browseSaCleoSdkFolder, le_saCleoOutputFolder);
        QWidget::setTabOrder(le_saCleoOutputFolder, btn_browseSaCleoOutputFolder);
        QWidget::setTabOrder(btn_browseSaCleoOutputFolder, le_visualStudioDocumentsFolder);
        QWidget::setTabOrder(le_visualStudioDocumentsFolder, btn_browseVisualStudioDocumentsFolder);
        QWidget::setTabOrder(btn_browseVisualStudioDocumentsFolder, btn_generate);
        QWidget::setTabOrder(btn_generate, btn_browseVcCleoSdkFolder);
        QWidget::setTabOrder(btn_browseVcCleoSdkFolder, le_vcCleoOutputFolder);
        QWidget::setTabOrder(le_vcCleoOutputFolder, btn_browseVcCleoOutputFolder);
        QWidget::setTabOrder(btn_browseVcCleoOutputFolder, le_vcCleoSdkFolder);
        QWidget::setTabOrder(le_vcCleoSdkFolder, le_vcAsiOutputFolder);
        QWidget::setTabOrder(le_vcAsiOutputFolder, btn_browseVcAsiOutputFolder);
        QWidget::setTabOrder(btn_browseVcAsiOutputFolder, le_iiiCleoSdkFolder);
        QWidget::setTabOrder(le_iiiCleoSdkFolder, btn_browseIIICleoOutputFolder);
        QWidget::setTabOrder(btn_browseIIICleoOutputFolder, btn_browseIIIAsiOutputFolder);
        QWidget::setTabOrder(btn_browseIIIAsiOutputFolder, le_iiiCleoOutputFolder);
        QWidget::setTabOrder(le_iiiCleoOutputFolder, le_iiiAsiOutputFolder);
        QWidget::setTabOrder(le_iiiAsiOutputFolder, btn_browseIIICleoSdkFolder);

        retranslateUi(ProjectWizardClass);

        tabGames->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ProjectWizardClass);
    } // setupUi

    void retranslateUi(QMainWindow *ProjectWizardClass)
    {
        ProjectWizardClass->setWindowTitle(QApplication::translate("ProjectWizardClass", "Plugin-SDK Templates Generator", nullptr));
        lbl_rwd3d9Folder->setText(QApplication::translate("ProjectWizardClass", "rwd3d9 folder:", nullptr));
        lbl_title->setText(QApplication::translate("ProjectWizardClass", "Plugin-SDK Templates Generator v.0.9.4", nullptr));
        lbl_visualStudioDocumentsFolder->setText(QApplication::translate("ProjectWizardClass", "Visual Studio Documents folder:", nullptr));
#ifndef QT_NO_TOOLTIP
        le_directX9SdkFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/DXSDK/9.0c", nullptr));
#endif // QT_NO_TOOLTIP
        le_directX9SdkFolder->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        le_visualStudioDocumentsFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Documents/Visual Studio 2017", nullptr));
#endif // QT_NO_TOOLTIP
        le_visualStudioDocumentsFolder->setPlaceholderText(QString());
        btn_browseDirectX9SdkFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        btn_restore->setText(QString());
        btn_close->setText(QString());
#ifndef QT_NO_TOOLTIP
        le_rwd3d9Folder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/rwd3d8to9", nullptr));
#endif // QT_NO_TOOLTIP
        le_rwd3d9Folder->setPlaceholderText(QString());
        lbl_icon->setText(QString());
        lbl_optionalRwD3d9->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
#ifndef QT_NO_TOOLTIP
        le_pluginSdkFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/plugin-sdk", nullptr));
#endif // QT_NO_TOOLTIP
        le_pluginSdkFolder->setPlaceholderText(QString());
        btn_browseRwD3d9Folder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        lbl_directX9SdkFolder->setText(QApplication::translate("ProjectWizardClass", "DirectX 9 SDK folder:", nullptr));
        btn_browsePluginSdkFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        btn_browseVisualStudioDocumentsFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        btn_generate->setText(QApplication::translate("ProjectWizardClass", "Install", nullptr));
        lbl_pluginSdkFolder->setText(QApplication::translate("ProjectWizardClass", "Plugin-SDK folder:", nullptr));
        btn_minimize->setText(QString());
        lbl_optionalDx9Sdk->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        lbl_saAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "ASI plugins output folder:", nullptr));
        lbl_saCleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO plugins output folder:", nullptr));
        lbl_optionalSaCleoSdk->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        btn_browseSaCleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        btn_browseSaCleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        lbl_saCleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO SDK folder:", nullptr));
        btn_browseSaAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        lbl_optionalSaCleoOutput->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
#ifndef QT_NO_TOOLTIP
        le_saCleoSdkFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/cleo/sa/cleo_sdk", nullptr));
#endif // QT_NO_TOOLTIP
        le_saCleoSdkFolder->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        le_saCleoOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto - San Andreas/cleo", nullptr));
#endif // QT_NO_TOOLTIP
        le_saCleoOutputFolder->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        le_saAsiOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto - San Andreas/scripts", nullptr));
#endif // QT_NO_TOOLTIP
        le_saAsiOutputFolder->setPlaceholderText(QString());
        tabGames->setTabText(tabGames->indexOf(tabSa), QApplication::translate("ProjectWizardClass", "San Andreas", nullptr));
        btn_browseVcCleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        le_vcCleoOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto - Vice City/cleo", nullptr));
#endif // QT_NO_TOOLTIP
        le_vcCleoOutputFolder->setPlaceholderText(QString());
        btn_browseVcCleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        lbl_vcAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "ASI plugins output folder:", nullptr));
#ifndef QT_NO_TOOLTIP
        le_vcCleoSdkFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/CLEO/vc/CLEO_SDK", nullptr));
#endif // QT_NO_TOOLTIP
        le_vcCleoSdkFolder->setPlaceholderText(QString());
        lbl_optionalVcCleoOutput->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        lbl_optionalVcCleoSdk->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        lbl_vcCleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO SDK folder:", nullptr));
#ifndef QT_NO_TOOLTIP
        le_vcAsiOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto - Vice City/scripts", nullptr));
#endif // QT_NO_TOOLTIP
        le_vcAsiOutputFolder->setPlaceholderText(QString());
        lbl_vcCleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO plugins output folder:", nullptr));
        btn_browseVcAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        tabGames->setTabText(tabGames->indexOf(tabVc), QApplication::translate("ProjectWizardClass", "Vice City", nullptr));
        lbl_iiiAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "ASI plugins output folder:", nullptr));
#ifndef QT_NO_TOOLTIP
        le_iiiCleoSdkFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Projects/CLEO/III/CLEO_SDK", nullptr));
#endif // QT_NO_TOOLTIP
        le_iiiCleoSdkFolder->setPlaceholderText(QString());
        btn_browseIIICleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        btn_browseIIIAsiOutputFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        le_iiiCleoOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto III/cleo", nullptr));
#endif // QT_NO_TOOLTIP
        le_iiiCleoOutputFolder->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        le_iiiAsiOutputFolder->setToolTip(QApplication::translate("ProjectWizardClass", "example: D:/Games/Grand Theft Auto III/scripts", nullptr));
#endif // QT_NO_TOOLTIP
        le_iiiAsiOutputFolder->setPlaceholderText(QString());
        lbl_iiiCleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO SDK folder:", nullptr));
        lbl_iiiCleoOutputFolder->setText(QApplication::translate("ProjectWizardClass", "CLEO plugins output folder:", nullptr));
        lbl_optionalIIICleoOutput->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        lbl_optionalIIICleoSdk->setText(QApplication::translate("ProjectWizardClass", "OPTIONAL", nullptr));
        btn_browseIIICleoSdkFolder->setText(QApplication::translate("ProjectWizardClass", "...", nullptr));
        tabGames->setTabText(tabGames->indexOf(tabIII), QApplication::translate("ProjectWizardClass", "GTA III", nullptr));
        btn_useEnvironmentVariables->setText(QApplication::translate("ProjectWizardClass", "Use environment variables", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProjectWizardClass: public Ui_ProjectWizardClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJECTWIZARD_H
