#include "global.h"

Cursor::Cursor(string tableName, int pageIndex, bool isMatrix)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex, isMatrix);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext(bool isMatrix)
{
    logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        
        if(!isMatrix)tableCatalogue.getTable(this->tableName)->getNextPage(this);
        else matrixCatalogue.getMatrix(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

vector<vector<int>> Cursor::getAllRows(bool isMatrix)
{
    if(!isMatrix)this->page.getRows();
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex, bool isMatrix)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex, isMatrix);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}