

TEMPLATE = app
INCLUDEPATH += . \
	../crypt 

DESTDIR =Debug
CONFIG += debug

LIBS += -lssl

# Input
HEADERS +=  \
	../crypt/testG.h \
	../crypt/LEncrypt.h \ 
	../crypt/LSummary.h 

SOURCES +=  createkey.cpp\
	../crypt/LEncrypt.cpp 

