
// Model.cpp
#include "Model.h"
#include <iostream>

#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl;
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl;

void Model::loadEnvFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG_ERROR("Failed to open .env file: " << filePath);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            // 去除前后空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);  
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // 设置环境变量
            #ifdef _WIN32
                        _putenv_s(key.c_str(), value.c_str());
            #else
                        setenv(key.c_str(), value.c_str(), 1);
            #endif
        }
    }
    file.close();
}

std::string Model::getEnvVariable(const std::string &key, const std::string &defaultValue)
{
    const char *value = std::getenv(key.c_str());
    return value ? std::string(value) : defaultValue;
}