#include "LEncrypt.h"
#include <QString>
#define CREATEKEY_HELP  "createkey filename\n"\
"function:  create publicKey and privateKey : filename.pub ,filename.pri \n"
#define CREATEKEY_ERR "Error:  createkey error \n"
#define CREATEKEY_NOFILE "Error:  need a filename for output keys \n"
 
//========================================start main=======================

void exitCREATEKEY(int id)
{
    char help[] = CREATEKEY_HELP;
    int r;
    r = id;
    cout << help;
    #ifndef TEST_UNIT
    exit(r);
    #else
    return;
    #endif

}
#ifndef TEST_UNIT
main(int argc, char **argv)
{
#else
U_START(testCREATEKEY)
#endif
   LEncrypt lic;
   QString str,pri,pub;
   int i;
   str = "key";
   #ifdef TEST_UNIT
   #else
   if (argc <2) 
   {
       cout << CREATEKEY_NOFILE;
       exitCREATEKEY(1);    
   }
   str = argv[1];
   #endif
   
   pub = str+".pub";
   pri = str+".pri";

   i = lic.rsaCreateKeyFiles(pri.Q2CH,pub.Q2CH); 
   
#ifdef TEST_UNIT
   REM("sstr = lic.uuid()");
   GT(i, 0);
#endif

   if (i <= 0)
   {
      cout << CREATEKEY_ERR;
      exitCREATEKEY(1);
   }
   cout << "create key ok: --->"<< pri.Q2CH << "  ,  " << pub.Q2CH << endl;
   
#ifndef TEST_UNIT
}
#else
   U_END
#endif


