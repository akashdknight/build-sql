#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * SORT <table_name> BY <c1, c2, ... ck> IN <ASC|DESC , ASC|DESC, ...........>
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");

    int sz = tokenizedQuery.size() - 4; // SORT tablename BY IN are subtracted
    sz = sz/2;
    if(tokenizedQuery.size() < 6 || tokenizedQuery[2] != "BY" || tokenizedQuery[2 + sz + 1] != "IN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    parsedQuery.queryType = SORT;

    parsedQuery.sortRelationName = tokenizedQuery[1];
    vector<string> colNames;
    for(int i = 0; i < sz; i++)
    {   
        string temp = tokenizedQuery[3+i];
        colNames.push_back(temp);
    }
        
    parsedQuery.sortColumnNames = colNames;
    parsedQuery.sortingStrategies.clear();

    for(int i = 0; i < sz; i++)
    {
        if(tokenizedQuery[4+sz+i] == "ASC")
            parsedQuery.sortingStrategies.push_back(ASC);
        else if(tokenizedQuery[4+sz+i] == "DESC")
            parsedQuery.sortingStrategies.push_back(DESC);
        else{
            cout<<"SYNTAX ERROR"<<endl;
            return false;
        }
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    //This is not required to us.
    // if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
    //     cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
    //     return false;
    // }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    for(auto col : parsedQuery.sortColumnNames)
    {
        if(!tableCatalogue.isColumnFromTable(col, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");

    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->sortRelation(true);
    return;
}