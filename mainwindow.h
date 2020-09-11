#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QtAndroidExtras/QtAndroid>

#include "libs/armeabi-v7a/qFisGoPirit/PiritLib.h"

#include "loggerdaemon.h"

#include "kf_files/https_exchange.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum OUTMODE
{
  NORMAL_T,
  GREEN_T
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pbConnect_clicked();

    void on_pbScroll_clicked();

    void on_pbPrintSDoc_clicked();

    void on_pbXReport_clicked();

    void on_pbOpenShift_clicked();

    void on_pbReciept_clicked();

    void on_pushButton_clicked();

    void on_pbStartExchange_clicked();

private:
    Ui::MainWindow *ui;
    LoggerDaemon *loggerDaemon;

    Config config_data;
    HttpsExchange he;
    JSON::JSON_POLL_ json_poll; // Буфер данных для заданий


    void OutText(const QString& str, OUTMODE mode = NORMAL_T);
    bool requestAndroidPermissions();

    void runLogDaemon();

    bool sendHandshakeToHub();
    void sendPollToHub();
};
#endif // MAINWINDOW_H
