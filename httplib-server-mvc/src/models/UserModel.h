#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <vector>
#include "../models/User.h"

class UserModel {
public:
    User getUserById(int id);
    bool createUser(const User &user);
    std::vector<User> getAllUsers();
};

#endif // USER_MODEL_H