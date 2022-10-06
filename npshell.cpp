#include <iostream>
#include <string>
#include <regex>

using namespace std;

vector<string> splitCmdStr(string cmdStr){
    regex sep_regex("\\s+");
    sregex_token_iterator iter(cmdStr.begin(), cmdStr.end(), sep_regex, -1);
    sregex_token_iterator end;
    return {iter, end};
}

istream &getCommand(string &cmdStr)
{
    cout << "% ";
    return getline(cin, cmdStr);
}

// helper function
void printIter(auto iter){
    for(auto i: iter)
        cout<<i<<" ";
    cout<<endl;
}

int main()
{
    string cmdStr;
    while (getCommand(cmdStr))
    {
        vector<string> cmd = splitCmdStr(cmdStr);
        // printIter(cmd);
    }
    cout<<endl;
    return 0;
}