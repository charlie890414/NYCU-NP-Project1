#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <map>

using namespace std;

void setenv(string var, string value);

#if !__linux__
void clearenv();
#endif

void printenv(string var);

istream& getCommand(string &cmdStr);

void execute(vector<string> cmd);

void exit();