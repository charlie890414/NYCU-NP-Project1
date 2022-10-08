#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

class Enviroment;

void setenv(string var, string value);

char** getenv();

void printenv(string var);

void exit();