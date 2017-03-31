#include "LLicEncrypt.h"
#define MY_HELP  "createlic outfilename\n"\
"inputfile: licfile.sample \n"\
"    venderPri: 1: form licfile.sample, 2:\"vender.pri\"\n"\
"    ServerPub: 1: form licfile.sample, 2:\"vender.pri\"\n"\
"    venderseed: from licfile.sample  \n"\
"    vendername: from licfile.sample  \n"\
"    serverid: mid of the server (length = 35)  \n"\
"    servername: hostname or IP  of the server  \n"\
"    uuid: create new uuid automaticly  \n"\
"outputfile : arg1\n"\
"     if omit outfile = lic_clientname+_uuid_.lic\n"

#define INPUT_ERR "Error: input file  licfile.sample Parser error \n"
#define SERVERPUB_ERR "Error: server Pub file is not exist!!!\n"
#define VENDERPRI_ERR "Error: vender Pri file is not exist!!!\n"
#define VENDERKEY_ERR "Error: get vender key error\n"
#define PACKAGEKEY_ERR "Error: get package key error\n"
#define WRITEFILE_ERR "Error: write license file  error\n"
#define MID_ERR "Error: mid length error\n"
//========================================start main=======================

void exitM(int id)
{
    char help[] = MY_HELP;
    cout << help;
    #ifndef TEST_UNIT
    exit(id);
    #else
    return;
    #endif

}
#ifndef TEST_UNIT
#define SERVER_PUB "server.pub"
#define SERVER_PRI "server.pri"
#define VENDER_PUB "vender.pub"
#define VENDER_PRI "vender.pri"

#define LIC_FILE_SAMPLE "licfile.sample"

main(int argc, char **argv)
{
#else
U_START(testCreateLic)
#endif
   
   int i;
   LLicEncrypt lic;
   LLicFile *lfile;
   QString str, fileo;
   LInfo * infoV,*infoP;
   LEncrypt en;
// open file:
   QString sPub, vPri, vSeed, venderKey, pKey;
   //i = lic.openLicFile(LIC_FILE_SAMPLE);//licfile.sample
   //i = lic.openLicFile(LIC_FILE); //licfile.sample
   fileo = "";
#ifndef TEST_UNIT
   if (argc >= 2) fileo = argv[1];
#endif

   lfile = new LLicFile();
   i = lfile->readFile((char *)LIC_FILE_SAMPLE);
   //qDebug() << "lfile.->readFile return = " << i;

#ifdef TEST_UNIT
   GT(i, 0);
#endif
   if (i <= 0)
   {
      cout << INPUT_ERR;

      exitM(1);
   }
   //lfile = lic.getLicFile();

// venderPri serverPub files:

   infoV = lfile->vender();
   // serverPub file:
   if (infoV->isKey(SERVERPUB))  sPub = infoV->get(SERVERPUB).toString();
   else sPub = SERVER_PUB;
   // venderPri file:
   if (infoV->isKey(VENDERPRI))  vPri = infoV->get(VENDERPRI).toString();
   else vPri = VENDER_PRI;
   // venderSeed:
   if (infoV->isKey(VENDERSEED))  vSeed = infoV->get(VENDERSEED).toString();
   else vSeed = VENDER_SEED;
// is serverPub,venserPri file exist:
   LFileDate fd;
   if (!fd.isFile(sPub))
   {
      cout << SERVERPUB_ERR << "file = " << sPub.Q2CH << endl;
      exitM(1);
   }
   if (!fd.isFile(vPri))
   {
      cout << VENDERPRI_ERR << "file = " << vPri.Q2CH << endl;
      exitM(1);
   }
// set new uuid:
   string sstr;
   sstr = en.uuid();
   str = sstr.c_str();
   infoV->set(UUID, str);
// serverID check:
   QString sid;
   sid = infoV->get(SERVERID).toString();
   if (sid.length() != MID_LEN) 
   {
      cout << MID_ERR << "  mid= " << sid.Q2CH << endl;
      exitM(1);
   }

//get venderKey
   str = lic.encryptVenderKey(sPub,  vPri, infoV, vSeed);
   infoV->set(VENDERKEY, str);

   //qDebug() << "venderKey = " << VENDERKEY<<str;
#if 0
   LPackageMng pm;
   pm = lfile->packageMng();
   qDebug() << "pm = " << pm.size() << pm.get(0)->size();
   infoP = lfile->package(0);
   qDebug() << "infoP = " << infoP << infoP->size();
   //exit(1);
#endif

#ifdef TEST_UNIT
   REM("str = lic.encryptVenderKey( sPub,  vPri, infoV,vSeed);");
   GT(str.length(), 0);
#endif

   if (str.length() <= 0)
   {
      cout << VENDERKEY_ERR;
      exitM(1);
   }
   venderKey = str;
// get package keys:
   int num;
   num  = lfile->size();
   //qDebug() << "packages = "<< num;

   for (i = 0; i < num; i++)
   {
      infoP = lfile->package(i);
      str = lic.digestPackageKey(venderKey, infoP);
      //qDebug() << "pkey1 = " << str.length() << str << PKEY;
      infoP->set(PKEY, str);
      //qDebug() << "pkey2 = " << PKEY;
#ifdef TEST_UNIT
      //qDebug() << "pkey = " << str;
      REM("str = lic.digestPackageKey(venderKey,infoP); ")
      GT(str.length(), 0);
#endif
   }
   QString client, uuid;
   client = infoV->get(CLIENTNAME).toString();
   uuid = infoV->get(UUID).toString();

// output files:
   if (fileo.length() == 0)  fileo = "lic_" + client + "_" + uuid + "_.lic";
   i = lfile->writeFile((char *)fileo.Q2CH);
#ifdef TEST_UNIT
   REM("i = lfile->writeFile(LIC_FILE1); ")
   GT(i, 0);
#endif
   if (i <= 0)
   {
      cout << WRITEFILE_ERR;
      exitM(1);
   }
   delete lfile;
   str = "create Lic file OK->" + fileo;
   cout << str.Q2CH << endl;

#ifndef TEST_UNIT
}
#else
   U_END
#endif
