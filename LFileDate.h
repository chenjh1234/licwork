#ifndef LFILEDATE_API_H
#define LFILEDATE_API_H

 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include <arpa/inet.h>

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
    QString curDT(int s = 0);//16
    QString curT();//8
    QString curTMS();//8+3
    QString curD(); //8
    QString curM();//6
    QString curY(); //4
    QString DT(QDateTime t);//16

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

    int  getFreePort(int portStart,int portLen);
    //
    QString passwdToday(QString seed = "");
    QString passwd2M(QString seed = "");

    QString passwdM(QString seed = "",int add = 0);
    QString getPassStr(QString dig);
    bool isPasswdToday(QString pass ,QString seed = "");
    bool isPasswd2M(QString pass ,QString seed = "");


   
// rand:
    int rand10();
    QString randText10();
private:
    QFileInfo *_fileI;
    QString _fileN;


};

#endif
