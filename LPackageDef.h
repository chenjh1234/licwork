#ifndef PACKAGE_DEF_H
#define PACKAGE_DEF_H

#define PACKAGENAME "packagename"
#define PVERSION "version"
#define PTYPE "type"
#define PLIMIT "limit"
#define PBORROW "pborrow"// if not NULL is the server pubkey
#define PSTARTDATE "startdate"
#define PENDDATE "enddate"
#define PKEY "key"
#define BMID "bmid"
    // (char *)"borrow", // remove it 
#define PACKAGE_KEYS_MIN {(char*)PACKAGENAME,(char*)PVERSION,(char*)PTYPE,(char*)PLIMIT,\
    (char *)PSTARTDATE,(char *)PENDDATE,(char *)PKEY,NULL} 
#define PACKAGE_KEYS_OUT {(char*)PACKAGENAME,(char*)PVERSION,(char*)PTYPE,(char*)PBORROW,(char*)PLIMIT,\
    (char *)PSTARTDATE,(char *)PENDDATE,(char *)PKEY,(char *)BMID,NULL} 
    // borrow: yes:
#define PACKAGE_KEYS_ALL {(char*)PACKAGENAME,(char*)PVERSION,(char*)PTYPE,(char*)PBORROW,(char*)PLIMIT,\
    (char *)PSTARTDATE,(char *)PENDDATE,(char *)PKEY,(char *)BMID,NULL} 

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
