#include "SLicMng.h"

SLicData *SLicMng::data = new SLicData();
//SLicData *SLicMng::data = NULL;

SLicMng::SLicMng()
{
   init();
}

SLicMng:: ~SLicMng()
{

   //qDebug() << " end of licMng=======";
}
bool SLicMng::startup()
{
 
    return data->startup();
}
 
void SLicMng::down()
{
    data->down();
}
void SLicMng::init()
{
   int i;
   //QString str;
   char *ch[] = LOADFILE_ERR;
   i = 0;
   mapLoadFile[OK] = ch[i]; i++;
   mapLoadFile[LOADFILE_FILE_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_VENDERKEY_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_PACKAGEKEY_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_MID_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_VENDERSIGN_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_UUID_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_LOAD_ERR] = ch[i]; i++;
   mapLoadFile[LOADFILE_DATE_ERR] = ch[i]; i++;
   //setCheckUUID(true);
}

//------------------app-----------------------------
int SLicMng::loginApp(SAppInfo& msg)
{
   return data->addApp(msg);
}

int SLicMng::logoutApp(SAppInfo& msg)
{
   return data->rmApp(msg);
}

//------heartBeat----------------------------------
int SLicMng::heartBeat(SAppInfo& msg)
{
   return data->appHB(msg);
}

#if 0
int SLicMng::unloadPackage(QString vender, QString package, QString version, QString serverPri)
{
   QString str, packid;
   packid = data->encodePackageName(vender, package, version);
   return 0;

   //return data->removePackage( packid);
}
#endif
//-------------chack file:-----------------------------
//int SLicMng::checkLicFile(QString filename, QString serverPri)
#if 1
int SLicMng::checkLicFile(QString filename)
{
   return checkLic(filename, LOAD_LIC_FILE);
}

int SLicMng::checkLicStr(QString str)
{
   return checkLic(str, LOAD_LIC_STR);
}
#endif
int SLicMng::checkLic(QString filename, int mode)
{

   int i;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo * infoV,*infoP;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   bool b;
   //qDebug() << "mode = " << mode;
   //qDebug() << "filename  = " << filename;

   // readfile:
   if (mode == LOAD_LIC_FILE) i = licFile.readFile(filename);
   else i = licFile.readStr(filename);
   //qDebug() << "check    lic return from read = " << i;

   //i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR;
   infoV = licFile.vender();

   // venderkey verify:
   // b = lic.isVenderKeyValid(serverPri, infoV);
   b = lic.isVenderKeyValid(infoV);
   if (!b) return LOADFILE_VENDERKEY_ERR;

   venderKey = infoV->get(VENDERKEY).toString();
   venderSign = lic.getVenderSign(); //  isVenderKeyValid() make it availibale
   uuid = infoV->get(UUID).toString();
   smid = infoV->get(SERVERID).toString();
   venderName = infoV->get(VENDERNAME).toString();
   // mid
   QString mid;
   mid = lic.getMid();
   //qDebug() << "mid = "<<mid;
   //qDebug() << "sid = "<<smid;
   if (mid != smid)  return LOADFILE_MID_ERR;

   // is vendername venderSign, consistent:
   b = isVenderSign(venderName, venderSign);
   if (!b)  return LOADFILE_VENDERSIGN_ERR;

   // packagekey verify:

   int num;
   LFileDate fd;
   str = fd.curD();
   num  = licFile.size();
   //qDebug() << "packages = " << num;
   for (i = 0; i < num; i++)
   {
      infoP = licFile.package(i);
      b = lic.isPackageKeyValid(venderKey, infoP);
      if (!b) return  LOADFILE_PACKAGEKEY_ERR;
      // checkDate:
      qDebug () << str << infoP->get(PSTARTDATE).toString() <<infoP->get(PSTARTDATE).toString() ;
      if (str < infoP->get(PSTARTDATE).toString() || str > infoP->get(PENDDATE).toString())  return  LOADFILE_DATE_ERR;
   }

   return num;
}
//load file------------------------
//int SLicMng::loadFile(QString filename, QString serverPri)

int SLicMng::loadFile(QString filename)
{
   int i;
   QString err;
   i = loadLic(filename, LOAD_LIC_FILE);
   if (i < 0)
   {
      err = "load file error: " + mapLoadFile[i] + " " + filename;
      data->plog(err);
   }
   return i;
}
int SLicMng::loadFileStr(QString str)
{
   int i;
   QString err;
   i = loadLic(str, LOAD_LIC_STR);
   if (i < 0)
   {

      err = "load file error: " + QString("%1").arg(i) + " " +mapLoadFile[i] ;
      data->plog(err);
   }
   return i;

}

int SLicMng::loadLic(QString filename, int mode)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo * infoP,*infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName, borrow;
   bool b;
   //num = checkLicFile(filename, serverPri);
   num = checkLic(filename, mode);
   if (num <= 0)  return num;

// readfile:
   if (mode == LOAD_LIC_FILE)  i = licFile.readFile(filename);
   else i = licFile.readStr(filename);

   if (i <= 0) return LOADFILE_FILE_ERR;
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();
   borrow = infoV->get(BORROW).toString();
   venderName = infoV->get(VENDERNAME).toString();
   smid = infoV->get(SERVERID).toString();

   //lic.isVenderKeyValid(serverPri, infoV);// we do not if
   lic.isVenderKeyValid(infoV); // we already checked ,here we only to get venderSign
   venderSign = lic.getVenderSign(); //  isVenderKeyValid make it available



// insert to Data:
   SPackInfo *infoSP;
   //SPackInfo *mngSP;
   QString packid, pname, version;
   int len;
   len = 20;
   int j;
   for (i = 0; i < num; i++)
   {
      infoP = licFile.package(i);
      infoSP = new SPackInfo();
      *((LInfo *)infoSP) = *infoP;
      // set infoSP:
      //remove:
      //infoSP->remove(SERVERID);
      //infoSP->remove(SERVERNAME);
      //get:
      pname = infoSP->get(PACKAGENAME).toString();
      version = infoSP->get(PVERSION).toString();
#if 0
      str = infoSP->get(PBORROW).toString();

      if (str.length() >len) // decode hex
      {
         str = data->unHex(str);
         infoSP->set(PBORROW,str);
      }
#endif
      //set:
      infoSP->set(VENDERNAME, venderName);// add
      infoSP->set(VENDERKEY, venderKey);
      infoSP->set(VENDERSIGN, venderSign);
      infoSP->set(BORROW, borrow);
      infoSP->set(UUID, uuid);
      infoSP->set(SERVERID, smid);

      //qDebug() << "info ===" <<  infoSP->getText();

      packid = data->encodePackageId(venderName, pname, version);
      // qDebug() << "000000=" ;
      infoSP->packid = packid;
      infoSP->setStat();
      //qDebug() << "00000011=" ;
      //infoSP->limit =  infoSP->get(PLIMIT).toInt();
      // is UUID exist:
      if (i == 0)
      {
         b = isUuid(packid, uuid);
         qDebug() << "isUUID =" <<b;
         if (!b)  return LOADFILE_UUID_ERR;
      }
      // add package:
      //qDebug() << "---addPackage(infoSP)  ";
      j = data->addPackage(infoSP);
      if (j <= 0)
      {
         qDebug() <<  "Load error";
         return LOADFILE_LOAD_ERR;
      }
   }
   ret = num;
   return ret;
}
// unload file
/// unload license  file:
//int SLicMng::unloadFile(QString filename, QString serverPri)
int SLicMng::unloadFile(QString filename, QString& proofStr)
{
   int i;
   QString err;
   i = unloadLic(filename, proofStr, LOAD_LIC_FILE);
   if (i < 0)
   {
      err = "unload file error: " + mapLoadFile[i] + " " + filename;
      data->plog(err);
   }
   return i;
   //return unloadLic(filename,proofStr,LOAD_LIC_FILE);
}
int SLicMng::unloadFileStr(QString str, QString& proofStr)
{
   int i;
   QString err;
   i = unloadLic(str, proofStr, LOAD_LIC_STR);
   if (i < 0)
   {
      err = "unload file error: " + mapLoadFile[i] + " " + str;
      data->plog(err);
   }
   return i;
   //return unloadLic(str,proofStr,LOAD_LIC_STR);
}
int SLicMng::unloadLic(QString filename, QString& proofStr, int mode)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   //num = checkLicFile(filename, serverPri);
   if (mode == LOAD_LIC_FILE)      num = checkLicFile(filename);
   else num = checkLicStr(filename);
   // qDebug() << " after check lic num = " << num;

   if (num <= 0)  return num;

   // readfile:
   if (mode == LOAD_LIC_FILE)  i = licFile.readFile(filename);
   else i = licFile.readStr(filename);
   //qDebug() << " after readfile i= " << i;

   if (i <= 0) return LOADFILE_FILE_ERR;
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();
   //qDebug() << " uuid = " <<uuid;

   ret = data->unloadPackage(uuid, proofStr);
   return ret;
}
/// pass ="" : unload, else remove

QString SLicMng::unloadPackage(QString vender,QString pack,QString version,QString number,QString ty,QString uuid,QString pass)
{

   SPackInfo info;
   QString packid,str,ret;
   LFileDate fd;

   info.set(VENDERNAME, vender);
   info.set(PACKAGENAME, pack);
   info.set(PVERSION, version);
   info.set(PTYPE, ty);
   info.set(PLIMIT, number);
   info.set(UUID, uuid);
   int mode;
   mode = 0;
   //packid = data->encodePackageId(vender, pack, version);
   if (pass == "")  
   {
       ret = data->unloadPackage(info,mode);
   }
   else
   {
       mode = 1;
       if (fd.isPasswd2M(pass,"removepackage"))  
       {
           ret = data->unloadPackage(info,mode);
       }
       else
           qDebug() << " ===========passwd is not valid";
   }
   return ret;
}
int SLicMng::removeFile(QString filename)
{
   int i;
   QString err,str;
   i = removeFile(filename, LOAD_LIC_FILE);
   if (i < 0)
   {
      err = "removeFile file error: " + mapLoadFile[i] + " " + str;
      data->elog(err);
   }
   return i;
}
int SLicMng::removeFileStr(QString filename)
{
   int i;
   QString err,str;
   i = removeFile(str,  LOAD_LIC_STR);
   if (i < 0)
   {
      err = "removeFile file error: " + mapLoadFile[i] + " " + str;
      data->elog(err);
   }
   return i;
}
int SLicMng::removeFile(QString filename,int mode)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   //num = checkLicFile(filename, serverPri);
    
      if (mode == LOAD_LIC_FILE)      num = checkLicFile(filename);
   else num = checkLicStr(filename);

  // num = checkLic(filename, mode);
   if (num <= 0)  return num;

   // readfile:
   if (mode == LOAD_LIC_FILE)  i = licFile.readFile(filename);
   else i = licFile.readStr(filename);

   //i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR;

   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();

   ret = data->removePackage(uuid);
   return ret;
}

//-------borrow-----------------------
int SLicMng::borrowReturn(QString fileProof)
{
   return data->borrowReturn(fileProof);

}
int SLicMng::borrowFile(QString& filename, QString& fo)
{
   return borrow(filename, fo, LOAD_LIC_FILE);
}
int SLicMng::borrowFileStr(QString& str, QString& stro)
{
   return borrow(str, stro, LOAD_LIC_STR);
}

int SLicMng::borrow(QString& filename, QString& fo, int mode)
{

   int i;
   LLicEncrypt lic;
   LLicFile *lfile;
   QString str, fileo;
   LInfo * infoV,*infoP;
   SPackInfo *infoSP;
   LEncrypt en;
   LFileDate fd;
//   QString sPubIn,sPubOut;// infact is serverID

// open file:
   QString  venderKey, pKey, sPub, vSign, uuid, mid;
   mid = lic.getMid();

   fileo = fo;
   lfile = new LLicFile();
   if (mode == LOAD_LIC_FILE)  i = lfile->readFile((char *)filename.Q2CH);
   else i = lfile->readStr((char *)filename.Q2CH);

   //qDebug() << "lfile.->readFile return = " << i;
   if (i <= 0)
   {
      cout << INPUT_ERR;
      delete lfile;
      return -1;
   }

// serverPub files:
   infoV = lfile->vender();
// vendername:
   QString vName;
   QString pname, pver, packid, ptype, bmid, borrow;

   vName = infoV->get(VENDERNAME).toString();
   uuid = infoV->get(UUID).toString(); // from inputfile
   bmid = infoV->get(SERVERID).toString(); // from inputfile


   infoP = lfile->package(0);

// register borrowOut------------------------------------
   infoSP = new SPackInfo();
   *((LInfo *)infoSP) = *infoP; // copy   // pack for bout -limit:

   pname = infoSP->get(PACKAGENAME).toString();
   pver = infoSP->get(PVERSION).toString();

   infoSP->set(BORROW, BORROW_NO);
   infoSP->set(UUID, uuid);

   infoSP->set(BMID, bmid);// this ID
   infoSP->set(SERVERID, mid); // borrowIn serverID message in Input_file

   packid = data->encodePackageId(vName, pname, pver);

   infoSP->packid = packid;
   infoSP->limit =  infoSP->get(PLIMIT).toInt();
   if (infoSP->limit <= 0)
   {
      cout << "borrow Error : limit <= 0";
      delete lfile;
      return -1;
   }
   //qDebug() << "infoSp = " << infoSP->getText();
   i = data->borrow(infoSP);//changed: assign new uuid,pborrow,vendersign
   //qDebug() << "infoSp after =  "<< infoSP->getText();
   if (i < 0)
   {
      cout << "borrow Error : " << infoSP->err.Q2CH;
      delete lfile;
      return -1;
   }
// ready for borrow license file:

// for Bout lic file:
//get venderKey:
   // uuid:
   str = infoSP->get(PBORROW).toString();// new uuid
   infoV->set(UUID, str);
   //borrow:
   infoV->set(BORROW, "no"); // ban of borrow  //str = lic.encryptVenderKey(sPubIn, infoV, vSign);
   // sign:
   vSign = infoSP->get(VENDERSIGN).toString();//qDebug() << " sign = " << vSign;
   str = lic.encodeVenderKey(infoV, vSign);
   infoV->set(VENDERKEY, str);

   //qDebug() << "venderKey = " << VENDERKEY<<str;
   if (str.length() <= 0)
   {
      cout << VENDERKEY_ERR;
      delete lfile;
      return -1;
   }
   venderKey = str;
// get package keys:
   infoP->set(PBORROW, uuid);
   infoP->set(BMID, mid);
   //key:
   str = lic.digestPackageKey(venderKey, infoP);
   infoP->set(PKEY, str);
// output files:
   if (mode == LOAD_LIC_FILE)
   {
      i = lfile->writeFile((char *)fileo.Q2CH);
   }
   else
   {
      str = lfile->writeStr();
      fo = str;
      i = fo.length();
   }



   if (i <= 0)
   {
      cout << WRITEFILE_ERR;
      delete lfile;
      return -1;
   }

   delete lfile;
   str = "borrow Lic  OK->" + fileo;
   cout << str.Q2CH << endl;
   return 1;


}
int SLicMng::packDB2DB()
{
   int i;
   QString str;
   str = "packDB2DB OK!!!";
   i = data->loadDBPackage();
   if (i > 0)
   {
      i = data->saveDB();
      if (i < 0) str = "daveDB error when packDB2DB\n";

   }
   else  str = " packDB load error!! when packDB2DB\n";


   cout << str.Q2CH;
   // plog(str);
   return i;
}
// report====================================
//
QStringList SLicMng::reportPackage()
{
   // name,size,task(limit,used),node(limit,used),user(limit,used),
   // name,ty,limit,start,end;
   int i, sz;
   QStringList packs, slist, rlist;
   QString pack;

   packs = data->packNames();
   sz = packs.size();
   for (i = 0; i < sz; i++)
   {
      pack = packs[i];
      slist = reportPackage(pack);
      rlist << slist;
   }
   return rlist;
}
QStringList SLicMng::reportPackage(QString pack)
{
   // name,size,task(limit,used),node(limit,used),user(limit,used),
   // name,ty,limit,start,end;
   int  j, szp;
   QStringList  slist;
   SPackMng *mng;
   SPackInfo *info;
   QString name, ty, size, str, limit, start, end, bmid,stat,uuid;
   int used, limitAll,limit0,left;
   int len1, len2, len3;
   len1 = 10;
   len2 = 20;
   len3 = 25;

   mng = data->packMng(pack);

   if (mng == NULL) return slist;

   szp = mng->size();
   // packID sizeInfo limitTask used Nodelimit used UserLimit used

   //str = QString("---------- %1 %2 task(%3,%4) node(%5,%6) user(%7,%8)").arg(pack,len3).arg(szp,len1).arg(mng->taskLimit()).arg(mng->taskUsed()).arg(mng->nodeLimit()).arg(mng->nodeUsed()).arg(mng->userLimit()).arg(mng->userUsed());
   //slist << str;

   for (j = 0; j < szp; j++)
   {
      info = (SPackInfo *)mng->get(j);
      ty = info->get(PTYPE).toString();
      limit = info->get(PLIMIT).toString();
      start = info->get(PSTARTDATE).toString();
      end = info->get(PENDDATE).toString();
      bmid = info->get(BMID).toString();
      uuid = info->get(UUID).toString();
      limit0 = info->limit;
      stat = info->statStr();
      if (ty == PTYPE_TASK)
      {
         limitAll = mng->taskLimit();
         used = mng->taskUsed();
      }
      if (ty == PTYPE_NODE)
      {
         limitAll = mng->nodeLimit();
         used = mng->nodeUsed();
      }
      if (ty == PTYPE_USER)
      {
         limitAll = mng->userLimit();
         used = mng->userUsed();
      }
      left = limitAll - used;
    
      str = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12").arg(pack, len3).arg(ty, len1).arg(limitAll, len1).arg(used, len1).arg(left, len1).arg(limit0, len1).arg(limit, len1).arg(stat, len1).arg(start, len1).arg(end, len1).arg(uuid, len3).arg(bmid,len3);
      slist << str;
   }

   return slist;
}

QStringList SLicMng::reportApp()
{
   // name,size,task(limit,used),node(limit,used),user(limit,used),
   // name,ty,limit,start,end;
   int i, sz;
   QStringList packs, slist, rlist;
   QString name, ty, size, str, limit, start, end, pack;

   packs = data->packNames();
   sz = packs.size();
   for (i = 0; i < sz; i++)
   {
      pack = packs[i];
      slist = reportApp(pack);
      rlist << slist;
   }
   return rlist;

}

QStringList SLicMng::reportApp(QString pack)
{
   // packid,appid,user,number type, start
   int  j, szp, number;
   QStringList  slist;
   SAppMng *mng;
   SAppInfo *info;
   QString appid, user, ty, str, start,appname;
   //long start;
   LFileDate fd;
   int len1, len2, len3;
   len1 = 10;
   len2 = 20;
   len3 = 25;


   mng = data->appMng(pack);
   //qDebug() << "mng = " << mng;

   if (mng == NULL) return slist;

   szp = mng->size();
   //qDebug() << "mng_size = " << szp;

   for (j = 0; j < szp; j++)
   {
      info = (SAppInfo *)mng->get(j);
      appid = info->appid;
      user = info->user;
      start = fd.EP2DT(info->start);
      ty = info->rtype;
      number = info->number;
      appname = info->get(APP_NAME).toString();
      str = QString("%1 %2 %3 %4 %5 %6 %7").arg(pack, len3).arg(appid, len2).arg(user, len1).arg(number, len1).arg(ty, len1).arg(start, len3).arg(appname,len3);
      slist << str;
   }
   //qDebug() << "slist_size = " << slist.size();

   return slist;
}
//======================================
/// if uuid exist in the DB: true no this uuid
bool SLicMng::isUuid(QString packid, QString uuid)
{
   // qDebug() << "isCheckUUID() = " << isCheckUUID();
   if (isCheckUUID())  return data->isUuid(packid, uuid);
   else return true;

}
/// if venderSign is the same with the venderName in the DB (),true if the same or no vendername in DB
bool SLicMng::isVenderSign(QString venderName, QString venderSign)
{
   return data->isVenderSign(venderName, venderSign);
}
void SLicMng::setCheckUUID(bool b)
{
   data->setCheckUUID(b);
}
bool SLicMng::isCheckUUID()
{
   return data->isCheckUUID();
}
