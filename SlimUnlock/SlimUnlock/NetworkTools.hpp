#ifndef NETWORKTOOLS_HPP
#define NETWORKTOOLS_HPP

#include <QMessageBox>
#include <QEventLoop>
#include <QException>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QMap>
#include <QTextDocument>
#include <QErrorMessage>
#include <QRegularExpression>
#include <QUrlQuery>

class NetworkTools {

public:

    static QMap<QString, QString>* getMbNames(const QString &html)
    {
        if (html.isEmpty())
            return nullptr;

        // Разбор сайта
        int pos1 = html.indexOf("var keyWords = [");
        pos1 = html.indexOf("[[", pos1)+2;
        int pos2 = html.indexOf("]]", pos1);

        if (pos1 <= -1 || pos2 <= -1)
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage("Парсинг сайта неудачен, свяжитесь со мной, Telegram: @slimrg \n The parsing of the site is unsuccessful, contact me, Telegram: @slimrg");
            errorMessage.exec();
            return nullptr;
        }

        QString motherBoardsDirty = html.mid(pos1, pos2-pos1);
        auto motherBoardsArray = motherBoardsDirty.split("],[");

        auto result = new QMap<QString, QString>();
        foreach (const QString &str, motherBoardsArray) {
            auto mbInfo = str.split(',');
            if (mbInfo[1] != "10" || mbInfo[0].contains("X99") == false)
                continue;

            QString name = mbInfo[0].mid(1, mbInfo[0].length()-2);
            name = name.replace("MOTHERBOARD", "", Qt::CaseInsensitive).trimmed();
            (*result)[mbInfo[2]] = name;
        }

        return result;
    }

    static QString download(const QUrl &url)
    {
        if (url.isEmpty())
        {
            QMessageBox::warning(
                NULL,
                "Внимание! / Attention!",
                "[System] URL is empty");

            return "";
        }

        QNetworkAccessManager nam;
        QNetworkRequest req;
        QEventLoop loop;

        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

        req.setUrl(url);
        nam.setTransferTimeout(5000);
        QNetworkReply* reply = nam.get(req);

        while (!reply->isFinished() || reply->isRunning())
        {
            loop.exec();
        }

        if(!reply)
        {
            QMessageBox::warning(
                NULL,
                "Внимание! / Attention!",
                "Парсинг сайта неудачен: вылет по таймауту \n Site parsing failed: timeout failure");

            return "";
        }
        else if(QNetworkReply::NoError != reply->error())
        {
            QMessageBox::warning(
                NULL,
                "Внимание! / Attention!",
                "Парсинг сайта неудачен: / Site parsing failed: \n" + reply->errorString());

            throw reply->error();
            delete reply;
            return "";
        }
        else
        {
            QString result(reply->readAll());
            delete reply;
            return result;
        }
    };

    static QByteArray downloadWithProgress(const QUrl &url, QObject *receiver, const char *progressSlot)
    {
        QByteArray data;
        char tries = 0;
        do
        {
            QNetworkAccessManager nam;
            nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
            QNetworkRequest req;
            QNetworkReply *reply = nullptr;
            QEventLoop loop;
            QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

            req.setUrl(url);
            reply = nam.get(req);
            QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
            QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), receiver, progressSlot);

            while (!reply->isFinished() || reply->isRunning()) loop.exec();
            data.append(reply->readAll());
            delete reply;

        } while (!data.length() && tries < 100);
        return data;
    }
};

#endif // NETWORKTOOLS_HPP
