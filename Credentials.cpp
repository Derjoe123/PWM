#include "Credentials.h"

#include <cstring>
#include <random>

std::string creds::FindPassword(int index)
{
    if (index != -1)
    {
        int middle = globalVars::fileContents[index].find('|') + 1;
        int end = globalVars::fileContents[index].size();
        std::string ret = globalVars::fileContents[index].substr(middle, end - middle);
        return Xorify(ret, GetMasterKey());
    }
    else
    {
        return "PASSWORD NOT AVAILABLE";
    }
}
std::string creds::FindPassword(std::string name)
{
    int nameIndex = Find(name, "");
    if (nameIndex != -1)
    {
        int middle = globalVars::fileContents[nameIndex].find('|') + 1;
        int end = globalVars::fileContents[nameIndex].size();
        std::string ret = globalVars::fileContents[nameIndex].substr(middle, end - middle);
        return this->Xorify(ret, this->GetMasterKey());
    }
    else
    {
        return "PASSWORD NOT AVAILABLE";
    }
}
bool creds::MatchStrings(const char *first, const char *second)
{
    if (*first == '\0' && *second == '\0')
        return true;

    if (*first == '*')
    {
        while (*(first + 1) == '*')
            first++;
    }

    if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
        return false;

    if (*first == '?' || *first == *second)
        return MatchStrings(first + 1, second + 1);

    if (*first == '*')
        return MatchStrings(first + 1, second) || MatchStrings(first, second + 1);
    return false;
}
bool creds::FindByNameOrPW(std::string name, std::string password, FindQueryResult *result)
{
    if (std::strcmp(password.c_str(), "NO PASSWORD GIVEN") != 0)
    {
        for (int i{}; i < (int)globalVars::fileContents.size(); i++)
        {
            std::string currentLine = globalVars::fileContents[i];
            int middle = currentLine.find('|');
            std::string subStringName = Xorify(currentLine.substr(0, middle), GetMasterKey());

            if (this->MatchStrings(name.c_str(), subStringName.c_str()) ||
                this->MatchStrings(password.c_str(), this->FindPassword(subStringName).c_str()))
            {
                result->names.push_back(subStringName);
                result->indices.push_back(i);
                result->pws.push_back(this->FindPassword(subStringName));
            }
        }
    }
    else
    {
        for (int i{}; i < (int)globalVars::fileContents.size(); i++)
        {
            std::string currentLine = globalVars::fileContents[i];
            int middle = currentLine.find('|');
            std::string subStringName = Xorify(currentLine.substr(0, middle), GetMasterKey());

            if (this->MatchStrings(name.c_str(), subStringName.c_str()))
            {
                result->names.push_back(subStringName);
                result->indices.push_back(i);
                result->pws.push_back(this->FindPassword(subStringName));
            }
        }
    }
    if (result->pws.size() && result->names.size() && result->indices.size())
        return true;
    else
        return false;
}
bool creds::GeneratePassword(std::string name, int length)
{
    std::random_device random;
    std::mt19937 generator(random());
    std::uniform_int_distribution<> distrib(33, 122);
    std::string password{};
    std::string input{};
    while (true)
    {
        input = "";
        password = "";
        for (int i = 0; i < length; i++)
        {
            char curChar = (char)distrib(generator);
            password.push_back(curChar);
        }

        std::cout << termcolor::bold << termcolor::yellow << "Continue with Password [" << password
                  << "] (y), generate new Password(r) or break operation(n)?" << std::endl;
        std::cin.clear();
        std::cin >> input;

        if (input[0] == 'y' || input[0] == 'Y')
        {
            this->SaveCreds(name, password);
            return true;
        }
        else if (input[0] == 'n' || input[0] == 'N')
        {
            std::cout << termcolor::bold << termcolor::yellow << "Operation stopped password was not changed"
                      << std::endl;
            return false;
        }
        else if (input[0] == 'h' || input[0] == 'H')
        {
            ui::showHelp();
            return true;
        }
        else if (input[0] == 'r' || input[0] == 'R')
        {
            continue;
        }
        else
        {
            ui::showHelp();
            continue;
        }
    }
    return true;
}
bool creds::SaveCreds(std::string name, std::string password)
{
    int nameIndex = Find(name, password);
    if (nameIndex != -1)
    {
        std::string input{};
        std::cout << termcolor::bold << termcolor::yellow << "[" << name << "]"
                  << " Already exists with Password "
                  << "[" << this->FindPassword(nameIndex) << "]" << std::endl;
        std::cout << termcolor::bold << termcolor::yellow << "Replace Password(y), or break operation(n)?" << std::endl;
        while (true)
        {
            input = "";
            std::cin.clear();
            std::cin >> input;
            if (input[0] == 'y' || input[0] == 'Y')
            {
                globalVars::fileContents[nameIndex] =
                    this->Xorify(name, this->GetMasterKey()) + '|' + this->Xorify(password, this->GetMasterKey());
                std::cout << termcolor::bold << termcolor::green << "[" << password << "]"
                          << " Set for "
                          << "[" << name << "]" << std::endl;
                return true;
            }
            else if (input[0] == 'n' || input[0] == 'N')
            {
                std::cout << termcolor::bold << termcolor::yellow << "Operation stopped password was not changed"
                          << std::endl;
                return false;
            }
            else if (input[0] == 'h' || input[0] == 'H')
                ui::showHelp();
            std::cout << termcolor::bold << termcolor::red
                      << "Input not valid please reenter('h' for help page y' for replace or 'n' for break)"
                      << std::endl;
        }
    }
    else
    {
        globalVars::fileContents.push_back(this->Xorify(name, this->GetMasterKey()) + '|' +
                                           this->Xorify(password, this->GetMasterKey()));

        std::cout << termcolor::bold << termcolor::green << "[" << password << "]"
                  << " Set for "
                  << "[" << name << "]" << std::endl;
    }
    return true;
}
int creds::Find(std::string name, std::string password)
{
    if (!globalVars::fileContents.size())
        return -1;
    std::string currentLine;
    for (int i = 0; i < (int)globalVars::fileContents.size(); i++)
    {
        currentLine = globalVars::fileContents[i];
        int middle = currentLine.find('|');
        std::string subString = currentLine.substr(0, middle);
        // std::string subString2 = currentLine.substr(0, middle+1);
        // std::cout<<this->Xorify(subString2,getMasterKey())<<"  ";
        // std::cout<<this->Xorify(currentLine.substr(middle+1,currentLine.size()-(middle+1)),getMasterKey())<<std::endl;

        if (strcmp(subString.c_str(), this->Xorify(name, this->GetMasterKey()).c_str()) == 0)
            return i;
    }
    return -1;
}
bool creds::Load()
{
    // printf("Load\n");
    if (globalVars::fileContents.size())
        return false;
    std::string currentLine;
    while (std::getline(globalVars::openFile, currentLine))
    {
        // std::cout<<currentLine.c_str()<<std::endl;
        globalVars::fileContents.push_back(currentLine);
    }
    return true;
}
bool creds::Save()
{
    //: printf("Save\n");
    for (int i = 0; i < (int)globalVars::fileContents.size(); i++)
    {
        // std::cout <<globalVars::fileContents[i] <<std::endl;
        globalVars::writeFile << globalVars::fileContents[i] << std::endl;
    }
    return true;
}
std::string creds::GetMasterKey()
{
    std::string currentLine;
    for (int i = 0; i < (int)globalVars::fileContents.size(); i++)
    {
        currentLine = globalVars::fileContents[i];
        int middle = currentLine.find('|');
        std::string subString = currentLine.substr(0, middle);
        if (strcmp(subString.c_str(), this->Xorify("lightworknoreaction", globalVars::stdKey).c_str()) == 0)
        {
            int end = currentLine.size();
            std::string ret = currentLine.substr(middle, end - middle);
            return this->Xorify(ret, globalVars::stdKey);
        }
    }
    std::string wantedKey;
    std::cout << termcolor::bold << termcolor::yellow
              << "No master key found, please input a key(up to 20 non special characters without spaces)" << std::endl;
    std::cin >> wantedKey;
    globalVars::fileContents.push_back(this->Xorify("lightworknoreaction", globalVars::stdKey) + '|' +
                                       this->Xorify(wantedKey, globalVars::stdKey));
    std::cout << termcolor::bold << termcolor::green << "New master key set!" << std::endl;
    return wantedKey;
}
std::string creds::Xorify(std::string s, std::string key)
{
    std::string out = s;
    for (int i = 0; i < (int)s.size(); i++)
        out[i] = s[i] ^ key[i % (key.size() / sizeof(char))];
    return out;
}
void ui::showTitle()
{
    std::string LUIS = "██╗     ██╗   ██╗██╗███████╗\n██║     ██║   ██║██║██╔════╝\n██║     ██║   ██║██║███████╗\n██║  "
                       "   ██║   ██║██║╚════██║\n███████╗╚██████╔╝██║███████║\n╚══════╝ ╚═════╝ ╚═╝╚══════╝\n";
    std::string PASSWORD =
        "\n██████╗  █████╗ ███████╗███████╗██╗    ██╗ ██████╗ ██████╗ ██████╗\n██╔══██╗██╔══██╗██╔════╝██╔════╝██║    "
        "██║██╔═══██╗██╔══██╗██╔══██╗\n██████╔╝███████║███████╗███████╗██║ █╗ ██║██║   "
        "██║██████╔╝██║  ██║\n██╔═══╝ ██╔══██║╚════██║╚════██║██║███╗██║██║   ██║██╔══██╗██║  ██║\n██║     ██║  "
        "██║███████║███████║╚███╔███╔╝╚██████╔╝██║  ██║██████╔╝\n╚═╝     ╚═╝  "
        "╚═╝╚══════╝╚══════╝ ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═════╝\n";
    std::string MANAGER =
        "\n███╗   ███╗ █████╗ ███╗   ██╗ █████╗  ██████╗ ███████╗██████╗\n████╗ ████║██╔══██╗████╗  "
        "██║██╔══██╗██╔════╝ ██╔════╝██╔══██╗\n██╔████╔██║███████║██╔██╗ ██║███████║██║  ███╗█████╗  "
        "██████╔╝\n██║╚██╔╝██║██╔══██║██║╚██╗██║██╔══██║██║   ██║██╔══╝  ██╔══██╗\n██║ ╚═╝ ██║██║  ██║██║ ╚████║██║  "
        "██║╚██████╔╝███████╗██║  ██║\n╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ "
        "╚══════╝╚═╝  ╚═╝";
    std::cout << termcolor::blue << LUIS << termcolor::red << PASSWORD << termcolor::cyan << MANAGER << std::endl;
}
void ui::showHelp()
{
    std::cout << termcolor::bold << termcolor::green << "[Usage:]" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "pwm ";
    std::cout << termcolor::bold << termcolor::blue << "{-operation} {Name} {Password}" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "[Operations:]" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "-h";
    std::cout << termcolor::bold << termcolor::blue << " -> Help: Shows all commands and arguments" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "-s";
    std::cout << termcolor::bold << termcolor::blue << " -> Set: For given {Name} sets the {Password}" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "-v";
    std::cout << termcolor::bold << termcolor::blue << " -> View: For given {Name} shows the corresponding password"
              << std::endl;
    std::cout << termcolor::bold << termcolor::green << "-g";
    std::cout << termcolor::bold << termcolor::blue
              << " -> Create: For given {Name} generates a password with {length} of 1-50" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "-f";
    std::cout << termcolor::bold << termcolor::blue
              << " -> Find: Shows all Names and Passwords\n || matching {Name} and optionally {Password}" << std::endl;
    std::cout << termcolor::bold << termcolor::blue
              << " || The '?' wildcard ignores a single character where placed\n || The '*' wildcard ignores multiple "
                 "characters"
              << std::endl;
    std::cout << termcolor::bold << termcolor::blue
              << " || `G??gle`,`G*gle` or `*gle` will find `Google`\n || `G?gle` or `G*tgle` will not" << std::endl;
    std::cout << termcolor::bold << termcolor::green << "[Info:]" << std::endl;
    std::cout << termcolor::bold << termcolor::blue
              << "If no {Operation} is given, but {Name} and {Password} are, -s will be assumed" << std::endl;
    std::cout << termcolor::bold << termcolor::blue << "If no {Operation} is given, but {Name} is, -g will be assumed"
              << std::endl;
    std::cout << termcolor::bold << termcolor::blue
              << "If no Master key is found you need to provide one so the encryption can work\nSave it in a different "
                 "place, as all data is useless without it"
              << std::endl;
    std::cout << termcolor::bold << termcolor::blue
              << "For safety the file containing the passwords is named 'hrtyojhbczxa.txt'\nThis file is in the "
                 "Documents directory"
              << std::endl;
}

