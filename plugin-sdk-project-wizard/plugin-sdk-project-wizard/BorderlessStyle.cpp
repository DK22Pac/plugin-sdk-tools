#include "BorderlessStyle.h"
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

void SetWidgetBorderless(QWidget *widget) {
    widget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());
    const LONG style = (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    const MARGINS shadow = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &shadow);
    SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

bool NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
    if (eventType != "windows_generic_MSG")
        return false;
    MSG* msg = static_cast<MSG*>(message);
    QWidget* widget = QWidget::find(reinterpret_cast<WId>(msg->hwnd));
    if (!widget)
        return false;
    switch (msg->message) {
    case WM_NCCALCSIZE:
        *result = 1;
        return true;
    case WM_NCHITTEST: {
        const LONG borderWidth = 0;
        RECT winrect;
        GetWindowRect(msg->hwnd, &winrect);
        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);
        // bottom left
        if (x >= winrect.left && x < winrect.left + borderWidth &&
            y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOMLEFT;
            return true;
        }
        // bottom right
        if (x < winrect.right && x >= winrect.right - borderWidth &&
            y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOMRIGHT;
            return true;
        }
        // top left
        if (x >= winrect.left && x < winrect.left + borderWidth &&
            y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOPLEFT;
            return true;
        }
        // top right
        if (x < winrect.right && x >= winrect.right - borderWidth &&
            y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOPRIGHT;
            return true;
        }
        // left
        if (x >= winrect.left && x < winrect.left + borderWidth)
        {
            *result = HTLEFT;
            return true;
        }
        // right
        if (x < winrect.right && x >= winrect.right - borderWidth)
        {
            *result = HTRIGHT;
            return true;
        }
        // bottom
        if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOM;
            return true;
        }
        // top
        if (y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOP;
            return true;
        }
        return false;
    }
    default:
        break;
    }
    return false;
}