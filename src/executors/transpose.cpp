#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX name
 */

bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.transposeMatrixName = tokenizedQuery[2];
    return true;
}
//printRelationname can be modified to transposeRelationName by adding a variable in syntacticParser.h
bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");
    if (!matrixCatalogue.isMatrix(parsedQuery.transposeMatrixName ))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.transposeMatrixName );
    matrix->transpose();

    cout << "Number of blocks read: " << matrix->blockCount << endl;     
    cout << "Number of blocks written: "<< matrix->blockCount << endl;      //in print we are only reading from the temp files not writing anything to them.
    cout << "Number of blocks accessed: " << matrix->blockCount << endl;    // we are writing to the same blocks from which we read 
    // so no need to do read + write in this case.
    return;
}
