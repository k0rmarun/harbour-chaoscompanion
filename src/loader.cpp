#include "loader.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>

Loader::Loader(QObject *parent) : QObject(parent)
{
    mBaseDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    mCacheFilename = mBaseDir + "/cache.json";
    mURLFilename = mBaseDir + "/url";
    qDebug() << "filename" << mCacheFilename;

    nam = new QNetworkAccessManager(this);
    mLoading = false;
    emit loadingChanged(mLoading);
    if(!loadFromDisk()){
        loadFromNetwork();
    }
}

bool Loader::loadFromDisk(){
    QByteArray conferenceData = read(mCacheFilename);
    if(conferenceData.isEmpty()){
        qDebug() << "LOAD FROM DISK FAILED";
        return false;
    }
    loadJSON(conferenceData);
    return true;
}

void Loader::loadFromNetwork(){
    mLoading = true;
    emit loadingChanged(mLoading);
    qDebug() << "Loading" << mURL << "from network";

    connect(nam, &QNetworkAccessManager::finished,
           this, &Loader::RequestFinished);
    nam->get(QNetworkRequest(mURL));
}

void Loader::setCurrent(const QString& cacheTitle, const QString &url) {
    qDebug() << "aaaaa";
    bool changed = mURL == url;
    if(!url.isEmpty()){
        mURL = url;
    }
    qDebug() << "bbbbb" << changed << mURL << url;
    if(changed || !loadFromDisk()){
        qDebug() << "ccccc";
        loadFromNetwork();
    }
}

void Loader::RequestFinished(QNetworkReply *reply){
    if(reply->error() == QNetworkReply::NoError){
        QByteArray conferenceData = reply->readAll();
        if(conferenceData.isEmpty()){
            emit loadFromNetworkFailed();
            qDebug() << "LOAD FROM NETWORK FAILED" << reply->url();
            return;
        }

        loadJSON(conferenceData);
        write(mCacheFilename, conferenceData);
        write(mURLFilename, mURL.toUtf8());
        qDebug() << "LOAD FROM NETWORK SUCCEEDED";
    } else {
        emit loadFromNetworkFailed();
        qDebug() << reply;
        qDebug() << "LOAD FROM NETWORK FAILED";
    }

    mLoading = false;
    emit loadingChanged(mLoading);
}

void Loader::write(const QString& filename, const QByteArray &data){
    QFile file(filename);
    if(!file.exists()){
        QDir dir = QDir::home();
        dir.mkpath(mBaseDir);
    }
    if(file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        file.write(data);
        file.close();
    }
}

QByteArray Loader::read(const QString& filename){
    QByteArray ret;
    QFile file(filename);
    if(file.exists()){
        file.open(QIODevice::ReadOnly);
        ret = file.readAll();
        file.close();
    }
    return ret;
}

void Loader::loadJSON(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    mConference = Conference::fromJson(doc.object());
    emit conferenceChanged(mConference);
}

