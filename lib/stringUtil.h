#include <string>
#include <vector>
#include <regex>

using namespace std;

void trimRight(string &str,
               const string &trimChars);

void trimLeft(string &str,
              const string &trimChars);

void trim(string &str, const string &trimChars);

vector<smatch> matchStr(string str, string matcher);

vector<string> splitStr(string str, string delimiter);