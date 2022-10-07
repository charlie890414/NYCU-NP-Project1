#include "stringUtil.h"

using namespace std;

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