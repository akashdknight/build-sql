#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case PRINT: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case LOADMATRIX: return semanticParseLOADMATRIX();
        case PRINTMATRIX: return semanticParsePRINTMATRIX();
        case TRANSPOSE: return semanticParseTRANSPOSE();
        case EXPORTMATRIX: return semanticParseEXPORTMATRIX();
        case RENAMEMATRIX: return semanticParseRENAMEMATRIX();
        case CHECKSYMMETRY: return semanticParseCHECKSYMMETRY();
        case COMPUTE: return semanticParseCOMPUTE();
        case ORDERBY: return semanticParseORDERBY();
        case GROUPBY: return semanticParseGROUPBY();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}