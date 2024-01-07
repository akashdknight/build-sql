#ifndef __EXECUTOR__
#define __EXECUTOR__

#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();

void executeLOADMATRIX();
void executePRINTMATRIX();
void executeTRANSPOSE();
void executeEXPORTMATRIX();
void executeRENAMEMATRIX();
void executeCHECKSYMMETRY();
void executeCOMPUTE();

void executeORDERBY();
void executeGROUPBY();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);

#endif