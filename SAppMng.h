#ifndef SAPP_MNG_H
#define SAPP_MNG_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include "LInfoMng.h"
#include "SAppInfo.h"

class SAppMng : public LInfoMng
{
public:
    SAppMng();
    ~SAppMng();
  
private:
    void init();
  

};
#endif
