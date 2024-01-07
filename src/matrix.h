#ifndef __MATRIX__
#define __MATRIX__

#include "cursor.h"

// enum IndexingStrategy
// {
//     BTREE,
//     HASH,
//     NOTHING
// }; 

//When you uncomment the above block, it will give error in multiple places bcos of multiple
// declaration of IndexingStrategy (used in table.h) too.

// Same thing as table
// Read comments in table.h for description of the same.

class Matrix
{
    // vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string matrixName = "";
    // vector<string> columns;         //store the values of each column of the first row.
    // vector<uint> distinctValuesPerColumnCount;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    vector<uint> colsPerBlockCount;
    // bool indexed = false;
    // string indexedColumn = "";
    // IndexingStrategy indexingStrategy = NOTHING;
    
    // bool extractColumnNames(string firstLine);
    bool extractColumnDetails(string firstLine);    // newly created

    bool blockify();
    void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string tableName);
    Matrix(string tableName, vector<string> columns);
    bool load();
    // bool isColumn(string columnName);
    // void renameColumn(string fromColumnName, string toColumnName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();

    void transpose();
    void switch_them(Page &page);
    bool checkSymmetry();
    void compute();

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout, bool endLine = true)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        if(endLine == true)fout << endl;
        else fout << ", ";          
        
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};

#endif