#include "systemUtil.h"

using namespace std;

istream& getCommand(string &cmdStr)
{
    cout << "% ";
    return getline(cin, cmdStr);
}