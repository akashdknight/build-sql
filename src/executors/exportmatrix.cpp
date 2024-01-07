#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT MATRIX <relation_name> 
 */

bool syntacticParseEXPORTMATRIX()
{
    logger.log("syntacticParseEXPORTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORTMATRIX;    
    parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORTMATRIX()
{
    logger.log("semanticParseEXPORTMATRIX");
    //Matrix should exist
    if (matrixCatalogue.isMatrix(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such Matrix exists" << endl;
    return false;
}

void executeEXPORTMATRIX()
{
    logger.log("executeEXPORTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
    matrix->makePermanent();

    cout << "Number of blocks read: " << matrix->blockCount << endl;        // all blocks of the given matrix must be read inorder to write to a file.
    cout << "Number of blocks written: "<< 0 << endl;      // no blocks are written to.
    cout << "Number of blocks accessed: " << matrix->blockCount << endl;    

    return;
}