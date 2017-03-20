#ifndef LFILEDATE_API_H
#define LFILEDATE_API_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <QDateTime>
#include <QFileInfo>
#include <QFile>
#include <QString>
#include <QDebug>

#ifndef Q2CH
    #define Q2CH toUtf8().data()
#endif

#define DT_FORMAT "yyyyMMddhhmmss"
#define TMS_FORMAT "hhmmsszzz"
class LFileDate
{
public:
    LFileDate();
    void setFile(QString);
    QString inode();
    QString inode(QString file);
    QString modify();
    QString modify(QFileInfo *fi);
    QString modify(QString file);
    QString size();
    QString size(QFileInfo *fi);
    QString size(QString file);
    QString TID(QString file);// same to modify

    QString getText(QString file);
    bool isFile(QString file);


    QString getDateStr(int y,int m,int d);
    QString curDT();
    QString curT();
    QString curTMS();
    QString curD();

    long sEP();
    long msEP();
    QString sEPS();
    QString msEPS();
    QString EP2DT(long s);

    QString DT(QDateTime t);
private:
    QFileInfo *_fileI;
    QString _fileN;


};

#endif
