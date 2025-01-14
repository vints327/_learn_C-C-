#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <string>

class UserModel {
public:
    UserModel();
    ~UserModel();

    // 设置和获取用户信息
    void setName(const std::string& name);
    std::string getName() const;

    // 模拟数据库操作
    bool saveToDatabase() const;
    static UserModel getUserById(int userId);

private:
    std::string name;
    int id;
};

#endif // USER_MODEL_H