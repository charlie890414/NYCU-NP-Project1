#include "helper.h"

using namespace std;

void printIter(vector<string> iter)
{
    for (auto i : iter)
        cout << i << " ";
    cout << endl;
}

void printIter(vector<smatch> iter)
{
    for (auto i : iter)
        cout << i.str() << " ";
    cout << endl;
}

void printIter(map<int, int*> iter)
{
    for (auto i : iter)
        cout << i.first << " ";
    cout << endl;
}