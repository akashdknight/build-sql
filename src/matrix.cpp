#include "global.h"

Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}


//no need of the second arguments, cos unlike tables we wont have names of columns in matrix
// Matrix::Matrix(string matrixName, vector<string> columns)
// {
//     logger.log("Matrix::Matrix");
//     this->sourceFileName = "../data/temp/" + matrixName + ".csv";
//     this->matrixName = matrixName;
//     this->columns = columns;
//     this->columnCount = columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
//     this->writeRow<string>(columns);
// }

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */

bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))             // read first row of the file
    {   
        fin.close();
        if (this->extractColumnDetails(line))         
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

// Gets the size of each row or basically counts the number of columns present.
bool Matrix::extractColumnDetails(string firstLine)
{
    logger.log("Matrix::extractColumnDetails");
 
    string word;
    stringstream s(firstLine);
    int col_count = 0;
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        // if (columnNames.count(word))                // column names should be unique, else return false.
        //     return false;
        // columnNames.insert(word);
        // this->columns.emplace_back(word);
        col_count++;
    }
    this->columnCount = col_count;
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1024) / (sizeof(int))); //block size in kb/(size of each row)
    this->maxRowsPerBlock = (uint)sqrt(this->maxRowsPerBlock);          //we will storing small square matrices in each block.
 
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;

    // the below lines are required when implementing the distinct values in column thing.
    // unordered_set<int> dummy;
    // dummy.clear();
    // this->distinctValuesInColumns.assign(this->columnCount, dummy);
    // this->distinctValuesPerColumnCount.assign(this->columnCount, 0);

    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;   //counting number of rows.
        this->rowCount++;   //this should be removed if updateStatistics() function is being called.
        // this->updateStatistics(row);  // this is required for distinct values in column thing
        if (pageCounter == this->maxRowsPerBlock)
        {   
            int index = 0, box_size = this->maxRowsPerBlock;
            vector<vector<int>> box_data(box_size, vector<int>(box_size, 0));
            while(index < this->columnCount)            
            {                
                int loop_till_col = (index + box_size) < this->columnCount ? (index + box_size) : this->columnCount;
                for(int r = 0; r < this->maxRowsPerBlock; r++)
                {  
                    for(int c = index; c < loop_till_col; c++)
                    {
                        box_data[r][c-index] = rowsInPage[r][c];
                    }
                }
                if(index + box_size >= this->columnCount) //to handle the last block exact size is required for the array.
                {
                    for(int r = 0; r < box_size; r++)
                    {
                        box_data[r].resize(loop_till_col - index);
                    }
                }
                index = index + box_size;
                bufferManager.writePage(this->matrixName, this->blockCount, box_data, pageCounter);
                this->blockCount++;
                this->rowsPerBlockCount.emplace_back(pageCounter);
                this->colsPerBlockCount.emplace_back(box_data[0].size());
            }
            // bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
            // // blockCount is the pageIndex here for writePage
            // //writePage basically creates a temp file consisting of the data present in vvi rowsInPage.
            // //Presently this is being called whenever pageCounter == this->maxRowsPerBlock i.e. each page is of size equal to block size.
            // // If we want multiple pages in the same block the if condition for this block needs to be modified accordingly.
            // //same bufferManager can be used for both matrix and table
            // this->blockCount++;
            // this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {   
        int index = 0, box_size = this->maxRowsPerBlock;
        vector<vector<int>> box_data(pageCounter, vector<int>(box_size, 0));
        while(index < this->columnCount)            
        {                
            int loop_till_col = (index + box_size) < this->columnCount ? (index + box_size) : this->columnCount;
            for(int r = 0; r < pageCounter; r++)
            {  
                for(int c = index; c < loop_till_col; c++)
                {
                    box_data[r][c-index] = rowsInPage[r][c];
                }
            }
            
            if(index + box_size >= this->columnCount) //to handle the last block exact size is required for the array.
            {
                for(int r = 0; r < pageCounter; r++)
                {
                    box_data[r].resize(loop_till_col - index);
                }
            }
            index = index + box_size;
            bufferManager.writePage(this->matrixName, this->blockCount, box_data, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            this->colsPerBlockCount.emplace_back(box_data[0].size());
        }
        // bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
        // this->blockCount++;
        // this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0) //being updated in updateStatistics() function in og code.
        return false;
    // this->distinctValuesInColumns.clear(); //this is required if distinctColumns thing is required.

    return true;
}

// /**
//  * @brief Given a row of values, this function will update the statistics it
//  * stores i.e. it updates the number of rows that are present in the column and
//  * the number of distinct values present in each column. These statistics are to
//  * be used during optimisation.
//  *
//  * @param row 
//  */
// void Table::updateStatistics(vector<int> row)
// {
//     this->rowCount++;   //this can be done above only without calling this func.
//     //distinct values are not required as of now.
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
//         {
//             this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
//             this->distinctValuesPerColumnCount[columnCounter]++;
//         }
//     }
// }

// /**
//  * @brief Checks if the given column is present in this table.
//  *
//  * @param columnName 
//  * @return true 
//  * @return false 
//  */
// bool Table::isColumn(string columnName)
// {
//     logger.log("Table::isColumn");
//     for (auto col : this->columns)
//     {
//         if (col == columnName)
//         {
//             return true;
//         }
//     }
//     return false;
// }


/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    // //print headings
    // this->writeRow(this->columns, cout);

    uint rows_to_print = count , cols_to_print = count;
    int den = this->maxRowsPerBlock;
    int num = this->rowCount;
    int n = (num-1)/den + 1;                    //bigger matrix is stored in small blocks of square shape.

    vector<int> row;
    int c = 0;
    for(int r = 0; r < rows_to_print; r++)
    {
        while(c < this->columnCount)
        {   
            int pageIndex = (r/den)*n + (c/den);

            Cursor cursor(this->matrixName, pageIndex, true);
            
            for(int i = (pageIndex/n)*this->maxRowsPerBlock; i <= r; i++)
            {
                row = cursor.getNext(true);
            }
            if(c + this->maxRowsPerBlock < this->columnCount)this->writeRow(row, cout, false);  //false mean dont write "\n"
            else this->writeRow(row, cout);

            c = c + this->maxRowsPerBlock;
        
        }
        c = 0;
    }
    printRowCount(this->rowCount);
    // Cursor cursor(this->matrixName, 0, true);  //0 is the page index from which cursor should be started.
    // vector<int> row;
    // for (int rowCounter = 0; rowCounter < count; rowCounter++)
    // {
    //     row = cursor.getNext();             //it gives next row.
    //     this->writeRow(row, cout);
    // }
    // printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex+1, true);
    }
}



/**
 * @brief called when EXPORT MATRIX command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    //uncomment below two line if you want to delete the source file and check recursively if it works for matrix.
    // if(!this->isPermanent())
    //     bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::trunc);  //ios::trunc is being used bcos if fileName already exists its content shd be removed.

    Cursor cursor(this->matrixName, 0, true);   //true for matrix.

    int rows_to_print = this->rowCount , cols_to_print = this->columnCount;
    int den = this->maxRowsPerBlock;
    int num = this->rowCount;
    int n = (num-1)/den + 1;                    //bigger matrix is stored in small blocks of square shape.
    // n is the number of such blocks per row.
    
    vector<int> row;
    int c = 0;
    for(int r = 0; r < rows_to_print; r++)
    {
        while(c < this->columnCount)
        {   
            int pageIndex = (r/den)*n + (c/den);

            Cursor cursor(this->matrixName, pageIndex, true);
            
            for(int i = (pageIndex/n)*this->maxRowsPerBlock; i <= r; i++)
            {
                row = cursor.getNext(true);
            }
            if(c + this->maxRowsPerBlock < this->columnCount)this->writeRow(row, fout, false);  //false mean dont write "\n"
            else this->writeRow(row, fout);

            c = c + this->maxRowsPerBlock;
        
        }
        c = 0;
    }
    // below commented lines are the og code written for Table::makePermanent.
    // for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    // {
    //     row = cursor.getNext();
    //     this->writeRow(row, fout);
    // }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    // why are we even deleting it.? its affecting rename.
    // if (!isPermanent())
    //     bufferManager.deleteFile(this->sourceFileName);
}

// /**
//  * @brief Function that returns a cursor that reads rows from this table
//  * 
//  * @return Cursor 
//  */
// Cursor Table::getCursor()
// {
//     logger.log("Table::getCursor");
//     Cursor cursor(this->tableName, 0);
//     return cursor;
// }
// /**
//  * @brief Function that returns the index of column indicated by columnName
//  * 
//  * @param columnName 
//  * @return int 
//  */
// int Table::getColumnIndex(string columnName)
// {
//     logger.log("Table::getColumnIndex");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (this->columns[columnCounter] == columnName)
//             return columnCounter;
//     }
// }

/* 
    - helper function for transpose
    - transposes the given smaller matrix;
*/
void Matrix::switch_them(Page &page)
{
    logger.log("Matrix::switch_them");
    vector<vector<int>> temp(page.getRows()[0].size());
    vector<vector<int>> temp2 = page.getRows();
    int row_l = temp2.size() , col_l = temp2[0].size();
    for(int i = 0; i < row_l; i++)
    {
        for(int j = 0; j < col_l; j++)
        {
            temp[j].push_back(temp2[i][j]);
        }
    }
    page.writePage(temp);       //instead we can go through bufferManager.writePage();
    //but it will create another instance of page it is more heavier compared to this method.
    // if required can be copied from makePermanent method.

}


/*
    - This will transpose the given matrix and its an update command (in place)
    - No extra blocks are created.
    - Main memory of 2 blocks is used during the process as temporary storage points.
    - Logic: select two blocks which are diagonally opposite
    - take transpose of them individually then switch them.
*/

void Matrix::transpose()
{   
    logger.log("Matrix::Transpose");
    
    int outside_rows = (this->rowCount - 1)/this->maxRowsPerBlock  + 1;
    int outside_cols = (this->columnCount - 1)/this->maxRowsPerBlock + 1;
    
    for(int i = 0; i < outside_rows; i++)
    {   
        for(int j = i; j < outside_rows; j++)
        {   

            int pIndex1 = i*outside_rows + j, pIndex2 = j*outside_rows + i;
            Page block1(this->matrixName, pIndex1, true);
            Page block2(this->matrixName, pIndex2, true);
            vector<vector<int>> temp = block2.getRows();
            block2.writePage(block1.getRows());
            block1.writePage(temp);
            temp.clear();
            if(pIndex1 == pIndex2)this->switch_them(block1);
            this->switch_them(block1);
            this->switch_them(block2);
        
            if(pIndex1 != pIndex2)block1.writePage();
            block2.writePage();
        }
    }
    bufferManager.clearPool();
}

/*
    - This will check for the symmetry of the given matrix and its an update command (in place)
    - No extra blocks are created.
    - Main memory of 2 blocks is used during the process as temporary storage points.
    Logic : Take two diagonally opposite blocks
    - transpose one of them and check if its equal to the other block
    - check if each of the matrices along the diagonal are symmetrical.
*/

bool Matrix::checkSymmetry()
{
    logger.log("Matrix::checkSymmetry");
    int outside_rows = (this->rowCount - 1)/this->maxRowsPerBlock  + 1;
    int outside_cols = (this->columnCount - 1)/this->maxRowsPerBlock + 1;
    
    bool flag = true;
    for(int i = 0; i < outside_rows; i++)
    {   
        for(int j = i; j < outside_rows; j++)
        {   
            int pIndex1 = i*outside_rows + j, pIndex2 = j*outside_rows + i;
            Page block1(this->matrixName, pIndex1, true);
            Page block2(this->matrixName, pIndex2, true);
            vector<vector<int>> temp = block2.getRows();
            vector<vector<int>> temp2(temp[0].size());

            //now transpose temp
            for(int j2 = 0; j2 < temp[0].size(); j2++)
            {
                for(int i2 = 0; i2 < temp.size(); i2++)
                {
                    temp2[j2].push_back(temp[i2][j2]);
                }
            }
            temp = block1.getRows();
            
            for(int i2 = 0; i2 < temp.size(); i2++)
            {
                for(int j2 = 0; j2 < temp[0].size(); j2++)
                {
                    if(temp[i2][j2] != temp2[i2][j2])
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

/*
    - This will compute us a new matrix with the name A_RESULT.
    - A_RESULT = A - A'
    - TRANSPOSE A 
    - RENAME A AS A_RESULT
    - LOAD MATRIX A again
    - NOW COMPUTE THEIR DIFFERENCE BY LOADING EACH PAGE.
*/

void Matrix::compute()
{
    logger.log("Matrix::Compute");
    this->transpose();
    matrixCatalogue.renameMatrix(this->matrixName, this->matrixName + "_RESULT");
    Matrix *matrix = new Matrix(parsedQuery.computeMatrixName);
    matrix->load();
    matrixCatalogue.insertMatrix(matrix);
    int outside_rows = (this->rowCount - 1)/this->maxRowsPerBlock  + 1;
    int outside_cols = (this->columnCount - 1)/this->maxRowsPerBlock + 1;
    
    //this is matrixName_RESULT. (but presently has matrix' values) 
    // matrix is OG matrix.
    for(int i = 0; i < outside_rows; i++)
    {   
        for(int j = 0; j < outside_rows; j++)
        { 

            int pIndex = i*outside_rows + j;
            Page block1(this->matrixName, pIndex, true);
            Page block2(matrix->matrixName, pIndex, true);
            vector<vector<int>> transposed_values = block1.getRows();
            vector<vector<int>> og_values = block2.getRows();
            int rows_size = transposed_values.size();
            int cols_size = transposed_values[0].size();
            for(int i = 0; i < rows_size; i++)
            {
                for(int j = 0; j < cols_size; j++)transposed_values[i][j] = og_values[i][j] - transposed_values[i][j];
            }
            block1.writePage(transposed_values);            
            block1.writePage();
        }
    }
    bufferManager.clearPool();
}