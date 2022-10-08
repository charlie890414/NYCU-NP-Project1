#include "builditin.h"

using namespace std;

class Enviroment
{
    map<string, string> envp = {{"PATH", "bin:."}};
public:
    void setenv(string var, string value)
    {
        envp.insert(pair<string, string>(var, value));
    }

    void printenv(string var)
    {
        if (envp.contains(var))
            cout << envp[var] << endl;
    }
};

Enviroment env;

void setenv(string var, string value)
{
    env.setenv(var, value);
}

void printenv(string var)
{
    env.printenv(var);
}

void exit()
{
    exit(0);
}