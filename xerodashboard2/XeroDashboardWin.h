#pragma once

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QPlainTextEdit>
#include <networktables/NetworkTableInstance.h>
#include "ValueDisplayWidget.h"
#include "PlotManager.h"
#include "PlotSelectWidget.h"
#include "DashView.h"

class XeroDashboardWin : public QMainWindow
{
    Q_OBJECT

public:
    XeroDashboardWin(QWidget *parent = nullptr);
    ~XeroDashboardWin();

    static QString versionString();

signals:
    void logMessage(QString msg);

protected:
    void showEvent(QShowEvent* ev) override;
    void closeEvent(QCloseEvent* ev) override;

private:
    static constexpr const char* GeometrySetting = "geometry";
    static constexpr const char* WindowStateSetting = "windowState";
    static constexpr const char* NTServerIPSetting = "NTServerIP";

private:
    void createWindows();
    void createMenus();
    void startNetworkTables(const QString& ipaddr, const QString& name);

    void listenNotify(const nt::Event& ev);
    void timerHandler();

    void putMessageInLogWindow(QString msg) {
        logwin_->appendPlainText(msg);
    }

private:
    QSettings settings_;

    DashView* dashboard_view_;

    ValueDisplayWidget* data_win_;
    PlotSelectWidget* plot_win_;
    QPlainTextEdit* logwin_;

    QDockWidget* data_win_dock_;
    QDockWidget* plot_win_dock_;
    QDockWidget* log_win_dock_;

    QMenu* file_menu_;
    QMenu* window_menu_;

    PlotManager* plots_;

    QString name_;
    QString ipaddr_;
    nt::NetworkTableInstance inst_;
    NT_Listener listener_;

    QTimer *timer_;

    QStringList storedLogLines_;
};
