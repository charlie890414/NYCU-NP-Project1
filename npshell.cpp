#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

#include "lib/stringUtil.h"
#include "lib/systemUtil.h"
#include "lib/helper.h"

using namespace std;

map<string, string> envp;

void setenv(string var, string value)
{
    envp.insert(pair<string, string>(var, value));
}

void printenv(string var)
{
    if (envp.contains(var))
        cout << envp[var] << endl;
}

void exit()
{
    exit(0);
}

int main()
{
    envp.insert(pair<string, string>("PATH", "bin:."));
    string cmdsStr;
    while (getCommand(cmdsStr))
    {
        vector<string> cmds = splitStr(cmdsStr, "\\s+\\|\\s+");
        vector<smatch> pipes = matchStr(cmdsStr, "\\|");
        printIter(cmds);
        printIter(pipes);
        for (string cmdStr : cmds)
        {
            vector<string> cmd = splitStr(cmdStr, "\\s+");
            printIter(cmd);
            if (cmd[0] == "exit")
                exit();
            else if (cmd[0] == "setenv")
                setenv(cmd[1], cmd[2]);
            else if (cmd[0] == "printenv")
                printenv(cmd[1]);
            else
            {
                cerr << "Unknown command: [" << cmd[0] << "]." << endl;
            }
        }
    }
    cout << endl;
    return 0;
}