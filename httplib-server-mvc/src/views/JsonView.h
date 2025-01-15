#ifndef JSON_VIEW_H
#define JSON_VIEW_H

#include <nlohmann/json.hpp>
#include "../models/UserModel.h"

using json = nlohmann::json;

class JsonView {
public:
    static std::string user(const User &user);
    static std::string users(const std::vector<User> &users);
    static std::string error(const std::string &message);
};

#endif // JSON_VIEW_H