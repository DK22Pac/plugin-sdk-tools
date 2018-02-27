#include "ErrorMessage.h"
#include "BorderlessStyle.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <Windows.h>

MessageDialog::MessageDialog(QWidget * parent) : QDialog(parent) {
    SetWidgetBorderless(this);
    ui.setupUi(this);
    connect(ui.btn_close, &QPushButton::clicked, this, &MessageDialog::on_closeButtonClicked);
    connect(ui.btn_ok, &QPushButton::clicked, this, &MessageDialog::on_closeButtonClicked);
}

void MessageDialog::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    if (!event->isAccepted()) {
        ReleaseCapture();
        SendMessage(reinterpret_cast<HWND>(winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
    }
}

void MessageDialog::on_closeButtonClicked(bool checked) {
    close();
}

void MessageDialog::Setup(QString title, QString text) {
    ui.lbl_title->setText(title);
    ui.lbl_text->setText(text);
}

bool MESSAGE_ERROR(QString message) {
    MessageDialog msgBox;
    msgBox.Setup("Error", message);
    msgBox.exec();
    return false;
}

bool MESSAGE_WARNING(QString message) {
    MessageDialog msgBox;
    msgBox.Setup("Warning", message);
    msgBox.exec();
    return false;
}

bool MESSAGE_INFO(QString message) {
    MessageDialog msgBox;
    msgBox.Setup("Info", message);
    msgBox.exec();
    return false;
}
