#pragma once
#include <QString>
#include <QWidget>
#include "ui_messagebox.h"

bool MESSAGE_ERROR(QString message);
bool MESSAGE_WARNING(QString message);
bool MESSAGE_INFO(QString message);

class MessageDialog : public QDialog {
    Q_OBJECT
public:
    MessageDialog(QWidget * parent = 0);
    void Setup(QString title, QString text);
private:
    void mousePressEvent(QMouseEvent* event) override;
    void on_closeButtonClicked(bool checked);
    Ui::dialogWidget ui;
};
