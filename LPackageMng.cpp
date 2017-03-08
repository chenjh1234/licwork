#include "LPackageMng.h"
//#include "LVenderMng.h"
LPackageMng::LPackageMng():LInfoMng()
{

}

LPackageMng::~LPackageMng()
{

}
bool LPackageMng::isKeyValid(char *key)
{
    char *chV[] = PACKAGE_KEYS_ALL;
    char **ch;
    bool b;
 
    b = true;
 
    ch = chV;
    if (b) 
        b = LInfoMng::isKeyValid(key,ch); 
    return b;

}
bool LPackageMng::isKeysExist()
{
 
    char *chP[] = PACKAGE_KEYS_MIN;
    char **ch;
    bool b;
 
    b = true;
    ch = chP;
    if (b) 
        b = LInfoMng::isKeysExist(ch); 
    return b;
}
 
QString  LPackageMng::getText()
{
 
    char *chP[] = PACKAGE_KEYS_OUT;
    char **ch;
    ch = chP;
    return LInfoMng::getText(ch);   
}
