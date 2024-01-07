#include "global.h"

void splitString(string& x, vector<string>& ret)
{
    ret.clear();
    string temp = "";
    for(auto i : x)
    {
        if(i == '(' || i == ')')
        {
            ret.push_back(temp);
            temp = "";
        }
        else temp.push_back(i);
    }
}

bool checkAggFunction(string fnName)
{
    if(fnName == "MIN")return true;
    if(fnName == "MAX")return true;
    if(fnName == "SUM")return true;
    if(fnName == "COUNT")return true;
    if(fnName == "AVG")return true;

    return false;
}

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    
    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING"
    || tokenizedQuery[11] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    vector<string> miniToken;
    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupResultRelationName = tokenizedQuery[0];
    parsedQuery.groupingAttribute = tokenizedQuery[4];
    parsedQuery.groupRelationName = tokenizedQuery[6];
    splitString(tokenizedQuery[8], miniToken);
    parsedQuery.aggregateOne = miniToken[0];
    parsedQuery.aggAttributeOne = miniToken[1];
    parsedQuery.attributeValue = stoll(tokenizedQuery[10]);
    splitString(tokenizedQuery[12], miniToken);
    parsedQuery.aggregateTwo = miniToken[0];
    parsedQuery.aggAttributeTwo = miniToken[1];

    bool flag = checkAggFunction(parsedQuery.aggregateOne);
    if(!flag)
    {
        cout << "SYNTAX ERROR : " << parsedQuery.aggregateOne << ", No such function" << endl;
    }
    flag = checkAggFunction(parsedQuery.aggregateTwo);
    if(!flag)
    {
        cout << "SYNTAX ERROR : " << parsedQuery.aggregateTwo << ", No such function" << endl;
    }

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupBinOp = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupBinOp = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupBinOp = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupBinOp = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupBinOp = EQUAL;
    // else if (binaryOperator == "!=")
    //     parsedQuery.groupBinOp = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }
    if (!tableCatalogue.isTable(parsedQuery.groupRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupingAttribute, parsedQuery.groupRelationName)
        || !tableCatalogue.isColumnFromTable(parsedQuery.aggAttributeOne, parsedQuery.groupRelationName)
        || !tableCatalogue.isColumnFromTable(parsedQuery.aggAttributeTwo, parsedQuery.groupRelationName))
    {   
        cout << "SEMANTIC ERROR: Attribute error, No such column exists in given relation" << endl;
        return false;
    }  

    return true;
}

void executeGROUPBY()
{
    logger.log("executeGROUPBY");

    Table * table = new Table(parsedQuery.groupResultRelationName);
    tableCatalogue.insertTable(table);
    table->group();
    
    return;
}