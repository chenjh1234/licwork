#include "SLicMng.h"

SLicData *SLicMng::data = new SLicData();
SLicMng::SLicMng()
{
   init();

}
SLicMng:: ~SLicMng()
{
    qDebug() << " end of licMng=======";
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

//
int SLicMng::loginApp(SAppInfo& msg)
{
   return data->addApp(msg);
}

int SLicMng::logoutApp(SAppInfo& msg)
{
   return data->rmApp(msg);
}

//
int SLicMng::heartBeat(QString vender, QString package, QString version, QString host, QString pid)
{
   return 0;
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
int SLicMng::checkLicFile(QString filename, QString serverPri)
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
   b = lic.isVenderKeyValid(serverPri, infoV);
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
   num  = licFile.size();
   //qDebug() << "packages = " << num;
   for (i = 0; i < num; i++)
   {
      infoP = licFile.package(i);
      b = lic.isPackageKeyValid(venderKey, infoP);
      if (!b) return  LOADFILE_PACKAGEKEY_ERR;
   }
   // checkDate:

   //??????????????
   return num;
}
//
int SLicMng::loadFile(QString filename, QString serverPri)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoP,*infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   bool b;
   num = checkLicFile(filename, serverPri);
   if (num <= 0)  return num;

// readfile:
   i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR; 
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();
   venderName = infoV->get(VENDERNAME).toString();
   lic.isVenderKeyValid(serverPri, infoV);// we do not if
   venderSign = lic.getVenderSign(); //  isVenderKeyValid make it available

// insert to Data:
   SPackInfo *infoSP;
   //SPackInfo *mngSP;
   QString packid, pname, version, borrow;
   int j;
   for (i = 0; i < num; i++)
   {
      infoP = licFile.package(i);
      infoSP = new SPackInfo();
      *((LInfo *)infoSP) = *infoP;
      // set infoSP:
      //remove:
      infoSP->remove(SERVERID);
      infoSP->remove(SERVERNAME);
      //get:
      pname = infoSP->get(PACKAGENAME).toString();
      version = infoSP->get(PVERSION).toString();
      borrow = infoSP->get(BORROW).toString();

      //set:
      infoSP->set(VENDERKEY, venderKey);
      infoSP->set(VENDERSIGN, venderSign);
      infoSP->set(BORROW, borrow);
      infoSP->set(UUID, uuid);

      // qDebug() << "uuid ===" <<  infoSP->getText();

      packid = data->encodePackageId(venderName, pname, version);
      infoSP->packid = packid;
      infoSP->limit =  infoSP->get(PLIMIT).toInt();
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
int SLicMng::unloadFile(QString filename, QString serverPri)
{
   int i, ret, num;
   LLicFile licFile;
   LLicEncrypt lic;
   LInfo *infoV;
   LEncrypt en;
   QString str, venderKey, pKey, venderSign, uuid, smid, venderName;
   num = checkLicFile(filename, serverPri);
   if (num <= 0)  return num;

   // readfile:
   i = licFile.readFile(filename);
   if (i <= 0) return LOADFILE_FILE_ERR; 
   infoV = licFile.vender();
   uuid = infoV->get(UUID).toString();

   ret = data->removePackage(uuid);
   return ret;
}

//
int SLicMng::borrow(QString vender, QString package, QString version, int number, QString mid)
{
   return 0;
}
//======================================
/// if uuid exist in the DB: true no this uuid
bool SLicMng::isUuid(QString packid, QString uuid)
{
   return data->isUuid(packid, uuid);

}
/// if venderSign is the same with the samevenderName in the DB (),true is the same or no vendername in DB
bool SLicMng::isVenderSign(QString venderName, QString venderSign)
{
   return data->isVenderSign(venderName, venderSign);
}

