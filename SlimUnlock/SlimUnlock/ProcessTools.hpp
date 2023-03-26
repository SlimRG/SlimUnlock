#ifndef PROCESSTOOLS_HPP
#define PROCESSTOOLS_HPP

#include <QTemporaryDir>
#include <QFile>
#include <QProcess>
#include <QErrorMessage>
#include <QString>
#include <QStringList>
#include <QEventLoop>


#include "FileTools.hpp"

class ProcessTools {
public:
    static QString getLinkFromParser(const QString &exeResourcePath, const QString &url)
    {
        QFile file(exeResourcePath);
        file.open(QIODevice::ReadOnly);
        QTemporaryDir dir;
        QString appName = dir.path() + QDir::separator() + "HuananzhiParser.exe";
        QFile app(appName);
        app.open(QIODevice::WriteOnly);
        app.write(file.readAll());
        file.close();
        app.close();

        QStringList arguments;
        arguments << url;

        QProcess parser;
        parser.start(appName, arguments);
        parser.waitForFinished(60000);
        QString link = parser.readLine().trimmed().replace("\\","/");

        return link;
    }

    static QString executeUnrar2ZIP(const QString& dir, const QString& inArgument) {
        // Resource path and executable name
        const QString resourcePath = ":/Apps/slimrg.exe";
        const QString exeName = "slimrg.exe";

        // Extract the application
        QFile unrar(resourcePath);
        unrar.open(QIODevice::ReadOnly);
        FileTools::saveDataToFile(dir + QDir::separator() + exeName, unrar.readAll());
        unrar.close();

        // Execute the process
        QString appName = dir + QDir::separator() + exeName;
        QString outArgument = inArgument;
        outArgument.replace(".rar", ".zip", Qt::CaseInsensitive);
        QProcess parser;
        parser.setProcessChannelMode(QProcess::MergedChannels);
        parser.start(appName, QStringList() << inArgument << outArgument);
        QEventLoop loop;
        while (parser.waitForFinished() == false) loop.exec();
        return parser.readAll();
    }

    static QString unpack(const QString& dir, const QString& inArgument) {
        // Resource path and executable name
        const QString resourcePath = ":/Apps/slimrg.exe";
        const QString exeName = "slimrg.exe";

        // Extract the application
        QFile unrar(resourcePath);
        unrar.open(QIODevice::ReadOnly);
        FileTools::saveDataToFile(dir + QDir::separator() + exeName, unrar.readAll());
        unrar.close();

        // Execute the process
        QString appName = dir + QDir::separator() + exeName;
        QProcess parser;
        parser.setProcessChannelMode(QProcess::MergedChannels);
        parser.start(appName, QStringList() << inArgument);
        QEventLoop loop;
        while (parser.waitForFinished() == false) loop.exec();
        return parser.readAll();
    }

    static QStringList getCPUInfoFromAMI(const QString &dirPath)
    {
        QString mmtoolPath = (dirPath + QDir::separator() + "mmtool.exe");
        mmtoolPath = mmtoolPath.replace("\\", "/");
        FileTools::saveResourceToFile(":/Apps/mmtool.exe", mmtoolPath);

        QStringList arguments;
        arguments << dirPath + QDir::separator() + "bios.bin" << "/p";

        QProcess parser;
        parser.setProcessChannelMode(QProcess::MergedChannels);
        parser.start(mmtoolPath, arguments);
        parser.waitForStarted();

        if (!parser.waitForFinished(3000))
            parser.kill();

        QString cpuInfos = parser.readAll();
        return cpuInfos.split("\r\n");
    }

    static bool checkCPUID(const QStringList &cpuInfoList)
    {
        QString targetCPUID = "06F2";
        for (const QString &cpuInfo : cpuInfoList)
        {
            if (cpuInfo.contains(targetCPUID, Qt::CaseInsensitive))
            {
                return true;
            }
        }
        return false;
    }

    static bool updateMicrocode(const QString &dirPath, const QStringList &cpuInfoList)
    {
        QProcess parser;
        QStringList arguments;
        QString mmtoolPath = dirPath + QDir::separator() + "mmtool.exe";
        mmtoolPath = mmtoolPath.replace("\\", "/");
        QString targetCPUID = "06F2";

        for (const QString &cpuInfo : cpuInfoList)
        {
            if (cpuInfo.contains(targetCPUID))
            {
                auto Vol = cpuInfo.section('|', 1, 1).trimmed();
                auto No = cpuInfo.section('|', 2, 2).trimmed();
                auto CPUID = cpuInfo.section('|', 6, 6).trimmed();

                if (CPUID != targetCPUID)
                    continue;

                arguments.clear();
                arguments << dirPath + QDir::separator() + "bios.bin" << "/d" << "/p" << No << Vol;

                parser.close();
                parser.start(mmtoolPath, arguments);
                if (!parser.waitForFinished())
                    parser.kill();

                QString log = parser.readAll();
                if (log.trimmed() == "Image rebuilding is on Progress...")
                {
                    parser.close();
                    return true;
                }
            }
        }
        parser.close();
        return false;
    }

    static bool executeAMI(const QString &dirPath)
    {
        QString amiName = dirPath + QDir::separator() + "ami.exe";
        amiName = amiName.replace("\\", "/");

        if (!FileTools::saveResourceToFile(":/Apps/ami.exe", amiName))
            return false;

        QProcess parser;
        QStringList arguments;
        QString biosBinPath = dirPath + QDir::separator() + "bios.bin";
        arguments << biosBinPath;

        parser.setProcessChannelMode(QProcess::MergedChannels);
        parser.start(amiName, arguments);
        bool started = parser.waitForStarted();
        if (!started) {
            return false;
        }
//        QEventLoop loop;
//        if (!parser.waitForFinished(3000))
//            parser.kill();
        return true;
    }
};

#endif // PROCESSTOOLS_HPP
