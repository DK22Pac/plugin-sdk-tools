#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QtWidgets/QMainWindow>
#include "ui_projectwizard.h"
#include "settings.h"

class ProjectWizard : public QMainWindow
{
    Q_OBJECT

public:
    ProjectWizard(QWidget *parent = 0);
    ~ProjectWizard();

private:
    Ui::ProjectWizardClass ui;
    AppSettings settings;

    bool isPointInsideWindowArea(QPoint point);

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void changeEvent(QEvent *event) override;

    void on_closeButtonClicked(bool checked);
    void on_minimizeButtonClicked(bool checked);

    void on_browsePluginSdkFolderButtonClicked(bool checked);
    void on_browseRwD3d9FolderButtonClicked(bool checked);
    void on_browseDirectXS9dkFolderButtonClicked(bool checked);
    void on_browseVisualStudioDocumentsFolderButtonClicked(bool checked);
    void on_browseCodeBlocksFolderButtonClicked(bool checked);
    void on_browseSaAsiPluginsFolderButtonClicked(bool checked);
    void on_browseSaCleoSdkFolderButtonClicked(bool checked);
    void on_browseSaCleoPluginsFolderButtonClicked(bool checked);
    void on_browseVcAsiPluginsFolderButtonClicked(bool checked);
    void on_browseVcCleoSdkFolderButtonClicked(bool checked);
    void on_browseVcCleoPluginsFolderButtonClicked(bool checked);
    void on_browseIIIAsiPluginsFolderButtonClicked(bool checked);
    void on_browseIIICleoSdkFolderButtonClicked(bool checked);
    void on_browseIIICleoPluginsFolderButtonClicked(bool checked);

    void on_useEnvironmentVariablesButtonClicked(bool checked);
    void on_generateButtonClicked(bool checked);

    bool bInsideWindowAreaWhenMousePressed;
    QPoint dragPosition;
};

#endif // PROJECTWIZARD_H
