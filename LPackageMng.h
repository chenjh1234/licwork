#ifndef PACKAGE_MNG_H
#define PACKAGE_MNG_H
#include "LInfoMng.h"
#include "LPackageDef.h"
class LPackageMng : public LInfoMng
{
public:
    LPackageMng();
    ~LPackageMng();

    bool isKeysExist();// is the MIN key defined
    bool isKeyValid(char * key);// is the key outofMAX defined
    QString getText(); // key = value as OUT defined sequence
  
};
#endif
