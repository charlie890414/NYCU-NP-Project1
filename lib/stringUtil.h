#include <string>
#include <vector>
#include <regex>

using namespace std;

void trimRight(string &str,
               const string &trimChars);

void trimLeft(string &str,
              const string &trimChars);

string trim(string &str, const string &trimChars);

vector<string> matchStr(string str, string matcher);

vector<string> splitStr(string str, string delimiter);