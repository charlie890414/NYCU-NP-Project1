#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

map<string, string> envp;

void trimRight(string &str,
               const string &trimChars = " ")
{
    string::size_type pos = str.find_last_not_of(trimChars);
    str.erase(pos + 1);
}

void trimLeft(string &str,
              const string &trimChars = " ")
{
    string::size_type pos = str.find_first_not_of(trimChars);
    str.erase(0, pos);
}

void trim(string &str, const string &trimChars = " ")
{
    trimRight(str, trimChars);
    trimLeft(str, trimChars);
}

vector<smatch> matchStr(string str, string matcher)
{
    regex matcher_regex(matcher);
    sregex_iterator iter(str.begin(), str.end(), matcher_regex);
    sregex_iterator end;
    return {iter, end};
}

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
    if (envp.contains(var))
        cout << envp[var] << endl;
}

void exit()
{
    exit(0);
}

// helper function
void printIter(vector<string> iter)
{
    for (auto i : iter)
        cout << i << " ";
    cout << endl;
}

// helper function
void printIter(vector<smatch> iter)
{
    for (auto i : iter)
        cout << i.str() << " ";
    cout << endl;
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