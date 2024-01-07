#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOADMATRIX()
{
    logger.log("syntacticParseLOADMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOADMATRIX;
    parsedQuery.loadRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOADMATRIX()
{
    logger.log("semanticParseLOADMATRIX");
    if (matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOADMATRIX()
{
    logger.log("executeLOADMATRIX");

    Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix. Column Count: " << matrix->columnCount << " Row Count: " << matrix->rowCount << endl;
        cout << endl;
        cout << "Number of blocks read: 0" << endl;     //in load we are only reading from the disk, and writing it to blocks
        cout << "Number of blocks written: " << matrix->blockCount << endl;      
        cout << "Number of blocks accessed: " << matrix->blockCount << endl;
    }
    return;
}