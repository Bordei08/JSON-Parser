#pragma once
#include <stack>
#include <fstream>
#include <cctype>
#include "types.h"

class JSONParser
{
private:
    std::stack<std::string> checkStack;
    jobject* myJSON;

    bool checkDeliminator(char);
    std::vector<std::string> lexer(std::ifstream&);
    bool verifyJSON(std::vector<std::string>);
    std::vector<std::vector<std::string>> splitJSON(std::vector<std::string>);
    bool stackIsFree();
    jobject* createJObject(std::vector<std::vector<std::string>>);
    jlist* createJList(std::vector<std::string>);
    void printObject(jobject*, int);
    void printList(jlist*, int);
    void writeSpace(int);
    std::string getValue(json_value*);

public:
    JSONParser(std::string);
    ~JSONParser();
    
    void parser(std::vector<std::string>);
    const jobject* getMyHSON();
    void toString();
};

