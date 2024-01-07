#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))             // read column names
    {   
        fin.close();
        if (this->extractColumnNames(line))         //not required if no column names exist, i.e. a matrix
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))                // column names should be unique, else return false.
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount)); //block size in kb/(size of each row)
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);         // ignoring the column names.
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
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            //writePage basically creates a temp file consisting of the data present in vvi rowsInPage.
            //Presently this is being called whenever pageCounter == this->maxRowsPerBlock i.e. each page is of size equal to block size.
            // If we want multiple pages in the same block the if condition for this block needs to be modified accordingly.
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0) //being updated in updateStatistics() function
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;   //this can be done above only without calling this func.
    //distinct values are not required as of now.
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext(false);
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}



/*
 * @brief This function will perform external sorting of the given relation Name
 * 
*/
//BLOCK_ALLOWED should be atleast 2
int BLOCK_ALLOWED = 10; //Number of blocks allowed in RAM while sorting (as per phase 2 document)

void Table::createTempPages(string tempTableName1, string tempTableName2)
{
    logger.log("Table::createTempPages");

    Table * tempTable1 = new Table(tempTableName1);
    Table * tempTable2 = new Table(tempTableName2);
    tableCatalogue.insertTable(tempTable1);
    tableCatalogue.insertTable(tempTable2);

    tempTable1->columnCount = this->columnCount;
    tempTable1->columns = this->columns;
    tempTable1->rowCount = this->rowCount;
    tempTable1->blockCount = this->blockCount;
    tempTable1->maxRowsPerBlock = this->maxRowsPerBlock;
    tempTable1->rowsPerBlockCount = this->rowsPerBlockCount;

    tempTable2->columnCount = this->columnCount;
    tempTable2->columns = this->columns;
    tempTable2->rowCount = this->rowCount;
    tempTable2->blockCount = this->blockCount;
    tempTable2->maxRowsPerBlock = this->maxRowsPerBlock;
    tempTable2->rowsPerBlockCount = this->rowsPerBlockCount;


    vector<vector<int>> emptyRow(this->maxRowsPerBlock, vector<int>(this->columnCount, 0));

    for(int index = 0; index < this->blockCount; index++)
    {   
        if(index != this->blockCount - 1)
        {
            bufferManager.writePage(tempTableName1, index, emptyRow, emptyRow.size());
            bufferManager.writePage(tempTableName2, index, emptyRow, emptyRow.size());
        }
        else
        {
            int row_size = this->rowCount % this->maxRowsPerBlock;
            if(row_size == 0)row_size = this->maxRowsPerBlock;
            emptyRow.resize(row_size);
            bufferManager.writePage(tempTableName1, index, emptyRow, emptyRow.size());
            bufferManager.writePage(tempTableName2, index, emptyRow, emptyRow.size());
        }
    }
}

bool compare_it(vector<int>& one, vector<int>& two, int index)
{
    if(index >= parsedQuery.sortColumnNames.size())return true;
    string colName = parsedQuery.sortColumnNames[index];
    int col_index = tableCatalogue.getTable(parsedQuery.sortRelationName)->getColumnIndex(colName);

    if(parsedQuery.sortingStrategies[index] == ASC)
    {
        if(one[col_index] < two[col_index])return true;
        else if(one[col_index] > two[col_index])return false;
        else return compare_it(one, two, index+1);
    }
    else
    {
        if(one[col_index] > two[col_index])return true;
        else if(one[col_index] < two[col_index])return false;
        else return compare_it(one, two, index+1);
    }
}

bool compare(vector<int>& one, vector<int>& two)
{
    return compare_it(one, two, 0);
}
//this will sort set of (block_allowed-1) blocks at a time.
void Table::sortPages(string tableone, string tabletwo)
{
    logger.log("Table::sortPages");
    
    int allowed_size = this->maxRowsPerBlock*(BLOCK_ALLOWED - 1);
    vector<vector<int>> data_copy;
    int rowsTaken = 0;
    int pageIndex = 0;
    Cursor cursor(this->tableName, 0);

    while(rowsTaken < this->rowCount)
    {
        for(int i = 0; i < allowed_size; i++)
        {   
            if(rowsTaken == this->rowCount)break;

            vector<int> row = cursor.getNext(false);
            data_copy.push_back(row);
            rowsTaken++;
        }
        sort(data_copy.begin(), data_copy.end(), compare);

        int dc_index = 0;
        while(dc_index < data_copy.size())
        {   
            vector<vector<int>> pageData;
            for(int i = 0; i < this->maxRowsPerBlock; i++)
            {
                if(dc_index == data_copy.size())break;
                pageData.push_back(data_copy[dc_index]);
                dc_index++;
            }
            bufferManager.writePage(tableone, pageIndex++, pageData, pageData.size());
        }
        data_copy.clear();
    }
}

//merges two sorted blocks
void merge(string readFrom, string writeTo, int firstPageIndex, int secondPageIndex)
{   
    Table * table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    int firstSize = (secondPageIndex - firstPageIndex)*(table->maxRowsPerBlock);
    int secondSize = min(firstSize, (int)(table->rowCount - (table->maxRowsPerBlock*secondPageIndex)));

    if(firstPageIndex == secondPageIndex)
    {
        secondSize = 0;
        firstSize = table->rowCount - firstPageIndex*table->maxRowsPerBlock;
    }

    Cursor firstCursor(readFrom, firstPageIndex);                
    Cursor secondCursor(readFrom, secondPageIndex);
    vector<int> firstReadRow, secondReadRow;
    firstReadRow = firstCursor.getNext(false);
    secondReadRow = secondCursor.getNext(false);

    int firstIndex = 0, secondIndex = 0;
    int writePageIndex = firstPageIndex;
    int writeRowIndex = 0;
    vector<vector<int>> writeRow;

    while(firstIndex < firstSize && secondIndex < secondSize)
    {
        if(compare(firstReadRow, secondReadRow))
        {
            writeRow.push_back(firstReadRow);
            firstIndex++;
            writeRowIndex++;

            if(firstIndex < firstSize)firstReadRow = firstCursor.getNext(false);
        }
        else
        {
            writeRow.push_back(secondReadRow);
            secondIndex++;
            writeRowIndex++;

            if(secondIndex < secondSize)secondReadRow = secondCursor.getNext(false);
        }

        if(writeRowIndex == table->maxRowsPerBlock)
        {
            bufferManager.writePage(writeTo, writePageIndex, writeRow, writeRow.size());
            writePageIndex++;
            writeRow.clear();
            writeRowIndex = 0;
        }
    }

    while(firstIndex < firstSize)
    {
        writeRow.push_back(firstReadRow);
        firstIndex++;
        writeRowIndex++;

        if(firstIndex < firstSize)firstReadRow = firstCursor.getNext(false);

        if(writeRowIndex == table->maxRowsPerBlock)
        {
            bufferManager.writePage(writeTo, writePageIndex, writeRow, writeRow.size());
            writePageIndex++;
            writeRow.clear();
            writeRowIndex = 0;
        }        
    }

    while(secondIndex < secondSize)
    {
        writeRow.push_back(secondReadRow);
        secondIndex++;
        writeRowIndex++;

        if(secondIndex < secondSize)secondReadRow = secondCursor.getNext(false);

        if(writeRowIndex == table->maxRowsPerBlock)
        {
            bufferManager.writePage(writeTo, writePageIndex, writeRow, writeRow.size());
            writePageIndex++;
            writeRow.clear();
            writeRowIndex = 0;
        }
    }

    if(writeRow.size())
    {
        bufferManager.writePage(writeTo, writePageIndex, writeRow, writeRow.size());
        writePageIndex++;
        writeRow.clear();
        writeRowIndex = 0;
    }
}


void Table::sortRelation(bool flag)
{
    logger.log("Table::sortRelation");
    //These temp pages are required as intermediate files while merging.
    string tempTableName1 = "tEmPtable1";
    string tempTableName2 = "tEmPtable2";   

    bufferManager.clearPool(); 

    this->createTempPages(tempTableName1, tempTableName2);
    
    this->sortPages(tempTableName1, tempTableName2);

    int mergePageIndex = BLOCK_ALLOWED - 1;
    string readFrom = tempTableName1, writeTo = tempTableName2;

    while(mergePageIndex < this->blockCount)
    {   
        int firstPageIndex = 0, secondPageIndex = mergePageIndex;

        while(firstPageIndex < this->blockCount)
        {   
            if(secondPageIndex < this->blockCount)
                merge(readFrom, writeTo, firstPageIndex, secondPageIndex);
            else
                merge(readFrom, writeTo, firstPageIndex, firstPageIndex);
            firstPageIndex = secondPageIndex + mergePageIndex;
            secondPageIndex = firstPageIndex + mergePageIndex;
        }

        mergePageIndex+=mergePageIndex;
        string temp = readFrom;
        readFrom = writeTo;
        writeTo = temp;
    }
    
    int rowsIndex = 0;
    int pageIndex = 0;
    Cursor readCursor(readFrom, 0);

    vector<vector<int>> writeRow;
    int writeRowIndex = 0;

    while(rowsIndex < this->rowCount)
    {
        vector<int> temp = readCursor.getNext(false);
        rowsIndex++;

        writeRow.push_back(temp);
        writeRowIndex++;
        if(writeRowIndex == this->maxRowsPerBlock)
        {   
            bufferManager.writePage(this->tableName, pageIndex, writeRow, writeRow.size());
            pageIndex++;
            writeRow.clear();
            writeRowIndex = 0;
        }
    }
    if(writeRow.size())
    {
        bufferManager.writePage(this->tableName, pageIndex, writeRow, writeRow.size());
    }

    bufferManager.clearPool(); 
    if(flag)this->makePermanent();
    tableCatalogue.deleteTable(tempTableName1);
    tableCatalogue.deleteTable(tempTableName2);
}

/*
 * This function creates a copy of the given table
*/

string createCopy(Table * ogTable)
{
    logger.log("Table::createCopy");

    string ogTableName = ogTable->tableName;
    string copyTableName = ogTableName + "cOpY";

    Table * tempTable1 = new Table(copyTableName);   
    tableCatalogue.insertTable(tempTable1);

    tempTable1->columnCount = ogTable->columnCount;
    tempTable1->columns = ogTable->columns;
    tempTable1->rowCount = ogTable->rowCount;
    tempTable1->blockCount = ogTable->blockCount;
    tempTable1->maxRowsPerBlock = ogTable->maxRowsPerBlock;
    tempTable1->rowsPerBlockCount = ogTable->rowsPerBlockCount;

    Cursor readCursor(ogTableName, 0);
    vector<vector<int>> rows;
    int rowIndex = 0;
    int pageIndex = 0;

    for(int i = 0; i < ogTable->rowCount; i++)
    {
        rows.push_back(readCursor.getNext(false));
        rowIndex++;

        if(rowIndex == ogTable->maxRowsPerBlock)
        {
            bufferManager.writePage(copyTableName, pageIndex, rows, rows.size());
            pageIndex++;
            rows.clear();
            rowIndex = 0;
        }
    }

    if(rows.size())
    {
        bufferManager.writePage(copyTableName, pageIndex, rows, rows.size());
        rows.clear();
    }

    return copyTableName;
}

void setup_sort(string tableName, string colName)
{   
    logger.log("Table::setup_sort");

    parsedQuery.sortRelationName = tableName;
    parsedQuery.sortColumnNames.clear();
    parsedQuery.sortColumnNames.push_back(colName);
    parsedQuery.sortingStrategies.clear();
    parsedQuery.sortingStrategies.push_back(ASC);

    tableCatalogue.getTable(tableName)->sortRelation(false);
}

void setup_result_table()
{   
    logger.log("Table::setup_result_table");

    Table * table = tableCatalogue.getTable(parsedQuery.joinResultRelationName);
    Table * firstTable = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table * secondTable = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    table->columnCount = firstTable->columnCount + secondTable->columnCount;
    //rowCount and blockCount needs to be determined along the process.
    table->rowCount = table->blockCount = 0;
    table->maxRowsPerBlock = (int)(BLOCK_SIZE*1000)/(sizeof(int)*table->columnCount);
    table->columns.clear();

    for(auto x : firstTable->columns)table->columns.push_back(x);
    for(auto x : secondTable->columns)table->columns.push_back(x);
}

// Similar to lower_bound function of cpp.
// Only looks at top row of the block.
// So in order to not miss anything better to take return_value - 1 th block (unless the return value is zero)
// So if all the values present in all the blocks are less than key then return value will be blockCount + 1
int binarySearch(Table * table, int index, int key)
{   
    logger.log("Table::binarySearch");
    int ans = table->blockCount;
    int l = 0, r = ans - 1;
    vector<int> row;
    while(l <= r)
    {
        int mid = l + (r-l)/2;
        Cursor temp(table->tableName, mid);
        row = temp.getNext(false);
        if(row[index] < key)l = mid + 1;
        else
        {
            ans = mid;
            r = mid - 1;
        }
    }
    return ans;
}

//Given equality range for values from table1 and table2 , this will return possible blocks to look at for given Binary_op
pair<int,int> getStartEnd(int indexOne, int indexTwo, int endIndex)
{      
    logger.log("Table::getStartEnd");
    int index1 = (int)indexOne;
    int index2 = (int)indexTwo;
    int end_index = (int)endIndex;
    BinaryOperator op = parsedQuery.joinBinaryOperator;
    if(op == LESS_THAN)return {0, index1};
    if(op == GREATER_THAN)return {index2, end_index};
    if(op == LEQ)return {0, index2};
    if(op == GEQ)return {index1, end_index};
    if(op == EQUAL)return {index1, index2};
}

bool check(int val1, int val2)
{   
    logger.log("Table::check");

    BinaryOperator op = parsedQuery.joinBinaryOperator;

    if(op == LESS_THAN)return val1 < val2;
    if(op == LEQ)return val1 <= val2;
    if(op == GREATER_THAN)return val1 > val2;
    if(op == GEQ)return val1 >= val2;
    if(op == EQUAL)return val1 == val2;
}

/*
 * This function joins the two tables based on a given condition.
 * Conditions can be ==, <=, >=, < , >
*/
void Table::join()
{   
    logger.log("Table::Join");

    string copyTableOne = createCopy(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    string copyTableTwo = createCopy(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
    string resultTable = parsedQuery.joinResultRelationName;

    setup_sort(copyTableOne, parsedQuery.joinFirstColumnName);
    setup_sort(copyTableTwo, parsedQuery.joinSecondColumnName);

    setup_result_table();

    Table * table1 = tableCatalogue.getTable(copyTableOne);
    Cursor tableOneCursor(copyTableOne, 0);
    int rowsOne = table1->rowCount;

    Table * table2 = tableCatalogue.getTable(copyTableTwo);
    Cursor tableTwoCursor(copyTableTwo, 0);

    Table * result = tableCatalogue.getTable(resultTable);

    int firstColIndex = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int secondColIndex = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    int pageIndex = 0;
    int writeRowIndex = 0;
    int resultRowCount = 0;
    vector<vector<int>> writeRow;

    int blockIndexFrom = 0, blockIndexTo = table2->blockCount;
    for(int i = 0; i < rowsOne; i++)
    {   
        vector<int> rowOne = tableOneCursor.getNext(false);
        blockIndexFrom = binarySearch(table2, secondColIndex, rowOne[firstColIndex]) - 1;
        blockIndexTo = binarySearch(table2, secondColIndex, rowOne[firstColIndex] + 1) - 1;
        if(blockIndexFrom < 0)blockIndexFrom = 0;
        if(blockIndexTo < 0)blockIndexTo = 0;

        pair<int,int> index = getStartEnd(blockIndexFrom, blockIndexTo, table2->blockCount - 1);

        for(int j = index.first; j <= index.second; j++)
        {
            int rowsInBlock = table2->rowsPerBlockCount[j];
            tableTwoCursor = Cursor(table2->tableName, j);

            for(int k = 0; k < rowsInBlock; k++)
            {
                vector<int> rowTwo = tableTwoCursor.getNext(false);
                if(check(rowOne[firstColIndex], rowTwo[secondColIndex]))
                {
                    vector<int> temp;
                    for(auto x : rowOne)temp.push_back(x);
                    for(auto x : rowTwo)temp.push_back(x);
                    writeRow.push_back(temp);
                    writeRowIndex++;
                    resultRowCount++;

                    if(writeRowIndex == result->maxRowsPerBlock)
                    {
                        bufferManager.writePage(resultTable, pageIndex, writeRow, writeRow.size());
                        pageIndex++;
                        result->rowsPerBlockCount.push_back(writeRow.size());
                        writeRow.clear();
                        writeRowIndex = 0;
                    }
                }
            }
        }
    }
    if(writeRow.size())
    {
        bufferManager.writePage(resultTable, pageIndex, writeRow, writeRow.size());
        pageIndex++;
        result->rowsPerBlockCount.push_back(writeRow.size());
        writeRow.clear();
        writeRowIndex = 0;
    }
    
    result->blockCount = pageIndex;
    result->rowCount = resultRowCount;

    tableCatalogue.deleteTable(copyTableOne);
    tableCatalogue.deleteTable(copyTableTwo);
}

void setupOrderResult()
{
    logger.log("Table::setupOrderResult");

    Table * table = tableCatalogue.getTable(parsedQuery.orderResultRelationName);
    Table * firstTable = tableCatalogue.getTable(parsedQuery.orderRelationName);

    table->columnCount = firstTable->columnCount;
    table->rowCount = firstTable->rowCount;
    table->blockCount = firstTable->blockCount;
    table->maxRowsPerBlock = firstTable->maxRowsPerBlock;
    table->rowsPerBlockCount = firstTable->rowsPerBlockCount;
    table->columns.clear();
    for(auto x : firstTable->columns)
    {
        table->columns.push_back(x);
    }

    int pageIndex = 0;
    vector<vector<int>> writeRow;

    Cursor readCursor(parsedQuery.orderRelationName, 0);

    for(int i = 0; i < firstTable->rowCount; i++)
    {
        writeRow.push_back(readCursor.getNext(false));

        if(writeRow.size() == firstTable->maxRowsPerBlock)
        {
            bufferManager.writePage(table->tableName, pageIndex, writeRow, writeRow.size());
            writeRow.clear();
            pageIndex++;
        }
    }

    if(writeRow.size())
    {
        bufferManager.writePage(table->tableName, pageIndex, writeRow, writeRow.size());
        writeRow.clear();
    }
}
/*
 * The following function orders the given relation as required by the attributes and their strategy.
*/
void Table::order()
{
    logger.log("Table::order");

    setupOrderResult();

    parsedQuery.sortRelationName = parsedQuery.orderResultRelationName;
    parsedQuery.sortColumnNames.clear();
    parsedQuery.sortColumnNames = parsedQuery.orderColumnNames;
    parsedQuery.sortingStrategies.clear();
    parsedQuery.sortingStrategies = parsedQuery.orderingStrategies;

    tableCatalogue.getTable(parsedQuery.orderResultRelationName)->sortRelation(false);
}

void setup_group_result()
{
    Table * table = tableCatalogue.getTable(parsedQuery.groupResultRelationName);
    table->columnCount = 2;
    table->rowCount = 0;
    table->blockCount = 0;
    table->columns.clear();
    string colName = parsedQuery.aggregateTwo + "(" + parsedQuery.aggAttributeTwo + ")";
    table->columns.push_back(parsedQuery.groupingAttribute);
    table->columns.push_back(colName);
    table->maxRowsPerBlock = (BLOCK_SIZE*1000)/(sizeof(int)*table->columnCount);
    table->rowsPerBlockCount.clear();
}

bool checkGroupOp(int value)
{
    BinaryOperator op = parsedQuery.groupBinOp;
    if(op == LESS_THAN)return value < parsedQuery.attributeValue;
    else if(op == LEQ)return value <= parsedQuery.attributeValue;
    else if(op == GREATER_THAN)return value > parsedQuery.attributeValue;
    else if(op == GEQ)return value >= parsedQuery.attributeValue;
    else if(op == EQUAL)return value == parsedQuery.attributeValue;
}

/*
 * The following function performs the GROUP BY instruction.
*/
void Table::group()
{
    logger.log("Tabel::group");

    string copyTableName = createCopy(tableCatalogue.getTable(parsedQuery.groupRelationName));
    setup_group_result();
    setup_sort(copyTableName, parsedQuery.groupingAttribute);

    Table * resultTable = tableCatalogue.getTable(parsedQuery.groupResultRelationName);
    Table * ogTable = tableCatalogue.getTable(copyTableName);
    int colIndex = ogTable->getColumnIndex(parsedQuery.groupingAttribute);
    int colIndexOne = ogTable->getColumnIndex(parsedQuery.aggAttributeOne);
    int colIndexTwo = ogTable->getColumnIndex(parsedQuery.aggAttributeTwo);

    int aggSumOne = 0, aggCountOne = 0, aggMinOne = 0, aggMaxOne = 0;
    int aggSumTwo = 0, aggCountTwo = 0, aggMinTwo = 0, aggMaxTwo = 0;
    int pageIndex = 0;
    vector<vector<int>> writeRow;

    Cursor readCursor(copyTableName, 0);
    int value = INT_MAX;
    for(int i = 0; i < ogTable->rowCount; i++)
    {
        vector<int> readRow = readCursor.getNext(false);
        bool writeFlag = false;
        int writeValue;
        if(readRow[colIndex] != value)
        {
            if(i != 0)
            {
                if(parsedQuery.aggregateOne == "COUNT" && checkGroupOp(aggCountOne))writeFlag = true;
                else if(parsedQuery.aggregateOne == "SUM" && checkGroupOp(aggSumOne))writeFlag = true;
                else if(parsedQuery.aggregateOne == "MIN" && checkGroupOp(aggMinOne))writeFlag = true;
                else if(parsedQuery.aggregateOne == "MAX" && checkGroupOp(aggMaxOne))writeFlag = true;
                else if(checkGroupOp(aggSumOne/aggCountOne))writeFlag = true;

                if(parsedQuery.aggregateTwo == "COUNT")writeValue = aggCountTwo;
                else if(parsedQuery.aggregateTwo == "SUM")writeValue = aggSumTwo;
                else if(parsedQuery.aggregateTwo == "MIN")writeValue = aggMinTwo;
                else if(parsedQuery.aggregateTwo == "MAX")writeValue = aggMaxTwo;
                else writeValue = aggSumTwo/aggCountTwo;

                if(writeFlag)
                {
                    vector<int> temp;
                    temp.push_back(value);
                    temp.push_back(writeValue);
                    writeRow.push_back(temp);
                    resultTable->rowCount++;
                    if(writeRow.size() == resultTable->maxRowsPerBlock)
                    {
                        bufferManager.writePage(resultTable->tableName, pageIndex, writeRow, writeRow.size());
                        pageIndex++;
                        resultTable->rowsPerBlockCount.push_back(writeRow.size());
                        writeRow.clear();
                    }
                }
            }
            value = readRow[colIndex];
            aggSumOne = aggMinOne = aggMaxOne = readRow[colIndexOne];
            aggCountOne = 1;
            aggSumTwo = aggMinTwo = aggMaxTwo = readRow[colIndexTwo];
            aggCountTwo = 1;
        }
        else
        {
            aggSumOne += readRow[colIndexOne];
            aggMinOne = min(aggMinOne, readRow[colIndexOne]);
            aggMaxOne = max(aggMaxOne, readRow[colIndexOne]);
            aggCountOne++;

            aggSumTwo += readRow[colIndexTwo];
            aggMinTwo = min(aggMinTwo, readRow[colIndexTwo]);
            aggMaxTwo = max(aggMaxTwo, readRow[colIndexTwo]);
            aggCountTwo++;
        }
    }

    if(writeRow.size())
    {
        bufferManager.writePage(resultTable->tableName, pageIndex, writeRow, writeRow.size());
        pageIndex++;
        resultTable->rowsPerBlockCount.push_back(writeRow.size());
        writeRow.clear();
        resultTable->blockCount = pageIndex;
    }

    tableCatalogue.deleteTable(copyTableName);
}
