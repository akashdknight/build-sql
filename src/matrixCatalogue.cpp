#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::insertMatrix"); 
    this->matrices[matrix->matrixName] = matrix;
}

void MatrixCatalogue::deleteMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::deleteMatrix"); 
    this->matrices[matrixName]->unload();
    delete this->matrices[matrixName];
    this->matrices.erase(matrixName);
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *matrix = this->matrices[matrixName];
    return matrix;
}
bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrices.count(matrixName))
        return true;
    return false;
}

// bool MatrixCatalogue::isColumnFromMatrix(string columnName, string matrixName)
// {
//     logger.log("MatrixCatalogue::isColumnFromMatrix"); 
//     if (this->isMatrix(matrixName))
//     {
//         Matrix* matrix = this->getMatrix(matrixName);
//         if (matrix->isColumn(columnName))
//             return true;
//     }
//     return false;
// }

void MatrixCatalogue::print()
{
    logger.log("MatrixCatalogue::print"); 
    cout << "\nRELATIONS" << endl;

    int rowCount = 0;
    for (auto rel : this->matrices)
    {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

void MatrixCatalogue::renameMatrix(string fromName, string toName)
{
    logger.log("MatrixCatalogue::renameMatrix");
    this->matrices[fromName]->matrixName = toName;
    this->matrices[toName] = this->matrices[fromName];
    this->matrices.erase(fromName);
    int pages = (this->matrices[toName]->rowCount - 1)/this->matrices[toName]->maxRowsPerBlock + 1;
    pages = pages*pages;

    for(int pi = 0; pi < pages; pi++)
    {
        string from = "../data/temp/" + fromName + "_Page" + to_string(pi);
        string to = "../data/temp/" + toName + "_Page" + to_string(pi);
        
        rename(from.c_str(), to.c_str());
    }
}

MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto matrix: this->matrices){
        matrix.second->unload();
        delete matrix.second;
    }
}
