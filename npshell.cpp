#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

map<string, string> envp;

vector<string> splitStr(string str, string delimiter)
{
    regex delimiter_regex(delimiter);
    sregex_token_iterator iter(str.begin(), str.end(), delimiter_regex, -1);
    sregex_token_iterator end;
    return {iter, end};
}

istream &getCommand(string &cmdStr)
{
    cout << "% ";
    return getline(cin, cmdStr);
}

void setenv(string var, string value)
{
    envp.insert(pair<string, string>(var, value));
}

void printenv(string var)
{
    if(envp.contains(var))
        cout << envp[var] << endl;
}

void exit()
{
    exit(0);
}

// helper function
void printIter(auto iter)
{
    for (auto i : iter)
        cout << i << " ";
    cout << endl;
}

int main()
{
    envp.insert(pair<string, string>("PATH", "bin:."));
    string cmdStr;
    while (getCommand(cmdStr))
    {
        vector<string> cmd = splitStr(cmdStr, "\\s+");
        // printIter(cmd);
        if (cmd[0] == "exit") exit();
        else if(cmd[0] == "setenv") setenv(cmd[1], cmd[2]);
        else if(cmd[0] == "printenv") printenv(cmd[1]);
    }
    cout << endl;
    return 0;
}