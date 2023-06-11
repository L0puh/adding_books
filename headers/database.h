#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include "sqlite3.h"
int create_table();
int connect_db();
bool check_user(std::string name, int password);
#endif
