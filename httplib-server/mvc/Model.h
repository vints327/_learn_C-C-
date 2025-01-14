// Model.h
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem>
#include <stdexcept>

class Model
{
public:
    void loadEnvFile(const std::string &filePath);
    std::string getEnvVariable(const std::string &key, const std::string &defaultValue);
};

#endif // MODEL_H
