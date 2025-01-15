#include "JsonView.h"

std::string JsonView::user(const User &user) {
    json j;
    j["id"] = user.id;
    j["name"] = user.name;
    j["email"] = user.email;
    return j.dump();
}

std::string JsonView::users(const std::vector<User> &users) {
    json j = json::array();
    for (const auto &user : users) {
        j.push_back({{"id", user.id}, {"name", user.name}, {"email", user.email}});
    }
    return j.dump();
}

std::string JsonView::error(const std::string &message) {
    json j;
    j["error"] = message;
    return j.dump();
}