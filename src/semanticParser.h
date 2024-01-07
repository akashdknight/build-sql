#ifndef __SEMPARSER__
#define __SEMPARSER__

#include"syntacticParser.h"

bool semanticParse();

bool semanticParseCLEAR();
bool semanticParseCROSS();
bool semanticParseDISTINCT();
bool semanticParseEXPORT();
bool semanticParseINDEX();
bool semanticParseJOIN();
bool semanticParseLIST();
bool semanticParseLOAD();
bool semanticParsePRINT();
bool semanticParsePROJECTION();
bool semanticParseRENAME();
bool semanticParseSELECTION();
bool semanticParseSORT();
bool semanticParseSOURCE();

bool semanticParseLOADMATRIX();
bool semanticParsePRINTMATRIX();
bool semanticParseTRANSPOSE();
bool semanticParseEXPORTMATRIX();
bool semanticParseRENAMEMATRIX();
bool semanticParseCHECKSYMMETRY();
bool semanticParseCOMPUTE();

bool semanticParseORDERBY();
bool semanticParseGROUPBY();

#endif