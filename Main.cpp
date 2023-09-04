#include "Credentials.h"
#include "termcolor.hpp"
inline bool exists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
int main(int argc, char *argv[])
{
    creds Creds;
#ifdef _WIN32
    TCHAR path[MAX_PATH];
    HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path);

    if (FAILED(hr))
    {
        std::cout << termcolor::bold << termcolor::red << "[ERROR]Could not locate default windows Documents path"
                  << std::endl;
        return 1;
    }
    else
    {
        if (!exists(path + ("\\" + globalVars::fileName)))
            std::cout << termcolor::bold << termcolor::yellow << "Password file not found. Creating..." << std::endl;
    }
    globalVars::filename = path + ("\\" + globalVars::fileName);
#endif //_WIN32

#ifdef __unix__
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }
    std::string path = getenv("HOME");
    globalVars::fileName = path + "/Documents/" + globalVars::fileName;
    if (!exists(globalVars::fileName))
        std::cout << termcolor::bold << termcolor::yellow
                  << "Password file not found! Will be created when saving password." << std::endl;
#endif //__unix__

    if (argc >= 2 && strcmp(argv[1], "-f") == 0)
    {
        if (argc > 3)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            FindQueryResult result{};
            if (Creds.FindByNameOrPW(argv[2], argv[3], &result))
            {
                std::cout << termcolor::bold << termcolor::green << "Found [" << result.names.size() << "] results!"
                          << std::endl;
                std::cout << termcolor::bold << termcolor::green << "[Names] | [Passwords]\n---------------------"
                          << std::endl;
                for (int i = 0; i < (int)result.names.size(); i++)
                {
                    std::cout << termcolor::bold << termcolor::blue << "[" << result.names[i] << "] | ["
                              << result.pws[i] << "]" << std::endl;
                }
            }
            else
            {
                std::cout << termcolor::bold << termcolor::red << "No results found!" << std::endl;
            }
            return 0;
        }
        else if (argc == 3)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            FindQueryResult result{};
            if (Creds.FindByNameOrPW(argv[2], "NO PASSWORD GIVEN", &result))
            {
                std::cout << termcolor::bold << termcolor::green << "Found [" << result.names.size() << "] results!"
                          << std::endl;
                std::cout << termcolor::bold << termcolor::green << "[Names] | [Passwords]\n---------------------"
                          << std::endl;
                for (int i = 0; i < (int)result.names.size(); i++)
                {
                    std::cout << termcolor::bold << termcolor::blue << "[" << result.names[i] << "] | ["
                              << result.pws[i] << "]" << std::endl;
                }
            }
            else
            {
                std::cout << termcolor::bold << termcolor::red << "No results found!" << std::endl;
            }
            return 0;
        }
        else
        {
            ui::showHelp();
        }
    }
    if (argc >= 2 && strcmp(argv[1], "-s") == 0)
    {
        if (argc > 3)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            Creds.SaveCreds(argv[2], argv[3]);
            globalVars::writeFile.open(globalVars::fileName);
            Creds.Save();
            globalVars::writeFile.close();
            return 0;
        }
        else
        {
            ui::showHelp();
            return 0;
        }
    }
    if (argc >= 2 && std::strcmp(argv[1], "-g") == 0)
    {
        if (argc > 3)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            int numChars = std::stoi(argv[3]);
            if (numChars > 50 || numChars < 1)
            {
                ui::showHelp();
                std::cout << termcolor::bold << termcolor::red << "Please enter password length in range [1-50]!"
                          << std::endl;
                return 0;
            }
            Creds.GeneratePassword(argv[2], numChars);
            globalVars::writeFile.open(globalVars::fileName);
            Creds.Save();
            globalVars::writeFile.close();
            return 0;
        }
        else
        {
            ui::showHelp();
            return 0;
        }
    }
    if (argc >= 2 && strcmp(argv[1], "-c") == 0)
    {
        if (argc > 2)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            std::string foundPW = Creds.FindPassword(argv[2]);
            if (std::strcmp(foundPW.c_str(), "PASSWORD NOT AVAILABLE") == 0)
                std::cout << termcolor::bold << termcolor::red << "Password not found" << std::endl;
            else
                std::cout << termcolor::bold << termcolor::green << "Found password: [" << foundPW << "]" << std::endl;
        }
        else
        {
            ui::showHelp();
            return 0;
        }
    }
    if (argc >= 2 && strcmp(argv[1], "-v") == 0)
    {
        if (argc > 2)
        {
            globalVars::openFile.open(globalVars::fileName);
            Creds.Load();
            globalVars::openFile.close();
            Creds.GetMasterKey();
            std::string foundPW = Creds.FindPassword(argv[2]);
            if (std::strcmp(foundPW.c_str(), "PASSWORD NOT AVAILABLE") == 0)
                std::cout << termcolor::bold << termcolor::red << "Password not found" << std::endl;
            else
                std::cout << termcolor::bold << termcolor::green << "Found password: [" << foundPW << "]" << std::endl;
        }
        else
        {
            ui::showHelp();
            return 0;
        }
    }
    if (argc >= 2)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            ui::showTitle();
            ui::showHelp();
        }
    }

    if (argc == 1 || argc > 4)
    {
        ui::showTitle();
        ui::showHelp();
        return 0;
    }
    if (argc == 2)
    {
        globalVars::openFile.open(globalVars::fileName);
        Creds.Load();
        globalVars::openFile.close();
        Creds.GetMasterKey();
        std::string foundPW = Creds.FindPassword(argv[1]);
        if (std::strcmp(foundPW.c_str(), "PASSWORD NOT AVAILABLE") == 0)
            std::cout << termcolor::bold << termcolor::red << "Password not found" << std::endl;
        else
            std::cout << termcolor::bold << termcolor::green << "Found password: [" << foundPW << "]" << std::endl;
        return 0;
    }
    if (argc == 3 && std::strcmp(argv[1], "-c") != 0 && std::strcmp(argv[1], "-s") != 0 && strcmp(argv[1], "-h") != 0 &&
        strcmp(argv[1], "-g") != 0)
    {
        globalVars::openFile.open(globalVars::fileName);
        Creds.Load();
        globalVars::openFile.close();
        Creds.GetMasterKey();
        Creds.SaveCreds(argv[1], argv[2]);
        globalVars::writeFile.open(globalVars::fileName);
        Creds.Save();
        globalVars::writeFile.close();
        return 0;
    }
    return 0;
}
