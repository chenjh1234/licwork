#include "LEncrypt.h"
#include <QString>
#define UUID_HELP  "uuid\n"\
"function:  get uuid = a different string each time \n"
#define UUID_ERR "Error:  uuid error \n"
 
//========================================start main=======================

void exitUUID(int id)
{
    char help[] = UUID_HELP;
    cout << help;
    #ifndef TEST_UNIT
    exit(id);
    #else
    return;
    #endif

}
#ifndef TEST_UNIT
main(int argc, char **argv)
{
#else
U_START(testUUID)
#endif
   LEncrypt lic;
   QString str;
   string sstr;

   sstr = lic.uuid();
   str = sstr.c_str();
  
#ifdef TEST_UNIT
   REM("sstr = lic.uuid()");
   GT(str.length(), 0);
#endif

   if (str.length() <= 0)
   {
      cout << UUID_ERR;
      exitUUID(1);
   }
   cout << sstr<< endl;
   
#ifndef TEST_UNIT
}
#else
   U_END
#endif

