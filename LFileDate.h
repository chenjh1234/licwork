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
#include <QByteArray>
#include <QDebug>

#define MDS5_DEF
#ifdef MDS5_DEF
#include "LEncrypt.h"
#endif

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
    QString inodeFull(QString file);
    unsigned long inodeLong(QString f);
    QString inodeDec(QString file);
    QString path(QString file);
    QString modify();
    QString modify(QFileInfo *fi);
    QString modify(QString file);
    QString size();
    QString size(QFileInfo *fi);
    QString size(QString file);
    QString TID(QString file);// same to modify
    #ifdef MDS5_DEF
        QString mds5(QString file);
        QString mds5c(QString file); 
        QString mds5cc(QString file); 
        QStringList cmd(QString c);
    #endif
     
    int writeText(QString file,QString text);
    int writeBuffer(QString file,char *buf,int len);
    QString getText(QString file);
    bool isFile(QString file);

// date:
    QString getDateStr(int y,int m,int d);
    QString curDT();
    QString curT();
    QString curTMS();
    QString curD(); 
    QString DT(QDateTime t);

    long sEP();
    long msEP();
    long sEP(QDateTime d);
    long msEP(QDateTime d);
    QString sEPS();
    QString msEPS();
    QString EP2DT(long s);
    long DT2EP(QString dt);
    int diffDT(QString t1,QString t2);
    int testFileCTime();
    int testFileCTime(QString filen);

   
// rand:
    int rand10();
    QString randText10();
private:
    QFileInfo *_fileI;
    QString _fileN;


};

#endif
