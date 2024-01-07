#include "global.h"
/**
 * @brief 
 * SYNTAX: COMPUTE matrix-name
 */

bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");
    if (!matrixCatalogue.isMatrix(parsedQuery.computeMatrixName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.computeMatrixName );
    matrix->compute();

    cout << "Number of blocks read: " << matrix->blockCount << endl;     
    cout << "Number of blocks written: "<< matrix->blockCount << endl;    
    cout << "Number of blocks accessed: " << 2*matrix->blockCount << endl;    
    // so no need to do read + write in this case.
    return;
}
