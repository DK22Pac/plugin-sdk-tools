#pragma once
#include <QWidget>
#include <QAbstractNativeEventFilter>

void SetWidgetBorderless(QWidget *widget);

class NativeEventFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
};