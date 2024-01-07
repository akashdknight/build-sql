#include "global.h"

bool syntacticParseORDERBY()
{
    logger.log("syntacticParseORDERBY");
    int sz = tokenizedQuery.size();
    int noOfAttributes = sz - 6;    //2* number of attributes
    if (tokenizedQuery.size() < 8 || tokenizedQuery[3] != "BY" || noOfAttributes%2 || tokenizedQuery[4 + noOfAttributes] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderResultRelationName = tokenizedQuery[0];
    parsedQuery.orderRelationName = tokenizedQuery.back();

    parsedQuery.orderColumnNames.clear();
    parsedQuery.orderingStrategies.clear();

    for(int i = 0; i < noOfAttributes/2; i++)
    {
        parsedQuery.orderColumnNames.push_back(tokenizedQuery[4+i]);
        SortingStrategy temp;
        if(tokenizedQuery[4+ noOfAttributes/2 + i] == "ASC")temp = ASC;
        else if(tokenizedQuery[4 + noOfAttributes/2 + i] == "DESC")temp = DESC;
        else 
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.orderingStrategies.push_back(temp);
    }

    return true;
}

bool semanticParseORDERBY()
{
    logger.log("semanticParseORDERBY");

    if (tableCatalogue.isTable(parsedQuery.orderResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    for(auto col : parsedQuery.orderColumnNames)
    {
        if(!tableCatalogue.isColumnFromTable(col, parsedQuery.orderRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }
    return true;
}

void executeORDERBY()
{
    logger.log("executeORDERBY");

    Table * table = new Table(parsedQuery.orderResultRelationName);
    tableCatalogue.insertTable(table);
    table->order();
    
    return;
}