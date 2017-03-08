#ifndef SPACK_MNG_H
#define SPACK_MNG_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>
#include "LInfoMng.h"
#include "SPackInfo.h"

class SPackMng : public LInfoMng
{
public:
    SPackMng();
    ~SPackMng();
     int request(QString ip,QString pid,QString user,int number);
  
private:
    void init();
    QMutex _lock;
   
  

};

#endif

