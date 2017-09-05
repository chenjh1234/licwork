#include "LLicEncrypt.h"

LLicEncrypt::LLicEncrypt()
{
   init();
}
LLicEncrypt::~LLicEncrypt()
{
   delete _licFile;
}
void LLicEncrypt::init()
{
   _licFile  = new LLicFile();
}
int LLicEncrypt::openLicFile(QString file)
{
    _fileIn = file;
    return _licFile->readFile(file.Q2CH);
}
LLicFile * LLicEncrypt::getLicFile()
{
    return _licFile;
}
#if 0
//createLic:  serverPub,venderPri,venderSeed
//venderKey:
/** @brief   encrypt vender message with serverPubKey and
             encodeHex , getvenderKey;
    @param infoVender  is from LLicFile
*/
QString LLicEncrypt::encryptVenderKey(QString ServerPub, QString venderPri, LInfo *infoVender,QString venderSeed)
{
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br ,venderSign;
   string sstr;

   vName = infoVender->get(VENDERNAME).toString();
   sID = infoVender->get(SERVERID).toString();
   uuid = infoVender->get(UUID).toString();
   br = infoVender->get(BORROW).toString();

   venderSign = signVenderSeed(venderPri,venderSeed,ServerPub);
   //qDebug() << "venderSign len = " << venderSign.length();

   str = encodeVender(vName,sID,uuid,br,venderSign);
   //qDebug() << "encodeVender len = " << str.length();

   sstr = en.encryptPubHex(ServerPub.Q2CH,str.Q2CH);
   str = QString(sstr.c_str());
   //qDebug() << "encryptHex len = " << str.length();

   return str;
}
/// for borrow:
QString LLicEncrypt::encryptVenderKey(QString ServerPub,LInfo *infoVender,QString venderSign)
{
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br ;
   string sstr;

   vName = infoVender->get(VENDERNAME).toString();
   sID = infoVender->get(SERVERID).toString();
   uuid = infoVender->get(UUID).toString();
   br = infoVender->get(BORROW).toString();

   str = encodeVender(vName,sID,uuid,br,venderSign);
   //qDebug() << "encodeVender len = " << str.length();

   sstr = en.encryptPubHex(ServerPub.Q2CH,str.Q2CH);
   str = QString(sstr.c_str());
   //qDebug() << "encryptHex len = " << str.length();

   return str;
}
/// sign venderSeed+serverPubKey with venderPriKeyfile and encodeHex : Vpri(Spub,VenderSeed)
QString LLicEncrypt::signVenderSeed(QString venderPri, QString venderSeed, QString serverPub)
{
   QString str,pubText;
   LEncrypt en;
   int len,ir;
   char *buf;

   pubText = getPKeyText(serverPub);
   str = venderSeed+pubText;
   len =  en.lenOfSignHex(str.length());
   buf = new char[len];
   ir = en.signHex(venderPri.Q2CH,str.Q2CH,buf);
   //qDebug() << "sign = in,t,len " << venderPri << str.length()<< ir;
   if (ir <=0) 
   {
       qDebug() << "sign error";
       str == "";
   }
   else
       str = buf;
  //qDebug() << "sign = strlen. len " << str.length() << len;
   

   delete []buf;
   return str;
}
/** @brief  compose the vender messages to encodeMsg as the
 *          encrypt input
*/
QString LLicEncrypt::encodeVender(QString venderName, QString ServerID, QString uuid, QString borrow, QString enSeed) //5
{
   QString str,dl;
   dl = VENDER_DELIMIT;
   str = venderName + dl + ServerID + dl + uuid +dl + borrow + dl + enSeed;
   return str;
}
#endif
//createLic:  serverPub,venderPri,venderSeed
//venderKey:
/** @brief   encrypt vender message with serverPubKey and
             encodeHex , getvenderKey;
    @param infoVender  is from LLicFile
*/
QString LLicEncrypt::encodeVenderKey( QString venderPri, LInfo *infoVender,QString venderSeed)
{
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br ,venderSign,enSign;
   string sstr;

   vName = infoVender->get(VENDERNAME).toString();
   sID = infoVender->get(SERVERID).toString();
   uuid = infoVender->get(UUID).toString();
   br = infoVender->get(BORROW).toString();

   venderSign = signVenderSeed(venderPri,venderSeed,sID);
   //qDebug() << "venderSign len = " << venderSign.length();

   //qDebug() << vName<< sID << uuid << br << venderSign;

   str = encodeVender(vName,sID,uuid,br,venderSign);
   //qDebug() << "encodeVender len = " << str.length();
   sstr = en.digest(str.Q2CH);
   enSign = encodeSign(venderSign,uuid);
   str = QString(sstr.c_str()) + enSign;
   //qDebug() << "encryptHex len = " << str.length();
   //qDebug() << "venderKey = " << str;

   return str;
}
/// for borrow:
QString LLicEncrypt::encodeVenderKey(LInfo *infoVender,QString venderSign)
{
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br,enSign ;
   string sstr;

   vName = infoVender->get(VENDERNAME).toString();
   sID = infoVender->get(SERVERID).toString();
   uuid = infoVender->get(UUID).toString();
   br = infoVender->get(BORROW).toString();
   //qDebug() << vName<< sID << uuid << br << venderSign;

   str = encodeVender(vName,sID,uuid,br,venderSign);
   
   sstr = en.digest(str.Q2CH);
   enSign = encodeSign(venderSign,uuid);
   str = QString(sstr.c_str()) + enSign;

   return str;
}
QString LLicEncrypt::encodeSign( QString sign,QString pass)
{
    string inp,hex;
    LEncrypt cr;
    QString str;

    inp  = sign.Q2CH;
    hex = cr.encodePassHex(inp,pass.Q2CH);
    str = hex.c_str();
    return str;
}
QString LLicEncrypt::decodeSign( QString sign,QString pass)
{
    string  hex,ret;
    LEncrypt cr;
    QString str;
    hex  = sign.Q2CH;
    ret= cr.decodePassHex(hex,pass.Q2CH);
    str = ret.c_str();
    return str;
}
/// sign venderSeed+serverID with venderPriKeyfile and encodeHex : Vpri(Spub,VenderSeed)
QString LLicEncrypt::signVenderSeed(QString venderPri, QString venderSeed, QString serverID)
{
   QString str,pubText;
   LEncrypt en;
   int len,ir;
   char *buf;

   //pubText = getPKeyText(serverPub);
   str = venderSeed+serverID;
   len =  en.lenOfSignHex(str.length());
   qDebug() << len;
   buf = new char[len];
   ir = en.signHex(venderPri.Q2CH,str.Q2CH,buf);
   //qDebug() << "sign = in,t,len " << venderPri << str.length()<< ir;
   if (ir <=0) 
   {
       qDebug() << "sign error ir=" << ir;
       str == "";
   }
   else
       str = buf;
  //qDebug() << "sign = strlen. len " << str.length() << len;
   

   delete []buf;
   return str;
}
/** @brief  compose the vender messages to encodeMsg as the
 *          encrypt input
*/
QString LLicEncrypt::encodeVender(QString venderName, QString ServerID, QString uuid, QString borrow, QString venderSign) //5
{
   QString str,dl;
   dl = VENDER_DELIMIT;
   
   str = venderName + dl + ServerID + dl + uuid +dl + borrow + dl + venderSign;
   return str;
}

//packageKey:
/** @brief   digest venderKey+encodedPackageMsg  
     @param infoPackage is from LLicFile
 */
QString LLicEncrypt::digestPackageKey(QString venderKey, LInfo *info)
{
   QString str;
   LEncrypt en;
   QString name,ver,ty,limit,br,sd,ed;
   string sstr;

   name = info->get(PACKAGENAME).toString();
   ver= info->get(PVERSION).toString();
   ty = info->get(PTYPE).toString();
   limit = info->get(PLIMIT).toString();
   br = info->get(PBORROW).toString();
   sd = info->get(PSTARTDATE).toString();
   ed = info->get(PENDDATE).toString();
 
   str = encodePackge(venderKey,name,ver,ty,limit,br,sd,ed);
   sstr = en.digest(str.Q2CH);
   str = QString(sstr.c_str());
   return str;
}

///@brief  compose the vender+Package messages to encodeMsg as the encrypt input
QString LLicEncrypt::encodePackge(QString venderkey, QString pName, QString version, QString type, QString limit, QString borrow, QString start, QString end) // 8
{
   QString str,dl;
   dl = VENDER_DELIMIT;
   str = venderkey + dl + pName + dl + version + dl  + type +  dl+ limit +  dl +borrow + dl + start + dl + end;
   return str;
}
///@brief   get server public key ;
QString LLicEncrypt::getPKeyText(QString file)
{
   QString str;
   LFileDate fa;
   return fa.getText(file);
}

 
//server: serverPri,serverPub,venderPub;borrowInPub,borrwOutPub
// vender:
/// if venderKey is valid load when load file
#if 0
bool LLicEncrypt::isVenderKeyValid(QString ServerPri, LInfo *infoVender)
{
   bool b;
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br,venderSign;
   QString venderKey;

   venderKey = infoVender->get(VENDERKEY).toString();
   str = decryptVenderKey(ServerPri,venderKey);
   if (str.length() ==0)  return false;
   //qDebug() << "decryptVenderKey len=" << str.length() << venderKey.length();

   decodeVender(str,vName,sID,uuid,br,venderSign);
   //qDebug() << "decodeVender len = " <<venderSign.length();

   b = true;

   if(vName != infoVender->get(VENDERNAME).toString()) b = false;
   if(sID != infoVender->get(SERVERID).toString()) b = false;
   if(uuid != infoVender->get(UUID).toString()) b = false;
   if(br != infoVender->get(BORROW).toString()) b = false;
   //qDebug() << "venderSign = " << venderSign;
   _venderSign = venderSign;

   return b;
}
#endif
bool LLicEncrypt::isVenderKeyValid(LInfo *infoVender)
{
   bool b;
   QString str,digest;
   LEncrypt en;
   QString vName,sID,uuid,br,venderSign,enSign;
   QString venderKey;
   string sstr;

   venderKey = infoVender->get(VENDERKEY).toString();
   digest = venderKey.left(32);
   enSign = venderKey.mid(32);
   

   vName = infoVender->get(VENDERNAME).toString();
   sID = infoVender->get(SERVERID).toString();
   uuid = infoVender->get(UUID).toString();
   br = infoVender->get(BORROW).toString();
   venderSign = decodeSign(enSign,uuid);


   str = encodeVender(vName,sID,uuid,br,venderSign);
   //qDebug() << " venderKey = "<< venderKey;
  // qDebug() << " encodeVender = "<< str;
   str = en.digest(str.Q2CH).c_str();
   
  // qDebug() << " digestOfEnVender= "<< str;
  // qDebug() << " digest = "<< digest;
   
   b = true; 
   if (str != digest) b = false;

   //qDebug() << "venderSign = " << venderSign;
   _venderSign = venderSign;

   return b;
}
 
 #if 0 // no serverPub now
///   decrypt vender message with serverPriKey (hexDecode and decryptPri)
QString LLicEncrypt::decryptVenderKey(QString ServerPri, QString venderKey)
{
   QString str;
   LEncrypt en;
   QString vName,sID,uuid,br,venderSign;
   string sstr;
   sstr = en.decryptPriHex(ServerPri.Q2CH,venderKey.Q2CH);
   str = QString(sstr.c_str());
   //cout << " sstr= " << sstr.length() << endl;
   //qDebug() << "decrypt = " << str.length()  <<venderKey.length() ;
   return str;
}
/// decode vender Message
void LLicEncrypt::decodeVender(QString venderKey, QString& venderName, QString& ServerID, QString& uuid, QString& borrow,QString &enSeed) //6
{
   QString str,dl;
   QStringList slist;
   int i;
   i = 0;
   dl = VENDER_DELIMIT;
   slist = venderKey.split(dl);
   venderName = slist[i];i++;
   ServerID = slist[i];i++;
   uuid = slist[i];i++;
   borrow = slist[i];i++;
   enSeed = slist[i];  
   //qDebug() << "decodeVender in  len = " << enSeed.length();
   
}
#endif
// package:
/// if PackageKey is valid , when load file
bool LLicEncrypt::isPackageKeyValid(QString venderKey, LInfo *info)
{
   QString str,key;
   str = digestPackageKey( venderKey, info);
   key =  info->get(PKEY).toString();
   return str == key;
}
#if 0
/// decode package Message
QString LLicEncrypt::decodePackge(QString key, QString& venderEncode, QString& pName, QString& version, QString& type, QString& limit, QString& borrow, QString& start, QString& end) //9 parameters
{
   QString str;
   return str;
}
#endif
// app: venderSeed ,venderPubChar
///
bool LLicEncrypt::verifyVenderSeed(QString venderPubChar, QString venderSign, QString venderSeed, QString serverID)//4 verify venderSeed & serverPub    app use it
{
   bool b;
   QString str;
   LEncrypt en;
   str = venderSeed+serverID;
   //qDebug() << "verify = in,t,len " << VENDER_PUB << str.length() << venderSign.length();
   qDebug() << " verify seed,sid,sign" << venderSeed << serverID<<venderSign;
   qDebug() << " pubchar =" << venderPubChar;

   b = en.verifyHexPubChar(venderPubChar.Q2CH, venderSign.Q2CH, str.Q2CH) ;
    
   return b;
}
// mid:
/// geMid
QString LLicEncrypt::getMid()
{
    string m;
    int j;
    LEncrypt cr;
    QString n,str,mac,seed,s;
    seed = "12345";
    str = getMidMark() + seed ;
    m = cr.digest(str.Q2CH);
    s = m.c_str();
    //qDebug() << "MID=" << s;
    str = "";
    for(j = 0; j <32 ;j =j+8)
    {
        mac = s.mid(j,8) + "-";
        str = str + mac;
    }
    str = str.mid(0,32+3);

    return str ;
}
QString LLicEncrypt::getMidMark()
{
   int j;
   QString str,s;
   char  filen[256];
   QString startx,yum,ifconfig,sbin,mac;
    
   //: /usr/bin/startx
   j = 0;
   filen[j] = '/'; j++;
   filen[j] = 'u'; j++;
   filen[j] = 's'; j++;
   filen[j] = 'r'; j++;
   filen[j] = '/'; j++;
   filen[j] = 'b'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'n'; j++;
   filen[j] = '/'; j++;
   filen[j] = 's'; j++;
   filen[j] = 't'; j++;
   filen[j] = 'a'; j++;
   filen[j] = 'r'; j++;
   filen[j] = 't'; j++;
   filen[j] = 'x'; j++;
   filen[j] = 0; j++;

   startx = filen; 

   //: /usr/bin/yum
   j = 0;
   filen[j] = '/'; j++;
   filen[j] = 'u'; j++;
   filen[j] = 's'; j++;
   filen[j] = 'r'; j++;
   filen[j] = '/'; j++;
   filen[j] = 'b'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'n'; j++;
   filen[j] = '/'; j++;
   filen[j] = 'y'; j++;
   filen[j] = 'u'; j++;
   filen[j] = 'm'; j++;
   filen[j] = 0; j++;
   yum = filen;
   j = 0;
   filen[j] = '/'; j++;
   filen[j] = 's'; j++;
   filen[j] = 'b'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'n'; j++;
   filen[j] = '/'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'f'; j++;
   filen[j] = 'c'; j++;
   filen[j] = 'o'; j++;
   filen[j] = 'n'; j++;
   filen[j] = 'f'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'g'; j++;
   filen[j] = 0; j++;
   ifconfig = filen;

 
   j = 0;
   filen[j] = '/'; j++;
   filen[j] = 's'; j++;
   filen[j] = 'b'; j++;
   filen[j] = 'i'; j++;
   filen[j] = 'n'; j++;
   filen[j] = 0; j++;
   sbin = filen;

   string m;
   LEncrypt cr;

   LFileDate fa;
   QString n;
   n = "\n";
// mac
   m = cr.getMac();
   mac = m.c_str()+ n;
   str = mac;
//startx
   s = fa.modify(startx)+ n;
   str = str +s;
//yum
   s = fa.modify(yum)+ n;
   str = str +s;
//ifconfig
   s = fa.modify(ifconfig)+ n;
   str = str +s;
//sbin:node
   s = fa.inode(sbin)+ n;
   str = str +s;
   return str;
}

