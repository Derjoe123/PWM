#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "termcolor.hpp"
#include <random>
#ifdef __unix__
#include <linux/input.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif//__unix__
#ifdef _WIN32
#include <windows.h>
#include <filesystem>
#include <shlobj_core.h>
#endif//_WIN32
class globalVars
{
public:
    inline static std::string fileName = "hrtyojhbczxa.txt";
    inline static std::fstream openFile;
    inline static std::ofstream writeFile;
    inline static std::vector<std::string> fileContents = {};
    inline static char stdKey[5] = {'`','b','1','j','&'};
};

typedef struct 
{
    std::vector<int> indices;
    std::vector<std::string> names;
    std::vector<std::string> pws;
}FindQueryResult;

class creds
{
public:
    bool SaveCreds(std::string name ,std::string password);
    int Find(std::string name ,std::string password);
    std::string FindPassword(std::string name);
    std::string FindPassword(int index);
    std::string GetMasterKey();
    bool MatchStrings(const char* first,const char* second);
    bool FindByNameOrPW(std::string name,std::string password,FindQueryResult* result);
    bool GeneratePassword(std::string name,int length);
    bool Load();
    bool Save();
private:
    std::string Xorify(std::string s,std::string key);






public:

};

class ui{
public:
    static void showTitle();
    static void showHelp();

public:
    
};
