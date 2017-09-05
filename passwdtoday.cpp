#include "LFileDate.h"
#include <QString>
#define UUID PASSWDTODAY

#define UUID_HELP  "passwdtoday seed(or none)\n"\
"function:  get a dynamic passwd string with a seed ,tomorrow will changed \n"
#define UUID_ERR "Error:  passwdtoday error \n"
 
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
   LFileDate lic;
   QString str,seed;
   if (argc>=2)  seed = argv[1];
   else
       seed = "";
   str = lic.passwdToday(seed);
   
#ifdef TEST_UNIT
   REM("str = lic.passwdtoday()");
   GT(str.length(), 0);
#endif

   if (str.length() <= 0)
   {
      cout << UUID_ERR;
      exitUUID(1);
   }
   cout << str.Q2CH<< endl;
   
#ifndef TEST_UNIT
}
#else
   U_END
#endif

