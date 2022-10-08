#include "systemUtil.h"

using namespace std;

map<string, string> envp = {{"PATH", "bin:."}};

void setenv(string var, string value)
{
    envp.insert(pair<string, string>(var, value));
}

void printenv(string var)
{
    if (envp.contains(var))
        cout << envp[var] << endl;
}

istream& getCommand(string &cmdStr)
{
    cout << "% ";
    return getline(cin, cmdStr);
}

void execute(vector<string> cmd){
    // create env value
    clearenv();
    for_each(begin(envp), end(envp),
                [](pair<const string, string> raw) {
                    char *c_raw = new char[raw.first.length() + 1 + raw.second.length() + 1];
                    strcpy(c_raw, (raw.first + "=" + raw.second).c_str());
                    putenv(c_raw);
                });

    // create arg value
    vector<char*>* argv = new vector<char*>();
    transform(begin(cmd), end(cmd), back_inserter(*argv),
                [](string raw) {
                    char *c_raw = new char[raw.length() + 1];
                    strcpy(c_raw, raw.c_str());
                    return c_raw;
                });
    argv->push_back(nullptr);

    // execute command
    if (execvp(cmd[0].c_str(), argv->data()) == -1) {
        cerr << "Unknown command: [" << cmd[0] << "]." << endl;
        exit(0);
    }
}

void exit()
{
    exit(0);
}