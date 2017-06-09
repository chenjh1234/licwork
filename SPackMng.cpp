#include "SPackMng.h"
//====================PResource:
PResource::PResource()
{
   limit = 0;
   used = 0;
}
int PResource::size()
{
   return pList.size();
}
int PResource::add(SPackInfo *info)
{
   int i;
   i = -1;
   if (info == NULL) return i;
   info->start = limit;
   limit = limit + info->limit;
   //ok:
   pList.append(info);
   i = pList.size();
   return i;

}
int PResource::changed(SPackInfo *inf, int change)
{
   int i;
   int b;
   i = -1;
   if (inf == NULL) return i;
   SPackInfo *info;
   info = find(inf);
   if (info == NULL) return i; // found it:

   b = -1;


   limit = limit + change; // limit of sourece;
   for (i = 0; i < size(); i++)
   {
      info = pList[i];
      if (info == inf)  b = i;
      if (b == -1) continue;
      // change start and limit;
      if (b == i)
      {
         inf->limit + change;
         inf->stat = info->getStat();
      }
      else
      {
         info->start = info->start + change;
      }
   }
   i = 1;

   return i;

}
int PResource::remove(SPackInfo *inf)
{
   int i;
   i = -1;
   if (inf == NULL) return i;
   SPackInfo *info;
   info = find(inf);
   if (info == NULL) return i;

   if (pList.removeOne(inf))
   { // remove in Presource:
      limit = limit - inf->limit;
      for (i = 0; i < size(); i++)
      {
         info = pList[i];
         if (info->start >= limit)
         {
            info->start = info->start - limit;
         }
      }
      i = 1;
   }
   else
   {
      i = -1;
   }
   return i;

}
SPackInfo* PResource::getInfo(int used)
{
   int i, sz, start, limit;
   SPackInfo *info;
   sz = pList.size();
   for (i = 0; i < sz; i++)
   {
      info = pList[i];
      start = info->start;
      limit = info->limit;
      if (used >= start && used <= start + limit)
      {
         return info;
      }
   }
   return NULL;
}
SPackInfo* PResource::find(SPackInfo *inf)
{
   int i, sz;
   SPackInfo *info;
   sz = pList.size();
   for (i = 0; i < sz; i++)
   {
      info = pList[i];
      if (info == inf)
      {
         return inf;
      }
   }
   return NULL;
}
/// @return i>=0 OK <0 error
int  PResource::appRequest(SAppInfo& info, SAppMng *mng)
{
   if (size() <= 0)  return -1;
   if (limit - used <= 0) return -1;

//qDebug()<< "in presource0";
   int i, sz, left, number;
   SPackInfo *pinfo;
   QString packid, appid, ty, ip, user;
//qDebug()<< "in presource0";
   i = -1;
   number = info.number;
   left = limit - used;
   if (number > left)  return -1;
// ok;
   sz = pList.size();
   pinfo = pList[0]; // we can return app the wrong BORROW;
                     //ty = pinfo->get(PTYPE).toString();
                     //info.set(PTYPE, ty); // assign type;
                     //info.rtype = ty;
                     // qDebug()<< "in presource0";

   ty = type;
   info.rtype = ty;
// return message:
   info.set(APP_VENDERSIGN, pinfo->get(VENDERSIGN));
   info.set(APP_PBORROW, pinfo->get(PBORROW));
   info.set(PTYPE, pinfo->get(PTYPE));

   qDebug() <<  "appRequest ty = " << ty;
   // qDebug() <<  "pinfo== " << pinfo->getText();
   i = number;
   if (mng == NULL)
   {
      used = used + number;
   }
   else
   {
      // task:
      if (ty == PTYPE_TASK)
      {
         used = used + number;
      }
      //node:
      else if (ty == PTYPE_NODE)
      {
//qDebug() <<  "pinfo== " ;
         ip = info.get(APP_IP).toString();
         //qDebug() << "Preaource :node mng.find=" << i << PTYPE_NODE << ip;
         i = mng->find(PTYPE, PTYPE_NODE, APP_IP, ip); // user
                                                       //qDebug() << "Preaource :node mng.find=" << i << PTYPE_NODE << ip;
                                                       //qDebug() << mng->size()<< mng->getText();
         if (i < 0)
         {
            used = used + number;
         }
         else i = 0;
         //rnode.remove(info);
      }
      // user +ip
      else if (ty == PTYPE_USER)
      {
         ip = info.get(APP_IP).toString();
         user = info.get(APP_USER).toString();
         i = mng->find(PTYPE, PTYPE_USER, APP_IP, ip, APP_USER, user); // ip + user
         if (i < 0)
         {
            used = used + number;
         }
         else i = 0;
      }
   }
//qDebug() << "11111";
   return number;
}
///  @return i>=0 OK <0 error
int  PResource::appRelease(SAppInfo& info, SAppMng *mng)
{
   //if (size() <= 0)  return -1;  // mybe package removed all
   // if (limit - used <= 0) return -1; // we not care

   int i,  number;
   //SPackInfo *pinfo;
   QString packid, appid, ty, ip, user, err;

   i = -1;
   number = info.number; // release number

   //  sz = pList.size();// packages
   ty = type;

//   if(!(ty == info.rtype)) return -1; // not the resource type;

// right resource now:

   qDebug() <<  "appRelease ty = " << ty;
#if 0
   // remove info from the SappMng:
   i = ((LInfoMng *)(mng))->rmInfo(&info); //donot remove it now;;
   if (i <=0)
   {
      err = QString("Error: Resource::appRelease ") +  "((LInfoMng *)(mng))->rmInfo(&info)";
      info.err = err;
   }
   else

   {
#endif
   // task:
   if (ty == PTYPE_TASK)
   {
      used = used - number;
   }
   //node:
   else if (ty == PTYPE_NODE)
   {

      ip = info.get(APP_IP).toString();
      i = mng->find(PTYPE, PTYPE_NODE, APP_IP, ip); // user
      if (i < 0) // no package runing in that node;
      {
         used = used - number;
      }
      else i = 0;
      //rnode.remove(info);
   }
   // user +ip
   else if (ty == PTYPE_USER)
   {
      ip = info.get(APP_IP).toString();
      user = info.get(APP_USER).toString();
      i = mng->find(PTYPE, PTYPE_USER, APP_IP, ip, APP_USER, user); // ip + user
      if (i < 0) // for user no package runing in my node;
      {
         used = used - number;
      }
      else i = 0;
   }
//   }

   return number;
}
int  PResource::encode(QDataStream& ds)
{
   //SPackInfo *info;
   int  sz;
   sz = size();
   ds << sz;
   ds << limit;
   ds << used;
   ds << type;
   return 1;
}
int  PResource::decode(QDataStream& ds)
{
   //SPackInfo *info;
   int  sz;
   ds >> sz;
   ds >> limit;
   ds >> used;
   ds >> type;
   return 1;
}
//====================================================================================


SPackMng::SPackMng() : LInfoMng()
{
   init();
}
SPackMng::~SPackMng()
{

}

void SPackMng::init()
{
   rtask.type = PTYPE_TASK;
   ruser.type = PTYPE_USER;
   rnode.type = PTYPE_NODE;
}
PResource* SPackMng::getPResource(QString ty)
{
   if (ty == PTYPE_TASK) return &rtask;
   if (ty == PTYPE_USER) return &ruser;
   if (ty == PTYPE_NODE) return &rnode;
   return NULL;
}
// -----------------rsize()-------------------------
int SPackMng::taskSize()
{
   return rtask.size();
}

int SPackMng::userSize()
{
   return ruser.size();

}
int SPackMng::nodeSize()
{
   return rnode.size();
}
// limit----------
int SPackMng::taskLimit()
{
   return rtask.limit;
}

int SPackMng::userLimit()
{
   return ruser.limit;

}
int SPackMng::nodeLimit()
{
   return rnode.limit;
}
// used----------
int SPackMng::taskUsed()
{
   return rtask.used;
}

int SPackMng::userUsed()
{
   return ruser.used;

}
int SPackMng::nodeUsed()
{
   return rnode.used;
}

//-----------remove Info-----------------------------------
int SPackMng::removeInfo(SPackInfo *info)
{
   /////////
   int  ret;
   QString ty, str;
   //SPackInfo *inf;
   _lock.lock();

   ty = info->get(PTYPE).toString();
   if (ty == PTYPE_TASK)
   {
      ret = rtask.remove(info);
   }
   else if (ty == PTYPE_NODE)
   {
      ret = rnode.remove(info);
   }
   else if (ty == PTYPE_USER)
   {
      ret = ruser.remove(info);
   }
   //
   if (ret < 0)
   {
      str = "SPackMng::removeInfo: Remove info  From Presource Error ty =" + ty;
      info->err = str;
   }
   else
   {
      ret = rmInfo(info); // in main mng,
      if (ret < 0)
      {
         str = "SPackMng::removeInfo: Remove info Error";
         info->err = str;
      }
   }

   _lock.unlock();
   return ret;
}
int SPackMng::unloadInfo(SPackInfo *info)
{
   /////////
   int  ret, limit;
   QString ty, str;
   //SPackInfo *inf;
   _lock.lock();

   limit = info->limit;
   if (limit > 0)
   {
      //start:
      limit = limit * -1;
      //qDebug() << "limit = " << limit;

      ty = info->get(PTYPE).toString();
      if (ty == PTYPE_TASK)
      {
         ret = rtask.changed(info, limit);
      }
      else if (ty == PTYPE_NODE)
      {
         ret = rnode.changed(info, limit);
      }
      else if (ty == PTYPE_USER)
      {
         ret = ruser.changed(info, limit);
      }
      //
      if (ret < 0)
      {
         str = "SPackMng::unloadInfo: Remove info  From Presource Error ty =" + ty;
         info->err = str;
      }
      else
      {
         ret =  info->setUnload(); // in main mng,
         if (ret < 0)
         {
            str = "SPackMng::unloadInfo: Remove info Error";
            info->err = str;
         }
      }

   }
   else
   {
      str = "SPackMng::unloadInfo: Can not unload a borrow out license Error";
      info->err = str;
      ret = -1;
   }
  // qDebug() << " SPackMng::unloadInfo(SPackInfo *info) ret = " << ret ;

   _lock.unlock();
   return ret;
}
int SPackMng::clear()
{
   int i, sz;
   SPackInfo *info;
   sz = size();
   for (i = 0; i < sz; i++)
   {
      info = (SPackInfo *)get(i);
      if (info != NULL)
      {
         setRemoveOption(true);
         if (removeInfo(info) < 0) return -1;
      }
      else
      {
         return -1;
      }
   }
   return sz;
}
/// add a package
int SPackMng::addInfo(SPackInfo *info)
{
   int i, ir;
   QString ty, str;
   _lock.lock();

   i = add(info);
   //qDebug() << "SPackMng::addInfo i=" << i;
   ir = 0;
   if (i  > 0)
   {
      ty = info->get(PTYPE).toString();
      //qDebug() << "ty=" << ty;
      if (ty == PTYPE_TASK)
      {
         ir = rtask.add(info);
      }
      else if (ty == PTYPE_NODE)
      {
         ir = rnode.add(info);
      }
      else if (ty == PTYPE_USER)
      {
         ir = ruser.add(info);
      }
      else
      {
         ir = rtask.add(info);
      }
      if (ir < 0)
      {
         str = "SPackMng::addInfo: add info  to Presource Error ty =:" + ty;
         info->err = str;
      }
   }
   else
   {
      str = "SPackMng::addInfo error";
      info->err = str;
   }
   _lock.unlock();
   return ir;
}
int SPackMng::encode(QDataStream& ds)
{
   int i;
   SPackInfo *info;
// main
   ds << size();
   for (i = 0; i < size(); i++)
   {
      info = (SPackInfo *)get(i);
      if (info != NULL)
      {
         info->encode(ds);
      }
      else
      {
         return -1;
      }
   }
// resources:
   rnode.encode(ds);
   ruser.encode(ds);
   rtask.encode(ds);
   return size();

}
int SPackMng::decode(QDataStream& ds)
{
   int i, sz;
   SPackInfo *info;

   ds >> sz;
   //qDebug() << __FUNCTION__ << sz;
   for (i = 0; i < sz; i++)
   {
      info = new SPackInfo();
      if (info != NULL)
      {
         info->decode(ds);
         if (info == NULL) return -1;
         if (addInfo(info) <= 0) return -1;
      }
      else return -1;
   }
   // resources:
   rnode.decode(ds);
   ruser.decode(ds);
   rtask.decode(ds);
   return sz;
}
//--------------------------app request:----------------------------------------------------------------------------------
/// @return i>=0 ok ;
int SPackMng::appRequest(SAppInfo& info, SAppMng *mng)
{
   //qDebug() << __FUNCTION__;

   _lock.lock();
   QString str; //, packid, appid;
   int i; //, number;
          //qDebug() << " over lock";
   if (mng != NULL)
   {
      qDebug() << "id = " << info.appid;
      if (mng->find(info.appid) >= 0)
      {
         //qDebug() << "mng find the appid = " << info.appid;
         info.err = " the app already registered appid = " + info.appid;
         _lock.unlock();
         return -1;
      }
      qDebug() << "no this id ";
   }
   //packid = info.packid;
   //number = info.number;
   //appid = info.appid;
   i = -1;
   //qDebug() << "mng apprequest0";
   i = rnode.appRequest(info, mng); // >=0 is ok; == 0 means for node&user,nothing changed resource number for this app
                                    //qDebug() << "mng apprequest1";
   if (i < 0) i = ruser.appRequest(info, mng);
   if (i < 0) i = rtask.appRequest(info, mng);

   if (i < 0)
   {
      str = "SPackMng::appRequest:  no lisense space error";
      info.err = str;
   }
   _lock.unlock();
   return i;

}
/// @return i>=0 ok ;
int SPackMng::appRelease(SAppInfo& info, SAppMng *mng)
{

   _lock.lock();
   QString str; //, packid, appid;
   int i; //, number;
          //packid = info.packid;
          //number = info.number;
          //appid = info.appid;
   i = -1;
   if (info.rtype == PTYPE_NODE)
   {
      i = rnode.appRelease(info, mng);
   }
   else if (info.rtype == PTYPE_USER)
   {
      i = ruser.appRelease(info, mng);
   }
   else if (info.rtype == PTYPE_TASK)
   {
      i = rtask.appRelease(info, mng);
   }

   if (i < 0)
   {
      str = "SPackMng::appRelease:  failed error";
      info.err += str;
   }
   _lock.unlock();
   return i;

}
