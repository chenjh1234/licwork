#include "SLicMng.h"

SLicData *SLicMng::data = new SLicData();
SLicMng::SLicMng()
{
   init();

}
SLicMng:: ~SLicMng()
{

    //qDebug() << " end of licMng=======";
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
int SLicMng::heartBeat(SAppInfo &msg)
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
int SLicMng::checkLicFile(QString filename)
{

   int i;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo * infoV,*infoP;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   bool b;
   // readfile:
   i = licFile.readFile(filename);
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
   str = fd.curDT();
   num  = licFile.size();
   //qDebug() << "packages = " << num;
   for (i = 0; i < num; i++)
   {
      infoP = licFile.package(i);
      b = lic.isPackageKeyValid(venderKey, infoP);
      if (!b) return  LOADFILE_PACKAGEKEY_ERR;
       // checkDate:
      if (str < infoP->get(PSTARTDATE).toString() || str > infoP->get(PENDDATE).toString())  return  LOADFILE_DATE_ERR;
   }
  
   return num;
}
//load file------------------------
//int SLicMng::loadFile(QString filename, QString serverPri)
int SLicMng::loadFile(QString filename)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoP,*infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName, borrow;
   bool b;
   //num = checkLicFile(filename, serverPri);
   num = checkLicFile(filename);
   if (num <= 0)  return num;

// readfile:
   i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR; 
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();
   borrow = infoV->get(BORROW).toString();
   venderName = infoV->get(VENDERNAME).toString();
   smid = infoV->get(SERVERID).toString();

   //lic.isVenderKeyValid(serverPri, infoV);// we do not if
   lic.isVenderKeyValid(infoV);// we do not if
   venderSign = lic.getVenderSign(); //  isVenderKeyValid make it available
   
   

// insert to Data:
   SPackInfo *infoSP;
   //SPackInfo *mngSP;
   QString packid, pname, version;
   int len;
   len =20;
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
      str = infoSP->get(PBORROW).toString();
       
          if (str.length() >len) // decode hex
          {
              str = data->unHex(str);
              infoSP->set(PBORROW,str);
          }
      //set:
      infoSP->set(VENDERKEY, venderKey);
      infoSP->set(VENDERSIGN, venderSign);
      infoSP->set(BORROW, borrow);
      infoSP->set(UUID, uuid);
      infoSP->set(SERVERID, smid);

      // qDebug() << "uuid ===" <<  infoSP->getText();

      packid = data->encodePackageId(venderName, pname, version);
      infoSP->packid = packid;
      infoSP->setStat();
      //infoSP->limit =  infoSP->get(PLIMIT).toInt();
      // is UUID exist:
      if (i == 0)
      {
         b = isUuid(packid, uuid);
         if (!b)  return LOADFILE_UUID_ERR;
      }
      // add package:
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
int SLicMng::unloadFile(QString filename,QString proofFile)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   //num = checkLicFile(filename, serverPri);
   num = checkLicFile(filename);
   if (num <= 0)  return num;

   // readfile:
   i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR; 
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();
   qDebug() << " uuid = " <<uuid;

   ret = data->unloadPackage(uuid,proofFile);
   return ret;
}
int SLicMng::removeFile(QString filename)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   //num = checkLicFile(filename, serverPri);
   num = checkLicFile(filename);
   if (num <= 0)  return num;

   // readfile:
   i = licFile.readFile(filename);
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
int SLicMng::borrow(QString filename, QString fo)
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
   QString  venderKey, pKey,sPub,vSign,uuid,mid;
   mid = lic.getMid();
 
   fileo = fo;
   lfile = new LLicFile();
   i = lfile->readFile((char *)filename.Q2CH);
   //qDebug() << "lfile.->readFile return = " << i;
   if (i <= 0)
   {
      cout << INPUT_ERR;
      return -1;
   }
 
// serverPub files:
   infoV = lfile->vender();
   #if 0
   sPub = mid;
// is serverPub,venserPri file exist:
   //pubIn
   if (!fd.isFile(sPub))
   {
      cout << SERVERPUB_ERR << "file = " << sPub.Q2CH << endl;
      return -1;
   }
   //pubOut
   sPub = sPubOut;
   if (!fd.isFile(sPub))
   {
      cout << SERVERPUB_ERR << "file = " << sPub.Q2CH << endl;
      return -1;
   }
 
    #endif
// vendername:
   QString vName;
   QString pname,pver,packid,ptype,bmid,borrow;

   vName = infoV->get(VENDERNAME).toString();  
   uuid = infoV->get(UUID).toString();// from inputfile
   bmid = infoV->get(SERVERID).toString();// from inputfile
   

   infoP = lfile->package(0);

// register borrowOut------------------------------------
       infoSP = new SPackInfo();
      *((LInfo *)infoSP) = *infoP;// copy
      // set infoSP:
 
      pname = infoSP->get(PACKAGENAME).toString();
      pver = infoSP->get(PVERSION).toString();
       
      //infoSP->set(VENDERKEY, venderKey);
      //infoSP->set(VENDERSIGN, venderSign);
      infoSP->set(BORROW,BORROW_NO);
      infoSP->set(UUID, uuid);
      infoSP->set(BMID, bmid);
      infoSP->set(SERVERID, mid);// borrowIn serverID

      packid = data->encodePackageId(vName, pname, pver);

      infoSP->packid = packid;
      infoSP->limit =  infoSP->get(PLIMIT).toInt();
      if (infoSP->limit <=0 ) 
      {
          cout << "borrow Error : limit <= 0"  ;
          return -1;
      }
// register a -limit package:-----------------------------------------
     // qDebug() << "infoSpID = " << infoSP->packid<<infoSP->limit;
      qDebug() << "infoSp = " << infoSP->getText();
      i = data->borrow(infoSP);
      //qDebug() << "infoSp after =  "<< infoSP->getText();
      if(i <0 ) 
      {
          cout << "borrow Error : " << infoSP->err.Q2CH;
          return i;
      }
// ready for borrow license file:
 
#if 1
     
//get venderKey
 
#if 0 //  test a diffrence new uuid
   string sstr;
   sstr = en.uuid();
   uuid = sstr.c_str();
   infoV->set(UUID,uuid);
#endif

   infoV->set(BORROW,"no"); // ban of borrow 
   //str = lic.encryptVenderKey(sPubIn, infoV, vSign);
   vSign = infoSP->get(VENDERSIGN).toString();
   qDebug() << " sign = " << vSign;
   str = lic.encodeVenderKey(infoV, vSign);
   infoV->set(VENDERKEY, str);

   //qDebug() << "venderKey = " << VENDERKEY<<str;
   if (str.length() <= 0)
   {
      cout << VENDERKEY_ERR;
      return -1;
   }
   venderKey = str;
// get package keys:
   // pborrow:
   //borrow = fd.getText(sPubOut);
   //qDebug() << "borrow = " << borrow;// tohex:
   //borrow = data->hex(borrow);
   borrow = mid;//
   infoP->set(PBORROW,borrow);
   infoP->set(BMID,mid);
   //key:
   str = lic.digestPackageKey(venderKey, infoP);
   infoP->set(PKEY, str);  
// output files:
  
   i = lfile->writeFile((char *)fileo.Q2CH);
    if (i <= 0)
   {
      cout << WRITEFILE_ERR;
      return -1;
   }
   delete lfile;
   return 1;

   str = "borrow Lic  OK->" + fileo;
   cout << str.Q2CH << endl;
   #endif
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
         if(i <0) str ="daveDB error when packDB2DB\n";
       
     }
     else  
         str = " packDB load error!! when packDB2DB\n";

     
     cout <<str.Q2CH;
    // plog(str);
     return i; 
}
// report====================================
// 
QStringList SLicMng::reportPackage()
{
    // name,size,task(limit,used),node(limit,used),user(limit,used),
    // name,ty,limit,start,end;
    int i,sz;
    QStringList packs,slist,rlist;
    QString pack ;
  
    packs = data->packNames();
    sz = packs.size();
    for (i = 0; i <sz; i++) 
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
    QString name,ty,size,str,limit,start,end,bmid;
    int len1,len2,len3;
    len1 = 10;
    len2 = 20;
    len3 = 25;
     
    mng = data->packMng(pack);

    if (mng == NULL) return slist;

    szp = mng->size(); 
        // packID sizeInfo limitTask used Nodelimit used UserLimit used
         
        str = QString("---------- %1 %2 task(%3,%4) node(%5,%6) user(%7,%8)").arg(pack,len3).arg(szp,len1).arg(mng->taskLimit()).arg(mng->taskUsed()).arg(mng->nodeLimit()).arg(mng->nodeUsed()).arg(mng->userLimit()).arg(mng->userUsed());
        slist << str;
      
        for (j = 0; j < szp; j++) 
        {
            info =(SPackInfo*) mng->get(j);
            ty = info->get(PTYPE).toString();
            limit = info->get(PLIMIT).toString();
            start = info->get(PSTARTDATE).toString();
            end = info->get(PENDDATE).toString();
            bmid = info->get(BMID).toString();
            str = QString("%1 %2 %3 %4 %5 %6").arg(pack,len3).arg(ty,len1).arg(limit,len1).arg(start,len3).arg(end ,len3).arg(bmid ,len3);;
            slist << str;
        }
 
    return slist;
}

QStringList SLicMng::reportApp()
{
        // name,size,task(limit,used),node(limit,used),user(limit,used),
    // name,ty,limit,start,end;
    int i,sz;
    QStringList packs,slist,rlist;
    QString name,ty,size,str,limit,start,end,pack;
  
    packs = data->packNames();
    sz = packs.size();
    for (i = 0; i <sz; i++) 
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
    int  j, szp,number;
    QStringList  slist;
    SAppMng *mng;
    SAppInfo *info;
    QString appid,user,ty,str,start;
    //long start;
    LFileDate fd;
   int len1,len2,len3;
    len1 = 10;
    len2 = 20;
    len3 = 25;
     
  
    mng = data->appMng(pack);

    if (mng == NULL) return slist;

    szp = mng->size(); 
      
        for (j = 0; j < szp; j++) 
        {
            info =(SAppInfo*) mng->get(j);
            appid = info->appid;
            user = info->user;
            start = fd.EP2DT(info->start);
            ty = info->rtype;
            number = info->number;
            str = QString("%1 %2 %3 %4 %5 %6").arg(pack,len3).arg(appid,len2).arg(user,len1).arg(number,len1).arg(ty,len1).arg(start,len3);
            slist << str;
        }
 
    return slist;
}
//======================================
/// if uuid exist in the DB: true no this uuid
bool SLicMng::isUuid(QString packid, QString uuid)
{
   return data->isUuid(packid, uuid);

}
/// if venderSign is the same with the venderName in the DB (),true if the same or no vendername in DB
bool SLicMng::isVenderSign(QString venderName, QString venderSign)
{
   return data->isVenderSign(venderName, venderSign);
}

