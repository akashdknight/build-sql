#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX name
 */

bool syntacticParseRENAMEMATRIX()
{
    logger.log("syntacticParseRENAMEMATRIX");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAMEMATRIX;
    parsedQuery.renameMatrixNameFrom = tokenizedQuery[2];
    parsedQuery.renameMatrixNameTo = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAMEMATRIX()
{
    logger.log("semanticParseRENAMEMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.renameMatrixNameFrom ))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeRENAMEMATRIX()
{
    logger.log("executeRENAMEMATRIX");
    matrixCatalogue.renameMatrix(parsedQuery.renameMatrixNameFrom, parsedQuery.renameMatrixNameTo);

    cout << "Number of blocks read: " << 0 << endl;             //since no blocks of data is read/written to.
    cout << "Number of blocks written: "<< 0 << endl;      
    cout << "Number of blocks accessed: " << 0 << endl; 
    
    return;
}
