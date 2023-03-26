#ifndef FILETOOLS_HPP
#define FILETOOLS_HPP

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

class FileTools
{
public:
    static bool copyFile(const QString& sourceFilePath, const QString& destinationFilePath)
    {
        QFile sourceFile(sourceFilePath);
        if (!sourceFile.exists() || !sourceFile.open(QIODevice::ReadOnly))
        {
            return false;
        }

        QFile destinationFile(destinationFilePath);
        if (destinationFile.exists() && !destinationFile.remove())
        {
            return false;
        }
        if (!destinationFile.open(QIODevice::WriteOnly))
        {
            return false;
        }

        while (!sourceFile.atEnd())
        {
            QByteArray data = sourceFile.read(8192);
            destinationFile.write(data);
        }

        sourceFile.close();
        destinationFile.close();

        QFileInfo sourceFileInfo(sourceFilePath);
        QFile::setPermissions(destinationFilePath, sourceFileInfo.permissions());

        return true;
    }

    static bool saveDataToFile(const QString &outFilePath, const QByteArray &data)
    {
        QFile file(outFilePath);
        if (!file.open(QIODevice::WriteOnly))
            return false;
        bool result = file.write(data);
        file.close();
        return result;
    }

    static bool saveResourceToFile(const QString &resourcePath, const QString &destinationFilePath)
    {
        QFile resourceFile(resourcePath);
        if (!resourceFile.open(QIODevice::ReadOnly))
            return false;

        QByteArray data = resourceFile.readAll();
        bool result = saveDataToFile(destinationFilePath, data);
        resourceFile.close();
        return result;
    }

};

#endif // FILETOOLS_HPP
