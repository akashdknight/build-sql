#include "global.h"
#include <fstream>
/**
 * @brief
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");
    // MODIFIED FROM HERE.
    string path = "../data/";
    path += parsedQuery.sourceFileName;
    path += ".ra";
    std::ifstream in(path);
    std::streambuf *cinbuf = std::cin.rdbuf(); // save old buf
    std::cin.rdbuf(in.rdbuf());                // redirect std::cin to in.txt!

    cout << "Running from " << path << " file" << endl;

    regex delim("[^\\s,]+");
    string command;
    system("rm -rf ../data/temp");
    system("mkdir ../data/temp");

    while (!cin.eof())
    {
        cout << "\n> ";
        tokenizedQuery.clear();
        parsedQuery.clear();
        logger.log("\nReading New Command: ");
        getline(cin, command);
        logger.log(command);

        cout << command << endl;
        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
        {
            break;
        }

        if (tokenizedQuery.empty())
        {
            continue;
        }

        if (tokenizedQuery.size() == 1)
        {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }

        logger.log("doCommand inside SOURCE");
        if (syntacticParse() && semanticParse())
            executeCommand();
    }

    std::cin.rdbuf(cinbuf); // reset to standard input again
    return;
}
