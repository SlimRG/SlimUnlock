#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->siteInfoCmb->setVisible(false);
    ui->saveBtn->setVisible(false);
    ui->patchBtn->setVisible(false);
    ui->dwnBiosPb->setVisible(false);

    ui->mmtoolCb->setVisible(false);
    ui->mmtoolCb->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->mmtoolCb->setFocusPolicy(Qt::NoFocus);

    ui->amiCb->setVisible(false);
    ui->amiCb->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->amiCb->setFocusPolicy(Qt::NoFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MakeMbList(QMap<QString, QString>* map)
{
    if (!map)
    {
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setVisible(false);
        return;
    }

    if (map->count() > 0)
        ui->siteInfoCmb->clear();

    foreach (const QString &key, map->keys())
    {
        ui->siteInfoCmb->addItem((*map)[key], key);
    }

    ui->siteInfoCmb->model()->sort(0);
    ui->siteInfoCmb->setCurrentIndex(0);
    ui->updateSiteInfoBtn->setEnabled(true);
    ui->openFileBtn->setEnabled(true);
    ui->siteInfoCmb->setVisible(true);
    delete map;
}

void MainWindow::on_updateSiteInfoBtn_clicked()
{
    ui->updateSiteInfoBtn->setEnabled(false);
    ui->openFileBtn->setEnabled(false);
    ui->saveBtn->setVisible(false);
    ui->patchBtn->setVisible(false);

    const QUrl productUrl("http://www.huananzhi.com/en/product.php?lm=10");

    QtConcurrent::run([](const QUrl &url) { return NetworkTools::download(url); }, productUrl)
            .then([](const QString &html) { return NetworkTools::getMbNames(html); })
            .then([this](QMap<QString, QString>* map) {
        QMetaObject::invokeMethod(this, [this, map] {
            MakeMbList(map);
        }, Qt::QueuedConnection);
        return map;
    });
}

void MainWindow::on_openFileBtn_clicked()
{
    ui->updateSiteInfoBtn->setEnabled(false);
    ui->openFileBtn->setEnabled(false);
    ui->saveBtn->setVisible(false);
    ui->patchBtn->setVisible(false);

    QString filePath = QFileDialog::getOpenFileName(this, tr("Open BIOS"), "", tr("Binary Files (*.*)"));

    if (!filePath.isEmpty())
    {
        ui->siteInfoCmb->clear();

        QString fileName = QFileInfo(filePath).fileName();
        ui->siteInfoCmb->addItem(fileName, filePath);

        ui->siteInfoCmb->model()->sort(0);
        ui->siteInfoCmb->setCurrentIndex(0);
        ui->siteInfoCmb->setMinimumWidth(ui->siteInfoCmb->sizeHint().width());
        ui->siteInfoCmb->setVisible(true);
    }
    ui->openFileBtn->setEnabled(true);
    ui->updateSiteInfoBtn->setEnabled(true);
}

void MainWindow::on_siteInfoCmb_currentTextChanged(const QString &str)
{
    if (str == "Требуется обновление / Needs update")
    {
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        return;
    }

    if (ui->siteInfoCmb->count() == 0)
    {
        ui->siteInfoCmb->setVisible(false);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        return;
    }

    ui->saveBtn->setVisible(true);
    //ui->patchBtn->setVisible(true);
}

void MainWindow::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    ui->dwnBiosPb->setMaximum(totalBytes);
    ui->dwnBiosPb->setValue(readBytes);
}

void MainWindow::on_saveBtn_clicked()
{
    ui->updateSiteInfoBtn->setEnabled(false);
    ui->openFileBtn->setEnabled(false);
    ui->siteInfoCmb->setEnabled(false);
    ui->saveBtn->setEnabled(false);
    ui->patchBtn->setEnabled(false);

    QVariant selectedData = ui->siteInfoCmb->itemData(ui->siteInfoCmb->currentIndex());
    if (!selectedData.canConvert<QString>()) {

        QMessageBox::warning(
            NULL,
            "Внимание! / Attention!",
            "[System] Выбранный параметр кривой / Selected string is broken");

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }
    QString selectedString = selectedData.toString();

    QRegExp regex("[a-zA-Z0-9-]+");
    bool isLocal = false;
    if (regex.exactMatch(selectedString))
    {
        isLocal = false;
    }
    else if (QFile::exists(selectedString))
    {
        isLocal = true;
    }
    else
    {
        QMessageBox::warning(
            NULL,
            "Внимание! / Attention!",
            "[System] Выбранный параметр кривой (2) / Selected string is broken (2)");

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    QString ext = selectedString.mid(
                selectedString.lastIndexOf('.'),
                selectedString.length()-selectedString.lastIndexOf('.'));

    if (isLocal == false)
        ext = ".RAR";

    QString fname = (ext.toUpper() == ".ZIP" || ext.toUpper() == ".RAR")
            ? QFileDialog::getSaveFileName(nullptr, "Сохранить/Save BIOS", nullptr, "BIOS (*.zip)")
            :  QFileDialog::getSaveFileName(nullptr, "Сохранить/Save BIOS", nullptr, "BIOS (*"+ext+")");

    if (fname == "")
    {
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    if (ext.toUpper() == ".RAR")
        fname = fname.replace(".RAR", ".zip", Qt::CaseInsensitive);

    if (!isLocal && !fname.endsWith(".zip", Qt::CaseInsensitive))
        fname += ".zip";

    QTemporaryDir dir;
    QString outFilePath = dir.path() + QDir::separator() + "1" + ext;
    if (isLocal)
    {
        if (!FileTools::copyFile(selectedString, outFilePath))
        {
            QMessageBox::warning(
                        NULL,
                        "Внимание! / Attention!",
                        "[System] Копирование файла неудачно / Copying the file failed");
            ui->updateSiteInfoBtn->setEnabled(true);
            ui->openFileBtn->setEnabled(true);
            ui->siteInfoCmb->setEnabled(true);
            ui->saveBtn->setVisible(false);
            ui->patchBtn->setVisible(false);
            ui->saveBtn->setEnabled(true);
            ui->patchBtn->setEnabled(true);
            return;
        }
    }
    else
    {
        QString url = "http://www.huananzhi.com/en/more.php?lm=10&id=" + selectedString;
        QString link = ProcessTools::getLinkFromParser(":/Apps/HuananzhiParser.exe", url);

        ext = link.mid(
                    link.lastIndexOf('.'),
                    link.length()-link.lastIndexOf('.'));

        if (!link.contains("http://www.huananzhi.com/attach/"))
        {
            QMessageBox::warning(
                        NULL,
                        "Внимание! / Attention!",
                        "[System] Парсинг сайта неудачен (2) / Copying the file failed");
            ui->updateSiteInfoBtn->setEnabled(true);
            ui->openFileBtn->setEnabled(true);
            ui->siteInfoCmb->setEnabled(true);
            ui->saveBtn->setVisible(false);
            ui->patchBtn->setVisible(false);
            ui->saveBtn->setEnabled(true);
            ui->patchBtn->setEnabled(true);
            return;
        }

        ui->dwnBiosPb->setVisible(true);
        QByteArray data = NetworkTools::downloadWithProgress(link, this, SLOT(updateDataTransferProgress(qint64,qint64)));
        FileTools::saveDataToFile(outFilePath, data);
        ui->dwnBiosPb->setVisible(false);
    }

    if (ext.toUpper() == ".RAR") {

        QString answr = ProcessTools::executeUnrar2ZIP(dir.path(), outFilePath);
        if (!answr.isEmpty())
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage(answr);
            errorMessage.exec();

            ui->updateSiteInfoBtn->setEnabled(true);
            ui->openFileBtn->setEnabled(true);
            ui->siteInfoCmb->setEnabled(true);
            ui->saveBtn->setVisible(false);
            ui->patchBtn->setVisible(false);
            ui->saveBtn->setEnabled(true);
            ui->patchBtn->setEnabled(true);
            return;
        }
    }

    if (QFile::exists(fname))
        QFile::remove(fname);

    outFilePath.replace(".rar", ".zip", Qt::CaseInsensitive);
    if (!QFile::copy(outFilePath, fname))
    {
        QMessageBox::warning(
            NULL,
            "Внимание! / Attention!",
            "[System] Файл назначения недоступен / The destination file is not available");

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    ui->updateSiteInfoBtn->setEnabled(true);
    ui->openFileBtn->setEnabled(true);
    ui->siteInfoCmb->setEnabled(true);
    ui->saveBtn->setEnabled(true);
    ui->patchBtn->setEnabled(true);
}

void MainWindow::on_patchBtn_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("ВНИМАНИЕ! / Attention!");
    msgBox.setInformativeText("Автор не несет ответственности за ваш PC! \n The author is not responsible for your PC!");
    msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Apply);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Apply)
        return;

    ui->updateSiteInfoBtn->setEnabled(false);
    ui->openFileBtn->setEnabled(false);
    ui->siteInfoCmb->setEnabled(false);
    ui->saveBtn->setEnabled(false);
    ui->patchBtn->setEnabled(false);

    QVariant selectedData = ui->siteInfoCmb->itemData(ui->siteInfoCmb->currentIndex());
    if (!selectedData.canConvert<QString>()) {

        QMessageBox::warning(
            NULL,
            "Внимание! / Attention!",
            "[System] Выбранный параметр кривой / Selected string is broken");

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }
    QString selectedString = selectedData.toString();

    QRegExp regex("[a-zA-Z0-9-]+");
    bool isLocal = false;
    if (regex.exactMatch(selectedString))
    {
        isLocal = false;
    }
    else if (QFile::exists(selectedString))
    {
        isLocal = true;
    }
    else
    {
        QMessageBox::warning(
            NULL,
            "Внимание! / Attention!",
            "[System] Выбранный параметр кривой (2) / Selected string is broken (2)");

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    QString ext = selectedString.mid(
                selectedString.lastIndexOf('.'),
                selectedString.length()-selectedString.lastIndexOf('.'));

    if (isLocal == false)
        ext = ".RAR";

    QString fname = (ext.toUpper() == ".ZIP" || ext.toUpper() == ".RAR")
            ? QFileDialog::getSaveFileName(nullptr, "Сохранить/Save BIOS", nullptr, "BIOS (*.zip)")
            :  QFileDialog::getSaveFileName(nullptr, "Сохранить/Save BIOS", nullptr, "BIOS (*"+ext+")");

    if (fname == "")
    {
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    if (ext.toUpper() == ".RAR")
        fname = fname.replace(".RAR", ".zip", Qt::CaseInsensitive);

    if (!isLocal && !fname.endsWith(".zip", Qt::CaseInsensitive))
        fname += ".zip";

    QTemporaryDir dir;
    QString outFilePath = dir.path() + QDir::separator() + "1" + ext;
    if (isLocal)
    {
        if (!FileTools::copyFile(selectedString, outFilePath))
        {
            QMessageBox::warning(
                        NULL,
                        "Внимание! / Attention!",
                        "[System] Копирование файла неудачно / Copying the file failed");
            ui->updateSiteInfoBtn->setEnabled(true);
            ui->openFileBtn->setEnabled(true);
            ui->siteInfoCmb->setEnabled(true);
            ui->saveBtn->setVisible(false);
            ui->patchBtn->setVisible(false);
            ui->saveBtn->setEnabled(true);
            ui->patchBtn->setEnabled(true);
            return;
        }
    }
    else
    {
        QString url = "http://www.huananzhi.com/en/more.php?lm=10&id=" + selectedString;
        QString link = ProcessTools::getLinkFromParser(":/Apps/HuananzhiParser.exe", url);

        ext = link.mid(
                    link.lastIndexOf('.'),
                    link.length()-link.lastIndexOf('.'));

        if (!link.contains("http://www.huananzhi.com/attach/"))
        {
            QMessageBox::warning(
                        NULL,
                        "Внимание! / Attention!",
                        "[System] Парсинг сайта неудачен (2) / Copying the file failed");
            ui->updateSiteInfoBtn->setEnabled(true);
            ui->openFileBtn->setEnabled(true);
            ui->siteInfoCmb->setEnabled(true);
            ui->saveBtn->setVisible(false);
            ui->patchBtn->setVisible(false);
            ui->saveBtn->setEnabled(true);
            ui->patchBtn->setEnabled(true);
            return;
        }

        ui->dwnBiosPb->setVisible(true);
        QByteArray data = NetworkTools::downloadWithProgress(link, this, SLOT(updateDataTransferProgress(qint64,qint64)));
        FileTools::saveDataToFile(outFilePath, data);
        ui->dwnBiosPb->setVisible(false);
    }

    QString answr = ProcessTools::unpack(dir.path(), outFilePath);
    if (!answr.isEmpty())
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage(answr);
        errorMessage.exec();

        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        return;
    }

    if (QFile::exists(fname))
        QFile::remove(fname);

    // Удаление микрокода
    ui->mmtoolCb->setCheckState(Qt::Unchecked);
    ui->mmtoolCb->setVisible(true);

    QStringList cpuInfoList = ProcessTools::getCPUInfoFromAMI(dir.path());
    bool isCPUIDPresent = ProcessTools::checkCPUID(cpuInfoList);
    if (!isCPUIDPresent)
    {
        QMessageBox::warning(
                    NULL,
                    "Внимание! / Attention!",
                    "[System] Микрокод не найден / Microcode not found");
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        ui->mmtoolCb->setVisible(false);
        return;
    }

    bool isSuccess = ProcessTools::updateMicrocode(dir.path(), cpuInfoList);
    if (!isSuccess)
    {
        QMessageBox::warning(
                    NULL,
                    "Внимание! / Attention!",
                    "[System] Ошибка при удалении микрокода / Error deleting microcode");
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        ui->mmtoolCb->setVisible(false);
        return;
    }
    ui->mmtoolCb->setCheckState(Qt::Checked);

    // Устанавливаем настройки BIOS
    ui->amiCb->setCheckState(Qt::Unchecked);
    ui->amiCb->setVisible(true);

    bool result = ProcessTools::executeAMI(dir.path());
    if (!result) {
        QMessageBox::warning(
                    NULL,
                    "Внимание! / Attention!",
                    "[System] Ошибка при удалении микрокода / Error deleting microcode");
        ui->updateSiteInfoBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(true);
        ui->siteInfoCmb->setEnabled(true);
        ui->saveBtn->setVisible(false);
        ui->patchBtn->setVisible(false);
        ui->saveBtn->setEnabled(true);
        ui->patchBtn->setEnabled(true);
        ui->mmtoolCb->setVisible(false);
        ui->amiCb->setVisible(false);
        return;
    }



    //QEventLoop loop;
//    if (!parser.waitForFinished(3000))
//        parser.kill();

//    QString cpuInfos = parser.readAll();
//    QStringList cpuInfoList = cpuInfos.split("\r\n");

//    bool isSuccess = false;
//    foreach (const QString &cpuInfo, cpuInfoList)
//    {
//        if (cpuInfo.contains("06F2"))
//        {
//            auto Vol = cpuInfo.section('|', 1, 1).trimmed();
//            auto No = cpuInfo.section('|', 2, 2).trimmed();
//            auto Boot = cpuInfo.section('|', 3, 3).trimmed();
//            auto MicroCodeID = cpuInfo.section('|', 4, 4).trimmed();
//            auto Platform = cpuInfo.section('|', 5, 5).trimmed();
//            auto CPUID = cpuInfo.section('|', 6, 6).trimmed();
//            auto Revision = cpuInfo.section('|', 7, 7).trimmed();

//            if (CPUID != "06F2")
//                continue;

//            arguments.clear();
//            arguments << dir.path() + QDir::separator() + "bios.bin" << "/d" << "/p" << No << Vol;

//            parser.close();
//            parser.start(mmtoolName, arguments);
//            if (!parser.waitForFinished())
//                parser.kill();

//            QString log = parser.readAll();
//            if (log.trimmed() == "Image rebuilding is on Progress...")
//            {
//                isSuccess = true;
//                break;
//            }
//        }
//    }

//    if (!isSuccess)
//    {
//        QErrorMessage errorMessage;
//        errorMessage.showMessage("CPUID неудачен, свяжитесь со мной, Telegram: @slimrg \n CPUID is unsuccessful, contact me, Telegram: @slimrg");
//        errorMessage.exec();

//        ui->dwnBiosPb->setVisible(false);
//        ui->saveBtn->setEnabled(true);
//        ui->patchBtn->setEnabled(true);
//        ui->siteInfoCmb->setEnabled(true);
//        ui->mmtoolCb->setVisible(false);
//        return;
//    }

//    ui->mmtoolCb->setCheckState(Qt::Checked);

}


