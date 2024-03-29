#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();
    cout << endl;
    cout << "Number of blocks read: " << matrix->blockCount << endl;     
    cout << "Number of blocks written: 0" << endl;      //in print we are only reading from the temp files not writing anything to them.
    cout << "Number of blocks accessed: " << matrix->blockCount << endl;
    return;
}
