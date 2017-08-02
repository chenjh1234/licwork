#include "SLicData.h"
SLicData ::SLicData()
{
   QString str;
   str = curLogName(ELOG);
   _elog.setFile(str);
   str = curLogName(ALOG);
   _alog.setFile(str);
   str = curLogName(PLOG);
   _plog.setFile(str);
#if 0
   _elog<< "1234567"<<endl;
   QString s;
   s = "56789111";
   _elog.tline("9999999");
   _elog.ts(s);
   s = "567890";
   elog(s);
   //_elog<< "1234567----"<<endl;
#endif

}
SLicData::~SLicData()
{
   qDebug() << " =======end of licData=======";
   _elog.close();
   _plog.close();
   _alog.close();
}
// ===================pack ===========

int SLicData::packSize()
{
   return mapPack.size();
}
/// package manager identified by pack id:
SPackMng* SLicData::packMng(QString packid)
{
   if (mapPack.contains(packid))  return mapPack[packid];
   else return NULL;
}
/// package names in this db
QStringList SLicData::packNames()
{
   QStringList slist;
   slist = mapPack.keys();
   return slist;
}
/// removed packge size;in this session
int SLicData::removedSize()
{
   return packRemoved.size();
}
QStringList SLicData::removedPackages()
{
   QString str;
   QStringList slist;
   int i;
   for (i = 0; i < removedSize(); i++)
   {
      str = ((SPackInfo *)packRemoved.get(i))->packid;
      slist << str;

   }
   return slist;
}
//------------ -----------------------app-----------------
/// all apps in the DB
int SLicData::appSize()
{
   //return mapApp.size();
   int i, sz, ret;
   sz = mapApp.size();
   QStringList slist;
   slist = mapApp.keys();
   ret = 0;
   for (i = 0; i < sz; i++)
   {
      ret += mapApp[slist[i]]->size();

   }
   return ret;
}
/// app managers
SAppMng* SLicData::appMng(QString packid)
{
   if (mapApp.contains(packid))  return mapApp[packid];
   else return NULL;
}
/// app packs
QStringList SLicData::appPacks()
{
   QStringList slist;
   slist = mapApp.keys();
   return slist;
}
int SLicData::appPackSize()
{
   return mapApp.size();
}
//=================================borrow=====================
int SLicData::borrow(SPackInfo *pack)
{
   QString str, packid, dt;
   // found package for borrow
   /// packid,borrow,type,uuid,limit,date,left
   int i, sz, left;
   SPackMng *mng;
   mng = packMng(pack->packid);
   if (mng == NULL)
   {
      pack->ret = -1;
      pack->err = " Error : cant not found packid:" + pack->packid;
      return -1;
   }
   // fonnd mng:
   sz = mng->size();
   SPackInfo *info;
   dt = _dt.curDT();

   for (i = 0; i < sz; i++)
   {
      info = (SPackInfo *)mng->get(i);
      if (info == NULL) continue;
      if (info->get(BORROW).toString() != BORROW_YES) continue;
      if (info->get(PTYPE).toString() != pack->get(PTYPE).toString()) continue;
      if (info->get(UUID).toString() != pack->get(UUID).toString()) continue;
      if (info->limit <= pack->limit) continue;
      if (dt < pack->get(PSTARTDATE).toString() || dt > pack->get(PENDDATE).toString()) continue; // today:
      if (pack->get(PSTARTDATE).toString() < info->get(PSTARTDATE).toString() ||  pack->get(PENDDATE).toString() > info->get(PENDDATE).toString()) continue; // today:
      left = (mng->getPResource(pack->get(PTYPE).toString()))->limit - (mng->getPResource(pack->get(PTYPE).toString()))->used;
      if (left >= pack->limit)
      {
         // yes: found it:
         //limit = limit *-1
         str = pack->get(PLIMIT).toString();
         str = "-" + str;
         pack->set(PLIMIT, str);
         pack->limit = -1 * pack->limit;
         // vendersign:
         str = info->get(VENDERSIGN).toString();
         pack->set(VENDERSIGN, str);
         // pborrow:
         str = info->get(PBORROW).toString();

         pack->set(PBORROW, str);
         // add a -limit pacakge:
         left = addPackage(pack);
         if (left < 0)
         {
            pack->err = " Error : cant not found package that have enouph license left:" + pack->packid;
         }
         // log:
         pack->ret = left;
         plog(pack, "borrow");
         return left;
      }
   }
   pack->ret = -1;
   pack->err = " Error : cant not found package that have enouph license left:" + pack->packid;
   plog(pack, "borrow");
   return -2;
}
int SLicData::borrowReturn(QString filen)
{
   int i, ir;
   SPackInfo *info;
   QString packID;
   info = NULL;

   i = verifyProof(filen, info);
   qDebug() << "verify return = " << i << info;
   ir = i;
   if (i > 0)
   {
      if (info != NULL)
      {
         packID = info->packid;
         ir = removePackage(packID, info);
      }

   }
   return ir;
}

// =======add package==============interface=======
/// add a package;
int SLicData::addPackage(SPackInfo *info) // base add
{

   QString packid, str;
   SPackMng *mng;
   int i;
   i = 0;
   // lock;
   _lockPack.lock();
   packid = info->packid;
   //qDebug() << "addPackage ,nameid = " << nameid;
   if (isPackidInPack(packid))
   {
      qDebug() << "addPackage yes= " << packid;
      mng = mapPack[packid];
      if (mng != NULL)
      {
         i = mng->addInfo(info);
      }
   }
   else // new package:
   {
      qDebug() << "addPackage no= " << packid;
      mng = new SPackMng();
      i = mng->addInfo(info);
   }
   // add to map:
   mapPack[packid] = mng;
   // unlock:
   _lockPack.unlock();
   //if (i <=0 ) info->err = "addPackage err";
   info->ret = i;
   plog(info, "addPackage");
   if (i > 0)  i = saveDBPackage();
   return i;

}
//-------unload:  packInfo * from packMng:--internal use
/// return i  index of info in mng <0 err
int SLicData::unloadPackage(QString packid, SPackInfo *info) // base remove
{
   int i;
   SPackMng *mng;
   QStringList slist;
   // lock:
   _lockPack.lock();
   // lock:
   //_lockPack.lock();
   mng =  mapPack[packid];
   i = mng->unloadInfo(info);
   //qDebug() << "unloadP return ==" << i;
   info->ret = i;
   plog(info, "unloadPackage");
   _lockPack.unlock();
   if (i >= 0)  i = saveDBPackage();
   return i; // index of info in mng <0 err
}
//-------remove packInfo * from packMng:--internal use
/// return i  index of info in mng <0 err
int SLicData::removePackage(QString packid, SPackInfo *info) // base remove
{
   int i;
   SPackMng *mng;
   QStringList slist;
   // lock:
   _lockPack.lock();
   // lock:
   //_lockPack.lock();
   mng =  mapPack[packid];
   //qDebug() << "removeP mng =" << mng << packid<< mng->size();
   i = mng->removeInfo(info);
   qDebug() << "removeP return ==" << i;
   if (i >= 0)
   {
      //info->nameid = packid;
      packRemoved.add(info); // move to packRemoved
      if (mng->size() == 0)
      {
         mapPack.remove(packid); // remove this mng from map if mng.size() ==0
         delete mng; // remove the mng pointer as well
      }
   }
   // unlock
   _lockPack.unlock();
   if (i == 0) i = 1;
   info->ret = i;
   plog(info, "removePackage");
   if (i >= 0)  i = saveDBPackage();
   return i; // index of info in mng <0 err
}
//----------remove uuid---------for unload license file------------------------------
/// remove info from uuid: return ret removed info
/// remove info from uuid: return ret removed info
int SLicData::removePackage(QString uuid)
{
   int i, sz, ret, ret1;
   QMap<QString, SPackInfo *> umap;
   SPackInfo *info;
   QStringList slist;
   QString err, str;

   umap = findPackageInfo(UUID, uuid);

   sz = umap.size();
   ret = sz;

   //qDebug() << "umap.sz = " << sz;
   if (sz > 0)
   {
      slist = umap.keys();
      //qDebug()<< "find uuid ,slist= " << slist ;
      for (i = 0; i < sz; i++) // all uuid in different mng;
      {
         info = umap[slist[i]];
         ret1 = removePackage(slist[i], info);
         //qDebug()<< "inloop " << i << ret;
         if (ret1 < 0)
         {
            ret = i;
            //info->ret = ret1;
            //info->err += "removePackage(QString uuid) packid = " + slist[i];
            //plog(info);
            break;
         }
         else
         {
            // plog in upper subs
            // str = "OK unloadPackage(QString uuid) packid = " + slist[i];
            //plog(str);
         }
      } //

   }
   else
   {
      err = "Error:  removePackage(QString uuid): not find uuid";
      plog(err);
   }
   // log
   if (ret == sz && ret > 0)
   {
      err = "OK:  removePackage(QString uuid): is OK removed package number = " + QString("%1 ").arg(sz) + uuid;
      plog(err);
   }
   else
   {
      err = "ERR:  removePackage(QString uuid): is Err removed package number = " + QString("%1 ").arg(ret) + uuid;
      plog(err);
   }
   return ret;
}
int SLicData::unloadPackage(QString uuid, QString filen)
{
   int i, sz, ret, ret1;
   QMap<QString, SPackInfo *> umap;
   SPackInfo *info;
   QStringList slist;
   QString err, str;
   QString filenn;

   umap = findPackageInfo(UUID, uuid);

   sz = umap.size();
   ret = sz;
   int ir;

   // qDebug() << "umap.sz = " << sz;
   if (sz > 0)
   {
      slist = umap.keys();
      //qDebug()<< "find uuid ,slist= " << slist ;
      for (i = 0; i < sz; i++) // all uuid in different mng;
      {

         info = umap[slist[i]];
         // qDebug() << i;<< info;
         ret1 = unloadPackage(slist[i], info);
         //qDebug()<< "inloop " << i << ret1;
         if (ret1 < 0)
         {
            ret = i;
            //info->ret = ret1;
            //info->err += "removePackage(QString uuid) packid = " + slist[i];
            //plog(info);
            break;
         }
         else
         {
            // plog in upper subs
            // str = "OK unloadPackage(QString uuid) packid = " + slist[i];
            //plog(str);
            if (i == 0) filenn = filen;
            else filenn = filen + QString("%1").arg(i);
            ir = createProof(filenn, info);
            //qDebug() << ir;
            if (ir < 0)
            {
               ret = i;
               break;
            }
         }
      } //for

   }
   else
   {
      err = "Error:  unloadPackage(QString uuid): not find uuid";
      plog(err);
   }
   //qDebug() << "unload out";
   // log
   if (ret == sz && ret > 0)
   {
      err = "OK:  unloadPackage(QString uuid): is OK removed package number = " + QString("%1 ").arg(sz) + uuid;
      plog(err);
   }
   else
   {
      err = "ERR:  unloadPackage(QString uuid): is Err removed package number = " + QString("%1 ").arg(ret) + uuid;
      plog(err);
   }

   return ret;
}
/// inf:create unloaded package proof file,usally for borrow In package ,unalod and return to the borrow Out server
int SLicData::createProof(QString filen, SPackInfo *inf)
{
   int  ret, len;
   QString packID, ty, limit, start, end, sinID, soutID, uuid, sign;
   QString proof, enProof;
   LEncrypt en;
   char buf[20480];

   packID = inf->packid;
   ty = inf->get(PTYPE).toString();
   limit = inf->get(PLIMIT).toString();
   start = inf->get(PSTARTDATE).toString();
   end = inf->get(PENDDATE).toString();
   sinID = inf->get(SERVERID).toString();
   soutID = inf->get(BMID).toString();
   uuid = inf->get(UUID).toString();
   sign = inf->get(VENDERSIGN).toString();
   printf("11111\n");
   proof = packID + DELIMIT_PROOF + ty + DELIMIT_PROOF + limit + DELIMIT_PROOF + start + DELIMIT_PROOF + end + DELIMIT_PROOF + sinID + DELIMIT_PROOF + soutID + DELIMIT_PROOF + uuid + DELIMIT_PROOF + sign;
   len = en.encrypt(proof.Q2CH, proof.length(), buf, PASSWD_PROOF);
   if (len <= 0)  return -1; // encrypt failed
   qDebug() << proof;
//geoeast_pc1_1.0%%task%%2%%20170101%%20180101%%73947D61-D163D96C-03295769-F3BF5EB7%%73947D61-D163D96C-03295769-F3BF5EB7%%df3d81d2-2484-45d3-8bb4-79d161691cf3%%0B2C366330A53B2FE2B17FD57103BE69615166AB47FAC728B243E5991F5265C41FB8BB48BC9F76EE649E05A29871EDF75EF52F0431F7270BF2FA8446C60A5F8BEF664386890BC4BBB7F92591B63AFA73E443ABC2B223B61B9481CF9CD99DEEAC2D37391BFF3429BDC83E4C96B438FAF70DA35475C3E87581CEF1E3D8DD8CF5F8
//qDebug() <<  "encrypt proof len = " << proof.length() << len;
//printf("22222\n");
// encrypt OK:

   QFile file(filen);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -2;

   ret = file.write(buf, len);
   if (ret != len)  return -3;

   //qDebug() <<  "write proof len = " <<  ret;

   file.close();
   ret = 1;
   return ret;
}
/// inf:found the Package borrowOut,for borrow return;
int SLicData::verifyProof(QString filen, SPackInfo *& retInfo)
{
   int   i, len;
   QString packID, ty, limit, start, end, sinID, soutID, uuid, sign;
   QString proof, err;
   LEncrypt en;
   int ret;


   char buf[20480];
   char bufo[20480];

   QFile file(filen);
   if (!file.open(QIODevice::ReadOnly)) return -1;
   len = file.size();
   //qDebug() << "files size =  = " << len << filen;

   i = file.read(buf, len);
   if (i != len)  return -2;
   file.close();
   //qDebug() << "len = " << len;
   i = en.decrypt(buf, len, bufo, PASSWD_PROOF);
   if (i <= 0) return -3;
   len = i;
// decrypt ok;
   bufo[len] = 0; // truncate;
   proof = bufo;
   //qDebug() << "decrypt proof = " << proof;

   QStringList slist;
   slist = proof.split(DELIMIT_PROOF);
   i = 0;
   packID = slist[i]; i++;
   ty = slist[i]; i++;
   limit = slist[i]; i++;
   start = slist[i]; i++;
   end = slist[i]; i++;
   sinID = slist[i]; i++;
   soutID = slist[i]; i++;
   uuid = slist[i]; i++;
   sign = slist[i]; i++;
// found the rigth info:

   SPackMng *mng;
   SPackInfo *info;
   QString  ty1,  start1, end1, sinID1, soutID1, uuid1, sign1;


   i = 0;
   ret = -1;
   err = "";
   // found the rigth info:
   if (isPackidInPack(packID))
   {
      mng = mapPack[packID];
      if (mng != NULL)
      {
         //qDebug() << "mngSize = " << mng->size();
         for (i = 0; i < mng->size(); i++)
         {
            info = (SPackInfo *)mng->get(i);
            if (info == NULL) break;
            //qDebug() << "limit = " << limit.toInt() << info->limit << info->isBorrowOut() << info->get(BMID).toString();
            if (limit.toInt() == (-1 * info->limit) && info->isBorrowOut())
            {
               ty1 = info->get(PTYPE).toString();
               start1 = info->get(PSTARTDATE).toString();
               end1 = info->get(PENDDATE).toString();
               sinID1 = info->get(BMID).toString();
               soutID1 = info->get(SERVERID).toString();
               uuid1 = info->get(UUID).toString();
               sign1 = info->get(VENDERSIGN).toString();
               qDebug() << "found info = " << packID << ty1 << start1 << end1 << sinID1 << soutID1 << uuid1 << sign1;
               if (ty == ty1 && start == start1 && end == end1 && sinID == soutID1 && soutID == soutID1 && uuid == uuid1 && sign == sign1)
               {
                  ret = 1;
                  retInfo = info;
                  break;
               }
            }
            else continue;
         }
      }
      else err = " packMng is NULL";
   }
   else err = " no this packID = " + packID;
   if (ret == -1)
   {
      retInfo = NULL;
      err = "verify proof : " + err;
      plog(err);
   }
   else
   {
      retInfo = info;
      plog(info, "verifyProof");
   }

   qDebug() << "verify proof err = " << err << info << retInfo;

   return ret;
}
//============================================service functions===================
/// if packid in exist in the mapPack
bool SLicData::isPackidInPack(QString packid)
{
   if (mapPack.contains(packid)) return true;
   else return false;
}
/// if packid in exists in the mapApp
bool SLicData::isPackidInApp(QString packid)
{
   if (mapApp.contains(packid)) return true;
   else return false;
}

/// found msg = name  in PackInfo in the PackMng that have the same packid
QMap<QString, SPackInfo *> SLicData::findPackageInfo(QString msg, QString value)
{
   SPackMng *mng;
   SPackInfo *info;
   QStringList slist;
   QString v;
   QMap<QString, SPackInfo *> imap;

   int sz, i, j;
   sz = mapPack.size();
   slist = mapPack.keys();
   //qDebug() << "isVenderSign sz = " << sz;
   if (sz == 0)  return imap; // empty

   for (i = 0; i < sz; i++) // found all SPackMng in map
   {
      mng = mapPack[slist[i]];
      j = mng->find(msg, value);
      if (j >= 0)
      {
         info = (SPackInfo *)mng->get(j);
         imap[slist[i]] = info;
         //qDebug() << "info = " << info;
      }
   }
   return imap;
}

/// found msg = name  in PackInfo in the PackMng that have the same packid
bool SLicData::isPackageMsgFind(QString packid, QString msg, QString value)
{
   // infact it for uuid only:
   SPackMng *mng;
   int i;
   if (!isPackidInPack(packid)) return false;
   mng = mapPack[packid];
   i = mng->find(msg, value);
   //qDebug()<< "msg = " << i;
   if (i >= 0) return true;
   return false;
}
/// if uuid is exist in the packageMng in map
bool SLicData::isUuid(QString packid, QString uuid)
{
   QString msg;
   msg = UUID;
   return  !isPackageMsgFind(packid, msg, uuid); // if found return false ,not
}
/// if venderSign is the same with the exists in the packageMng
bool SLicData::isVenderSign(QString vendername, QString venderSign)
{
   SPackMng *mng;
   SPackInfo *info;
   QStringList slist;
   QString sign, vname;

   int sz, i;
   sz = mapPack.size();
   slist = mapPack.keys();
   //qDebug() << "isVenderSign sz = " << sz;
   if (sz == 0)  return true; // no this vender:

   for (i = 0; i < sz; i++) // found all SPackMng in map
   {
      mng = mapPack[slist[i]]; // in the same packageID, get only one ,all the vendersign in this mng will be the same
      info = (SPackInfo *)mng->get(0);
      sign = info->get(VENDERSIGN).toString();
      vname = info->get(VENDERNAME).toString();
      // qDebug() << "sign = " << sign << venderSign;
      if (vname == vendername)
      {
         if (sign == venderSign) return true;
         else return false;
      }

   }
   return true; // not found;

}
/// packid = vender + package + version:
QString SLicData::encodePackageId(QString vender, QString package, QString version)
{
   QString str, dl;
   dl = "_";
   str = vender + dl + package + dl +   version;
   return str;
}
/// appid = ip+pid;
QString SLicData::encodeAppId(QString ip, QString pid)
{
   QString str, dl;
   dl = "_";
   str = ip + dl + pid;
   return str;
}
//=app =================== =register APP==============================================
/// appInfo request Package resources;
int SLicData::registerApp(SAppInfo& info)
{
   int i;
   QString packid, err;
   SPackMng *mng;
   SAppMng *appmng;
   packid = info.packid;
   qDebug() << "packid=" << packid;

   i = -1;
   if (isPackidInPack(packid))
   {
      if (isPackidInApp(packid)) // is packid in appMng
      {
         appmng = mapApp[packid];
      }
      else
      {
         appmng = NULL;
      }
      //
      mng = mapPack[packid];
      //qDebug() << "appReuest0" << appmng;

      i = mng->appRequest(info, appmng);
      //qDebug() << " i = mng->appRequest(info,appmng)" << i;
   }
   else // no this package
   {
      err = "registerApp Error:no this package name = " + packid;
      info.err = err;

   }
   //qDebug() << "222";
   return i;
}
//--------------------------------------------------release App----
/// appInfo release the  Package resources;
int SLicData::releaseApp(SAppInfo& info)
{

   int i;
   QString packid;
   SPackMng *mng;
   SAppMng *appmng;
   packid = info.packid;
   qDebug() << "packid=" << packid;

   i = -1;
#if 0   // we donot care if apckid exist or removed:
   if (!isPackidInPack(packid))
   {
      err = "releaseApp Error: no this package in packMng name = "+packid;
      info->err = err;
   }
#endif
   if (isPackidInApp(packid)) // is a registerd packid
   {
      appmng = mapApp[packid]; // appmng
      mng = mapPack[packid];   // packmng;
      i = mng->appRelease(info, appmng);
   }
   else
   {
      info.err  = "releaseApp Error: no this package in appMap name = " + packid;
   }
   return i;
}
//===========================add app===========================
/**
 * @brief
 * 1: new a SAppinfo ,and  copy the info data;
2:registerApp(info *)
3:insert the info to appMng

 
 * @return int 
 */
int SLicData::addApp(SAppInfo& ainfo)
{
   int i, number;
   LFileDate fd;
   QString str;
   QString vender, package, version, packid, pid, ip, user;
// check App info
   SAppInfo *info;
   SAppMng *mng;
   mng = NULL;

   info = new  SAppInfo();
   *info = ainfo;

   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   version = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();
   //qDebug() << "packid00=" << vender << package << version ;
   qDebug() << "info=" << info->getText();
   // qDebug() << "ainfo=";// << ainfo.getText();
   packid = encodePackageId(vender, package, version);

   info->packid = packid;
   info->number = number;
   info->appid = encodeAppId(ip, pid);
   info->user = user;
   info->nameid = info->appid;
   info->start = fd.sEP();
   // qDebug() <<  "i0 =registerApp( *info); =" << i;

   i = registerApp(*info);

   //qDebug() <<  "i1 =registerApp( *info); =" << i << packid << info->appid;
   if (i >= 0)
   {
// add to map:
      _lockApp.lock();
      //info->startMS =
      info->number = number;
      // qDebug() << "return = " << i;

      if (isPackidInApp(packid))
      {
         mng = mapApp[packid];
         if (mng != NULL)
         {
            i = mng->add(info);
         }

      }
      else
      {
         mng = new SAppMng();
         i = mng->add(info);
         mapApp[packid] = mng;
      }
      _lockApp.unlock();
      //qDebug() << "return = " << i;
   }
   else // failed
   {
      info->ret = i;
      elog(info);
      delete info;
      info = NULL;
   }

   //qDebug()<< "info   ==" << info->appid << info->err;
//qDebug() << "return1 = " << i;
   if (info != NULL)
   {
      info->ret = i;
      info->end = fd.sEP();

      elog(info);
   }
   //qDebug() << "return2 = " << i;
   return i; // >=0 OK <0 err;
}
/// get packid from input AppInfo, get and remove  *info from appMng ,and call releaseAPP;
int SLicData::rmApp(SAppInfo& ainfo)
{
   int i, number;
   QString str, err;
   QString vender, package, version, packid, pid, ip, user;
// check App info
   SAppInfo * info,*rmInfo;
   SAppMng *mng;
   mng = NULL;
   err = "";

   info = &ainfo;


   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   version = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();
   //qDebug() << "packid00=" << vender << package << version ;
   // qDebug() << "info=" << info->getText();
   // qDebug() << "ainfo=" << ainfo.getText();
   packid = encodePackageId(vender, package, version);
// packid,appid
   info->packid = packid;
   info->number = number;
   info->appid = encodeAppId(ip, pid);
   info->nameid = info->appid;

   _lockApp.lock();
   i = -1;


   rmInfo = (SAppInfo *)((LInfoMng *)(mapApp[info->packid]))->get(info->appid);
   if (rmInfo != NULL) // found it:
   {
      i = ((LInfoMng *)(mapApp[info->packid]))->rmInfo(rmInfo);
      if (i < 0)
      {
         err = QString("Error: cannot remove Info form appMng; ") + " appid= " + info->appid + " packid = " + info->packid;
      }
      else
      {
         i = releaseApp(*rmInfo);
         err = "Error: releaseApp(*rmInfo)" + rmInfo->err + " appid= " + info->appid + " packid = " + info->packid;
      }

   }
   else
   {
      err = "Error: rmApp: not found appid = " + info->appid + " packid = " + info->packid;
   }

   _lockApp.unlock();

   qDebug() <<  "i =releaseApp( *info); =" << i << packid;

   //info->err += err;

   if (i < 0)
   {
      elog(err);
   }
   else
   {
      alog(rmInfo);
      delete rmInfo; // remove the appInfo if ok;
   }
   return i; // >=0 OK <0 err;
}
// ============log==========================================================
/// plog info
void SLicData::plog(SPackInfo *info, QString fun)
{
   QString str;
   if (info->ret <= 0)
   {

      str = "Error:" +  info->packid + ":" + info->err;
      _plog.ts(str);
   }
   else
   {
      str =  fun + " OK:" + info->packid + " " + info->get(PTYPE).toString() + " " + info->get(PLIMIT).toString() + " " + info->get(PSTARTDATE).toString() + " " + info->get(PENDDATE).toString() + " " + info->get(UUID).toString();
      _plog.ts(str);
   }
}
/// plog string
void SLicData::plog(QString s)
{
   _plog.ts(s);
}
//------------------------
/// elog string for app
void SLicData::elog(QString s)
{
   _elog.ts(s);
}
/// elog info;
void SLicData::elog(SAppInfo *info, QString fun)
{
   if (info->ret > 0) return;

   QString str;
   str = "Error: " +  info->packid + " " + info->appid + " " + info->user + " " + info->err;
   //qDebug() << "elog = " <<str ;
   _elog.ts(str);
}

//---------alog---------------
/// app log :when app succsess complete

void SLicData::alog(SAppInfo *info)
{
   _lockLog.lock();
   QString str;
   str = _dt.curDT();
   _alog << str;
   _alog << info->packid << info->appid << info->user << info->start << info->end << info->end - info->start << info->number << info->rtype << endl;
   _lockLog.unlock();
}
//====================================saveDB==============loadDB============
#if 0
int SLicData::saveDB()
{
   int sz, i;
   QString fileDB;
   QStringList slist;

   fileDB = getDBFile();
   //qDebug() << " save app ok0=" << _dt.TID(fileDB);
   QFile file(fileDB);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -1;

   QDataStream ds(&file);
// pack:
   SPackMng *pmng;
   slist = packNames();
   ds << slist;

   sz = packSize();
   for (i = 0; i < sz; i++)
   {
      pmng = mapPack[slist[i]];
      pmng->encode(ds);
   }
//qDebug() << " save pack ok sz = " << sz;
// app:
//qDebug() << " save app ok1=" << _dt.TID(fileDB);
   SAppMng *amng;
   slist = appPacks();
   ds << slist;

   sz = appPackSize();
   //qDebug() << "appsize = " <<sz;
   for (i = 0; i < sz; i++)
   {
      //  qDebug() << i << slist[i];
      amng = mapApp[slist[i]];
      // qDebug() << i << slist[i] << amng;
      if (amng != NULL)  amng->encode(ds);
   }
// close
//ds.flush();
   file.flush();
   file.close();
   qDebug() << " save app ok=" << _dt.TID(fileDB);
   return saveDBMsg();
}
#endif
#if 1 // new one
int SLicData::saveDB()
{

   QString fileDB;
   QStringList slist;

   fileDB = getDBFile();
   //qDebug() << " save app ok0=" << _dt.TID(fileDB);
   QFile file(fileDB);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -1;

   QDataStream ds(&file);
   saveDBPack(ds);
   saveDBApp(ds);

// close
//ds.flush();
   file.flush();
   file.close();
   //qDebug() << " save app ok=" << _dt.TID(fileDB);
   return saveDBMsg();
}
#endif

int SLicData::saveDBPack(QDataStream& ds)
{
   int sz, i;
   QStringList slist;

// pack:
   SPackMng *pmng;
   slist = packNames();
   ds << slist;

   sz = packSize();
   for (i = 0; i < sz; i++)
   {
      pmng = mapPack[slist[i]];
      pmng->encode(ds);
   }
   return sz;
}
int SLicData::saveDBApp(QDataStream& ds)
{
   int sz, i;
   QStringList slist;

   SAppMng *amng;
   slist = appPacks();
   ds << slist;

   sz = appPackSize();
   //qDebug() << "appsize = " <<sz;
   for (i = 0; i < sz; i++)
   {
      //  qDebug() << i << slist[i];
      amng = mapApp[slist[i]];
      // qDebug() << i << slist[i] << amng;
      if (amng != NULL)  amng->encode(ds);
   }
   return sz;
}
#if 0
int SLicData::saveDBMsg()
{
   int sz;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();

   QFile ptr(filePtr);

   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
//qDebug() << " save DBMsg";
   fpath = fi.absoluteFilePath();
   fmds5 = _dt.mds5(fileDB);       // chang the TID of the file
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
   fcdate = _dt.TID(fileDB);

#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
   //open:

   if (!ptr.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -2;
   QDataStream ds(&ptr);
   // msg of db:
   ds << fpath;
   ds << fcdate;
   ds << fmds5;
   ds << finode;
   ds << fsz;
   ptr.flush();
   // my inode:
   QFileInfo fii(filePtr);
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);
   //msg of ptr:
   ds << fpath;
   ds << finode;
   QString randT;
   randT = _dt.randText10();
   ds << randT; //make the ptr file rand length;
   fcdate = _dt.TID(filePtr);
   ds << fcdate;
   ptr.flush();
   sz = ptr.size();
   ds << sz + 4;
   ptr.close();
// ptr Mds5 save:
   qDebug() << "date db end0= " <<  _dt.TID(fileDB) << _dt.TID(filePtr);
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME, SAPP_NAME);
   st.setValue(DB_PTR, fmds5);
   qDebug() << "date db end = " <<  _dt.TID(fileDB) << _dt.TID(filePtr);

   return sz + 4;
}
#endif
#if 1
int SLicData::saveDBMsg()
{

   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();
   return saveDBMsg(fileDB, filePtr);
}
#endif
int SLicData::saveDBMsg(QString fileDB, QString filePtr)
{
   int sz;
   QStringList slist;

   QFile ptr(filePtr);
// dbMsge:
   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
//qDebug() << " save DBMsg";
   fpath = fi.absoluteFilePath();
   fmds5 = _dt.mds5(fileDB);       // chang the TID of the file
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
   fcdate = _dt.TID(fileDB);

#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
   //open:
//save to PTR:
   if (!ptr.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -2;
   QDataStream ds(&ptr);
   // msg of db:
   ds << fpath;
   ds << fcdate;
   ds << fmds5;
   ds << finode;
   ds << fsz;
   ptr.flush();
   // my inode:
   QFileInfo fii(filePtr);
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);
   //msg of ptr:
   ds << fpath;
   ds << finode;
   QString randT;
   randT = _dt.randText10();
   qDebug() << "rndt = " << randT.length() << randT;
   ds << randT; //make the ptr file rand length;
   fcdate = _dt.TID(filePtr);
   ds << fcdate;
   ptr.flush();
   sz = ptr.size();
   ds << sz + 4;
   ptr.close();
// save to EN:
// ptr Mds5 save:
//qDebug() << "date db end0= " <<  _dt.TID(fileDB) << _dt.TID(filePtr);
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME, SAPP_NAME);
   st.setValue(filePtr, fmds5);
   //qDebug() << "date db end = " <<  _dt.TID(fileDB) << _dt.TID(filePtr);

   return sz + 4;
}
#if 0
int SLicData::loadDBMsg()
{
   int sz;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();

   QFile ptr(filePtr);

   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
   // ptr Mds5 load:
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME, SAPP_NAME);
   fsz = st.value(DB_PTR, fmds5).toString();
   if (fsz != fmds5) RET("DB_PTR fileContains error");

   fpath = fi.absoluteFilePath();
   fcdate = _dt.TID(fileDB);
   fmds5 = _dt.mds5(fileDB);
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
   //open:
   int ilen;
   QString str;
   if (!ptr.open(QIODevice::ReadOnly)) return -2;
   QDataStream ds(&ptr);
   ds >> str;
   if (str != fpath) RET("DB filePath error");
   ds >> str;

   ilen = abs(_dt.diffDT(str, fcdate));
   qDebug() << " DB Date = " << str << fcdate << ilen;

   //if(str != fcdate) RET("DB fileDate error");
   if (ilen > DBDATE_MIS) RET("DB fileDate error");
   ds >> str;
   if (str != fmds5) RET("DB fileContains error");
   ds >> str;
   if (str != finode) RET("DB fileInode error");
   ds >> str;
   if (str != fsz) RET("DB fileSize error");

   QFileInfo fii(filePtr);
   int i;
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);
   fcdate = _dt.TID(filePtr);
   sz = ptr.size();

   ds >> str;
   if (str != fpath) RET("DB_Ptr filePath error");
   ds >> str;
   if (str != finode) RET("DB_Ptr fileInode error");
   ds >> str; // randText10: make the ptr file rand length;
   ds >> str;
   //ilen = abs(str.toLong() - fcdate.toLong());
   ilen = abs(_dt.diffDT(str, fcdate));
   qDebug() << " ptr Date = " << str << fcdate << ilen;

   if (ilen > DBDATE_MIS) RET("DB_ptr fileDate error");
   //if(str != fcdate) RET("DB_Ptr fileDate error");

   ds >> i;
   if (i != sz) RET("DB_Ptr fileSize error");

   ptr.close();
   return sz;
}
#endif
#define RET(x) {cout << x <<endl; return -1;}
int SLicData::loadDBMsg()
{

   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();
   return loadDBMsg(fileDB, filePtr);
}
int SLicData::loadDBMsg(QString fileDB, QString filePtr)
{
   int sz;
   //  QString fileDB, filePtr;
   QStringList slist;

   //  fileDB = getDBFile();
   //  filePtr = getDBFileIndex();

   QFile ptr(filePtr);

   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
   // ptr Mds5 load:
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME, SAPP_NAME);
   fsz = st.value(filePtr, fmds5).toString();
   if (fsz != fmds5) RET("DB_PTR fileContains error");

   fpath = fi.absoluteFilePath();
   fcdate = _dt.TID(fileDB);
   fmds5 = _dt.mds5(fileDB);
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
//open:
   int ilen;
   QString str;
   if (!ptr.open(QIODevice::ReadOnly)) return -2;
   QDataStream ds(&ptr);
   ds >> str;
   if (str != fpath) RET("DB filePath error");
   ds >> str;

   ilen = abs(_dt.diffDT(str, fcdate));
   qDebug() << " DB Date = " << str << fcdate << ilen;

   //if(str != fcdate) RET("DB fileDate error");
   if (ilen > DBDATE_MIS) RET("DB fileDate error");
   ds >> str;
   if (str != fmds5) RET("DB fileContains error");
   ds >> str;
   if (str != finode) RET("DB fileInode error");
   ds >> str;
   if (str != fsz) RET("DB fileSize error");

   QFileInfo fii(filePtr);
   int i;
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);
   fcdate = _dt.TID(filePtr);
   sz = ptr.size();

   ds >> str;
   if (str != fpath) RET("DB_Ptr filePath error");
   ds >> str;
   if (str != finode) RET("DB_Ptr fileInode error");
   ds >> str; // randText10: make the ptr file rand length;
   ds >> str;
   //ilen = abs(str.toLong() - fcdate.toLong());
   ilen = abs(_dt.diffDT(str, fcdate));
   qDebug() << " ptr Date = " << str << fcdate << ilen;

   if (ilen > DBDATE_MIS) RET("DB_ptr fileDate error");
   //if(str != fcdate) RET("DB_Ptr fileDate error");

   ds >> i;
   if (i != sz) RET("DB_Ptr fileSize error");

   ptr.close();
   return sz;
}

#if 0 // old one
int SLicData::loadDB()
{
   int sz, i;
   QString fileDB;
   QStringList slist;
   if (!isDBRegisted())
   {
      i = registerDB();
      return i;
   }
   i = loadDBMsg();
   if (i <= 0)  return -1;

   fileDB = getDBFile();
   QFile file(fileDB);
   if (!file.open(QIODevice::ReadOnly)) return -1;
   QDataStream ds(&file);
// pack:
   SPackMng *pmng;

   ds >> slist;
   sz = slist.size();
   //qDebug() << "slist in load = " << slist;
   for (i = 0; i < sz; i++)
   {
      pmng = new SPackMng();
      pmng->decode(ds);
      //qDebug() << "inloop = " << i <<   pmng->size() << pmng->getText();
      mapPack[slist[i]] = pmng;
   }

// app:
   SAppMng *amng;
   ds >> slist;

   sz = slist.size();
   for (i = 0; i < sz; i++)
   {
      amng = new SAppMng();
      amng->decode(ds);
      mapApp[slist[i]] = amng;
   }
// close
   file.close();
   return 1;

}
#endif
#if 1 // new one
int SLicData::loadDB()
{
   int   i;
   QString fileDB;
   QStringList slist;
   if (!isDBRegisted())
   {
      i = registerDB();
      return i;
   }
   i = loadDBMsg();
   if (i <= 0)  return -1;

   fileDB = getDBFile();
   QFile file(fileDB);
   if (!file.open(QIODevice::ReadOnly)) return -1;
   QDataStream ds(&file);
   loadDBPack(ds);
   loadDBApp(ds);
// close
   file.close();
   return 1;

}
#endif
int SLicData::loadDBPack(QDataStream& ds)
{
   int sz, i;
   QString fileDB;
   QStringList slist;
// pack:
   SPackMng *pmng;

   ds >> slist;
   sz = slist.size();
   //qDebug() << "slist in load = " << slist;
   for (i = 0; i < sz; i++)
   {
      pmng = new SPackMng();
      pmng->decode(ds);
      //qDebug() << "inloop = " << i <<   pmng->size() << pmng->getText();
      mapPack[slist[i]] = pmng;
   }
   return sz;
}
int SLicData::loadDBApp(QDataStream& ds)
{
   int sz, i;
   QString fileDB;
   QStringList slist;

// app:
   SAppMng *amng;
   ds >> slist;

   sz = slist.size();
   for (i = 0; i < sz; i++)
   {
      amng = new SAppMng();
      amng->decode(ds);
      mapApp[slist[i]] = amng;
   }
}
//================packDB interface====================================
int  SLicData::saveDBPackage()
{
   //int sz, i;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBPackFile();
   filePtr = getDBPackFileIndex();
   //qDebug() << " save app ok0=" << _dt.TID(fileDB);
   QFile file(fileDB);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -1;

   QDataStream ds(&file);
   saveDBPack(ds);
   // saveDBApp(ds);

   file.flush();
   file.close();
   //qDebug() << " save app ok=" << _dt.TID(fileDB);
   return saveDBMsg(fileDB, filePtr);
}
int  SLicData::loadDBPackage()
{

#if 0 // we donot care register here,only load the package
   if (!isDBRegisted())
   {
      i = registerDB();
      return i;
   }
#endif
   int i;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBPackFile();
   filePtr = getDBPackFileIndex();

   i = loadDBMsg(fileDB, filePtr);
   if (i <= 0)  return -1;

   fileDB = getDBFile();
   QFile file(fileDB);

   if (!file.open(QIODevice::ReadOnly)) return -1;
   QDataStream ds(&file);
   loadDBPack(ds);
// close
   file.close();
   return 1;
}
//==========clear:
/// clear db
int SLicData::clear()
{
   int sz, i;
   QStringList slist;

// pack:
   SPackMng *pmng;
   slist = mapPack.keys();

   sz = mapPack.size();
   for (i = 0; i < sz; i++)
   {
      pmng = mapPack[slist[i]];
      pmng->clear();
      delete pmng;
   }
   mapPack.clear();

// app:
   SAppMng *amng;
   slist = mapApp.keys();

   sz = mapApp.size();
   for (i = 0; i < sz; i++)
   {
      amng = mapApp[slist[i]];
      amng->setRemoveOption(true);
      amng->clear();
      delete amng;
   }
   mapApp.clear();

   return 1;

}
int SLicData::registerDB()
{
   QSettings st(ORG_NAME, SAPP_NAME);
   st.setValue(DB_REGISTER, "dbname");
   return 1;

}
bool SLicData::isDBRegisted()
{
   QSettings st(ORG_NAME, SAPP_NAME);
   QString  str;
   str = st.value(DB_REGISTER).toString();
   if (str == "dbname") return true;
   else return false;
}
//=======================heartBeat:
int SLicData::appHB(SAppInfo& app)
{
   int  number;
   LFileDate fd;
   QString str;
   QString vender, package, version, packid, pid, ip, user;
   // check App info
   SAppInfo *info;
   SAppMng *mng;
   mng = NULL;

   info = &app;

   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   version = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();

   packid = encodePackageId(vender, package, version);
   mng = appMng(packid);
   QString appid;

   appid = encodeAppId(ip, pid);

   info = NULL;
   info = (SAppInfo *)mng->get(appid);

   if (info != NULL)  return info->appHB();
   else return -1;

}
//---------checkPackExp----------------
int SLicData::checkPackExp(QString packid)
{
   int i, sz, ir, ic;
   SPackMng *mng;
   QString str;
   SPackInfo *info;
   QStringList slist;
   mng =  mapPack[packid];
   sz = mng->size();
   ic = 0;

   for (i = 0; i < sz; i++)
   {
      info = (SPackInfo *)mng->get(i);
      if (info->getStat() == STAT_EXP)
      {
         str = "packid = " + packid + " is expired!!!!";
         plog(str);
         ir = removePackage(packid, info);
         if (ir >= 0)
         {
            ic++;
         }
      }
   }
   return ic; // index of info in mng <0 err
}
int SLicData::checkPackExp()
{
   int i, sz;
   QStringList packs;
   QString pack;
   int ic, ir;
   ic = 0;

   packs = packNames();
   sz = packs.size();
   for (i = 0; i < sz; i++)
   {
      pack = packs[i];
      ir = checkPackExp(pack);
      ic = ic + ir;
   }
   return ic;
}
//---------checkAppExp----------------
int SLicData::checkAppExp(QString packid, long intvs)
{
   int i, sz;
   SAppMng *mng;
   QString str;
   SAppInfo *info;
   QStringList slist;
   mng =  mapApp[packid];
   if (mng == NULL)  return 0;
   sz = mng->size();
   qDebug() << "mng SZ= " << sz;
   int ic;
   ic = 0;
   for (i = 0; i < sz; i++)
   {
      info = (SAppInfo *)mng->get(sz - i + 1);
      if (info != NULL) if (info->isExpired(intvs))
         {
            str = "Warning: app expired =" + info->packid + " " +  info->appid + " " +  info->user + " start = " +  _dt.EP2DT(info->start);
            plog(str);
            ic++;
            //qDebug() << str;
         }
   }
   return ic; // index of info in mng <0 err
}
int SLicData::checkAppExp(long intvs)
{
   int i, sz;
   QStringList packs;
   QString pack;
   int ic, ir;
   ic = 0;

   packs = packNames();
   sz = packs.size();
   //qDebug() << "packs = " << sz;
   for (i = 0; i < sz; i++)
   {
      pack = packs[i];
      //qDebug() << "pack = "<<i<<  pack;
      ir = checkAppExp(pack, intvs);
      ic = ic + ir;
   }
   return ic;
}
//---------------------------------checkHB--------------
int SLicData::checkHB(long intvs)
{
   int i, sz, ir, ic;
   QStringList packs;
   QString pack;
   ic = 0;

   packs = packNames();
   sz = packs.size();
   for (i = 0; i < sz; i++)
   {
      pack = packs[i];
      ir = checkHB(pack, intvs);
      ic = ic + ir;
   }
   return ic;
}
int SLicData::checkHB(QString pack, long intvs)
{

   // packid,appid,user,number type, start
   int i, sz, ir, ic;
   QStringList  slist;
   SAppMng *mng;
   SAppInfo *info;
   QString appid, user, ty, str, start;
   //long start;
   LFileDate fd;


   mng = appMng(pack);
   //qDebug() << " checkHB0 =" << pack << mng;

   if (mng == NULL) return 0;

   sz = mng->size();
   //qDebug() << " checkHB =" << pack << sz;
   ic = 0;

   for (i = 0; i < sz; i++)
   {
      //qDebug() << "checkHB0 =" << i << pack;
      info = (SAppInfo *)mng->get(sz - i - 1);
      //qDebug() << "checkHB =" << i << pack << info->appid;
      if (info != NULL)  if (info->checkHB(intvs))
         {
            ic++;
            appid = info->appid;
            user = info->user;
            ir = rmApp(*info);
            if (ir > 0)
            {
               qDebug() << "check hearBeat rmApp OK appid = " << pack << appid << user;
            }
            else
            {
               qDebug() << "check hearBeat rmApp Err appid = " << pack << appid << user;
            }
         }
      //qDebug() << "checkHB1 =" << i << pack;
   }

   return ic;
}
//=============================util====================
QString SLicData::hex(QString s)
{
   string ss1, ss2;
   QString str;
   LEncrypt en;
   ss1 =  s.Q2CH;
   ss2 = en.bin2Hex(ss1);
   str = ss2.c_str();
   return str;
}
QString SLicData::unHex(QString s)
{
   string ss1, ss2;
   QString str;
   LEncrypt en;
   ss1 =  s.Q2CH;
   ss2 = en.hex2Bin(ss1);
   str = ss2.c_str();
   return str;
}
//============================other====================
QString SLicData::curLogName(QString s)
{
   QString str, str1;
   str1 = _dt.curM();
   str = s + str1 + ".log";
   str = logDir() + "/" + str;
   return str;
}

int  SLicData::changeLogNames()
{
   QString str;
   str = curLogName(ELOG);
   if (str != _elog.filename())
   {
      qDebug() << "logname changed!!!!!!!---------------------";
      _elog.close();
      _elog.setFile(str);

      str = curLogName(PLOG);
      _plog.close();
      _plog.setFile(str);

      str = curLogName(ALOG);
      _alog.close();
      _alog.setFile(str);
      plog("logname changed------------------");
   }
   else return 0;
   return 1;
}

QString SLicData::rootDir()
{
   QString s;
   s = getenv(LIC_ROOT_PATH);
   if (s == "")  s = ".";
   return s;
}
QString SLicData::dbDir()
{
   QString s;
   s = rootDir() + "/" + LIC_DB_PATH;
   mkDir(s);
   return s;
}
QString SLicData::fileDir()
{
   QString s;
   s = rootDir() + "/" + LIC_FILE_PATH;
   mkDir(s);
   return s;
}
QString SLicData::logDir()
{
   QString s;
   s = rootDir() + "/" + LIC_LOG_PATH;
   mkDir(s);
   return s;
}
bool SLicData::mkDir(QString path)
{
   QDir d;
   return d.mkpath(path);
}
//
QString SLicData::dbName()
{
   QString s;
   s = dbDir() + "/" + DB_FILE;
   return s;
   
}
QString SLicData::dbPtrName()
{
   QString s;
   s = dbDir() + "/" + DB_FILE_PTR;
   return s;
}
QString SLicData::dbPackageName()
{
   QString s;
   s = dbDir() + "/" + DB_PACKFILE;
   return s;
}
QString SLicData::dbPackagePtrName()
{
   QString s;
   s = dbDir() + "/" + DB_PACKFILE_PTR;
   return s;
}

QString SLicData::getDBFile()
{
   return dbName();
}
QString SLicData::getDBFileIndex()
{
   return dbPtrName();
}

QString  SLicData::getDBPackFile()
{
   return dbPackageName();
}
QString  SLicData::getDBPackFileIndex()
{
   return dbPackagePtrName();

}
