#include "LVenderMng.h"
LVenderMng::LVenderMng():LInfoMng()
{

}

LVenderMng::~LVenderMng()
{

}
bool LVenderMng::isKeyValid(char *key)
{
    char *chV[] = VENDER_KEYS_ALL;
    char **ch;
    bool b;
 
    b = true;
 
    ch = chV;
    b = LInfoMng::isKeyValid(key,ch); 
    return b;

}
bool LVenderMng::isKeysExist()
{
 
    char *chV[] = VENDER_KEYS_MIN;
    char **ch;
    bool b;
 
    b = true;
 
    ch = chV; 
    b = LInfoMng::isKeysExist(ch); 
    return b;
}
QString  LVenderMng::getText()
{
 
    char *chV[] = VENDER_KEYS_OUT;
    char **ch;
    ch = chV;
    return LInfoMng::getText(ch);   
}
