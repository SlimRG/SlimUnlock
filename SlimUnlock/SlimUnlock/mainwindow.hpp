#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QFuture>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QProcess>
#include <QThread>
#include <QSystemTrayIcon>
#include <QMutex>
#include <QFileDialog>
#include <QErrorMessage>
#include <QTimer>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QMap>
#include <QRegExp>
#include <QtConcurrent>

#include "NetworkTools.hpp"
#include "FileTools.hpp"
#include "ProcessTools.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString link;

private slots:

    void MakeMbList(QMap<QString, QString>* map);
    void on_updateSiteInfoBtn_clicked();

    void on_saveBtn_clicked();

    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    void on_patchBtn_clicked();

    void on_openFileBtn_clicked();

    void on_siteInfoCmb_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QFutureWatcher<QMap<QString, QString>*> watcher;
};
#endif // MAINWINDOW_HPP
