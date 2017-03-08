#ifndef VENDERMNG_H
#define VENDERMNG_H
#include "LInfoMng.h"
#include "LVenderDef.h"
class LVenderMng :public LInfoMng
{
public:
    LVenderMng();
    ~LVenderMng();

    bool isKeysExist();// is the MIN key defined
    bool isKeyValid(char *key);// is the key outof MAX defined
    QString getText();// key = value as OUT defined sequence
 
};
#endif
