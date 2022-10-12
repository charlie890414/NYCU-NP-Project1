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

string trim(string &str, const string &trimChars = " ")
{
    trimRight(str, trimChars);
    trimLeft(str, trimChars);
    return str;
}

vector<string> matchStr(string str, string matcher)
{
    vector<string> match;
    regex matcher_regex(matcher);
    sregex_iterator iter(str.begin(), str.end(), matcher_regex);
    sregex_iterator end;
    for (; iter != end; ++iter)
        match.emplace_back(iter->str());
    for (string &str : match)
        trim(str);
    return match;
}

vector<string> splitStr(string str, string delimiter)
{
    regex delimiter_regex(delimiter);
    sregex_token_iterator iter(str.begin(), str.end(), delimiter_regex, -1);
    sregex_token_iterator end;
    vector<string> split(iter, end);
    for (string &str : split)
        trim(str);
    return split;
}