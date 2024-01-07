#ifndef __SYNPARSER__
#define __SYNPARSER__

#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    LOADMATRIX,
    PRINTMATRIX,
    TRANSPOSE,
    EXPORTMATRIX, 
    RENAMEMATRIX,
    CHECKSYMMETRY,
    COMPUTE,
    ORDERBY, 
    GROUPBY, 
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    vector<SortingStrategy> sortingStrategies;
    string sortResultRelationName = "";
    vector<string> sortColumnNames;
    string sortRelationName = "";

    string sourceFileName = "";

    string transposeMatrixName = "";
    string renameMatrixNameFrom = "";
    string renameMatrixNameTo = "";
    string computeMatrixName = "";

    string orderResultRelationName = "";
    vector<string> orderColumnNames;
    vector<SortingStrategy> orderingStrategies;
    string orderRelationName = "";

    string groupResultRelationName = "";
    string groupingAttribute = "";
    string groupRelationName = "";
    string aggregateOne = "";
    string aggAttributeOne = "";
    BinaryOperator groupBinOp;
    long long int attributeValue;
    string aggregateTwo = "";
    string aggAttributeTwo = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();

bool syntacticParseLOADMATRIX();
bool syntacticParsePRINTMATRIX();
bool syntacticParseTRANSPOSE();
bool syntacticParseEXPORTMATRIX();
bool syntacticParseRENAMEMATRIX();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();

bool syntacticParseORDERBY();
bool syntacticParseGROUPBY();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);

#endif
