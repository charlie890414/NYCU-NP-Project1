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

    char** getenv()
    {
        vector<char*>* vec = new vector<char*>();
        transform(begin(envp), end(envp), back_inserter(*vec),
                    [](pair<const string, string>& p) {
                        char *raw = new char[p.first.length() + 1 + p.second.length() + 1];
                        strcpy(raw, (p.first + "=" + p.second).c_str());
                        cout<<raw<<endl;
                        return raw;
                    });
        vec->push_back(nullptr);
        return vec->data();
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

char** getenv(){
    return env.getenv();
}

void printenv(string var)
{
    env.printenv(var);
}

void exit()
{
    exit(0);
}