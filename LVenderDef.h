#ifndef VENDER_DEF_H
#define VENDER_DEF_H

#define VENDERNAME "vendername"
#define CLIENTNAME "clientname"
#define SERVERNAME "servername"
#define SERVERID "serverid"

#define SERVERPRI "serverpri"
#define SERVERPUB "serverpub"
#define VENDERPRI "venderpri"
#define VENDERPUB "venderpub"
#define VENDERSEED "venderseed"
//#define VENDERBORROW "venderborrow"

#define UUID "uuid"
#define BORROW "borrow"
#define VENDERKEY "venderkey"

#define VENDER_KEYS_MIN {(char*)VENDERNAME,(char*)SERVERID,\
    (char *)UUID,\
    (char *)VENDERKEY,NULL}
    
#define VENDER_KEYS_OUT {(char*)VENDERNAME,(char*)CLIENTNAME,(char*)SERVERNAME,(char*)SERVERID,\
    (char *)UUID,\
    (char *)BORROW,(char *)VENDERKEY,NULL}  
    // borrow:yes|no
#define VENDER_KEYS_ALL {(char*)VENDERNAME,(char*)CLIENTNAME,(char*)SERVERNAME,(char*)SERVERID,\
    (char *)UUID,\
    (char *)SERVERPUB, (char *)SERVERPRI, (char *)VENDERPUB, (char *)VENDERPRI, (char *)VENDERSEED,\
    (char *)BORROW,(char *)VENDERKEY,NULL}  

#endif
#if 0
# licfile examples:
#  vender info: venderName mush be the firstline



venderName = geoeast
clientName = bgp 
serverName =   12345
serverID =   12345
uuid = 699a6047-90e7-4f1d-b3d0-f377587362d6
borrow = on  #on|off
venderKey = 123

#  package info: packageName mush be the firstline version the 2nd

packageName = pc1
version  = 1.0
# type: task|user|node
type = task
limit = 10
startDate = 20170101
endDate = 20180101
key = 123
#endif
