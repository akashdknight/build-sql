#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX name
 */

bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.transposeMatrixName = tokenizedQuery[1];
    return true;
}

//Tired of adding new names in parsedQuery for every operation.
bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.transposeMatrixName ))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.transposeMatrixName );
    if(matrix->checkSymmetry())cout << "TRUE" << endl;
    else cout << "FALSE" <<  endl;

    cout << "Number of blocks read: " << matrix->blockCount << endl;     
    cout << "Number of blocks written: "<< 0 << endl;      //no need to write anything to any blocks
    cout << "Number of blocks accessed: " << matrix->blockCount << endl;
    // so no need to do read + write in this case.
    return;
}
