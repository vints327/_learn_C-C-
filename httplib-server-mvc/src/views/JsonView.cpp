#include "JsonView.h"
#include <nlohmann/json.hpp> // 使用第三方 JSON 库

std::string JsonView::renderUser(const std::string& userName) {
    nlohmann::json jsonResponse;
    jsonResponse["user"] = userName;
    return jsonResponse.dump();  // 返回 JSON 字符串
}