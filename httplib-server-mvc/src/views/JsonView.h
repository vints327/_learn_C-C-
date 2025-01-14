#ifndef JSON_VIEW_H
#define JSON_VIEW_H

#include <string>
#include <nlohmann/json.hpp> // 使用第三方 JSON 库

class JsonView {
public:
    static std::string renderUser(const std::string& userName);
};

#endif // JSON_VIEW_H
