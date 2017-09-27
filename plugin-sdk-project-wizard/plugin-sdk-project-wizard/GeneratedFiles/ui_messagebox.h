/********************************************************************************
** Form generated from reading UI file 'messagebox.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEBOX_H
#define UI_MESSAGEBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dialogWidget
{
public:
    QLabel *lbl_title;
    QPushButton *btn_close;
    QLabel *lbl_icon;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *lbl_text;
    QPushButton *btn_ok;

    void setupUi(QDialog *dialogWidget)
    {
        if (dialogWidget->objectName().isEmpty())
            dialogWidget->setObjectName(QStringLiteral("dialogWidget"));
        dialogWidget->resize(351, 113);
        dialogWidget->setStyleSheet(QLatin1String("QMenu {\n"
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
        lbl_title = new QLabel(dialogWidget);
        lbl_title->setObjectName(QStringLiteral("lbl_title"));
        lbl_title->setGeometry(QRect(49, 7, 261, 16));
        btn_close = new QPushButton(dialogWidget);
        btn_close->setObjectName(QStringLiteral("btn_close"));
        btn_close->setGeometry(QRect(317, 0, 34, 26));
        lbl_icon = new QLabel(dialogWidget);
        lbl_icon->setObjectName(QStringLiteral("lbl_icon"));
        lbl_icon->setGeometry(QRect(14, 6, 22, 22));
        verticalLayoutWidget = new QWidget(dialogWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 40, 333, 61));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lbl_text = new QLabel(verticalLayoutWidget);
        lbl_text->setObjectName(QStringLiteral("lbl_text"));
        lbl_text->setMinimumSize(QSize(331, 21));
        lbl_text->setMaximumSize(QSize(331, 16777215));
        lbl_text->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_text->setWordWrap(true);

        verticalLayout->addWidget(lbl_text);

        btn_ok = new QPushButton(verticalLayoutWidget);
        btn_ok->setObjectName(QStringLiteral("btn_ok"));
        btn_ok->setMinimumSize(QSize(100, 26));
        btn_ok->setMaximumSize(QSize(100, 26));
        btn_ok->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(btn_ok);


        retranslateUi(dialogWidget);

        QMetaObject::connectSlotsByName(dialogWidget);
    } // setupUi

    void retranslateUi(QDialog *dialogWidget)
    {
        dialogWidget->setWindowTitle(QApplication::translate("dialogWidget", "Message", nullptr));
        lbl_title->setText(QApplication::translate("dialogWidget", "Message", nullptr));
        btn_close->setText(QString());
        lbl_icon->setText(QString());
        lbl_text->setText(QString());
        btn_ok->setText(QApplication::translate("dialogWidget", "Ok", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dialogWidget: public Ui_dialogWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEBOX_H
