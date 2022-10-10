#include "systemUtil.h"

using namespace std;

map<string, string> envp = {{"PATH", "bin:."}};

void setenv(string var, string value)
{
    envp[var] = value;
}

#if !__linux__
extern char **environ;

void clearenv() {
  vector<string> data;
  for (auto it = environ; it && *it; ++it) {
    string entry(*it);
    auto equalsPosition = entry.find('=');
    if (equalsPosition == string::npos || equalsPosition == 0) {
      continue;
    } else {
      data.emplace_back(entry.substr(0, equalsPosition));
    }
  }

  for (auto s : data) {
    unsetenv(s.c_str());
  }
}
#endif

void printenv(string var)
{
    if (envp.contains(var))
        cout << envp[var] << endl;
}

istream& getCommand(string &cmdStr)
{
    if (isatty(fileno(stdin)))
        cout << "% ";
    istream& ret = getline(cin, cmdStr);
    if (isatty(fileno(stdin)) && !ret)
        cout << endl;
    return ret;
}

void execute(vector<string> cmd){
    // create env value
    clearenv();
    for_each(begin(envp), end(envp),
                [](pair<const string, string> raw) {
                    char *c_raw = new char[raw.first.length() + 1 + raw.second.length() + 1];
                    strcpy(c_raw, (raw.first + "=" + raw.second).c_str());
                    putenv(c_raw);
                });

    // create arg value
    vector<char*>* argv = new vector<char*>();
    transform(begin(cmd), end(cmd), back_inserter(*argv),
                [](string raw) {
                    char *c_raw = new char[raw.length() + 1];
                    strcpy(c_raw, raw.c_str());
                    return c_raw;
                });
    argv->push_back(nullptr);

    // execute command
    if (execvp(cmd[0].c_str(), argv->data()) == -1) {
        cerr << "Unknown command: [" << cmd[0] << "]." << endl;
        exit(0);
    }
}

void exit()
{
    exit(0);
}

void countdown(map<int, int*> &number_pfds)
{
    number_pfds.erase(0);
    map<int, int*> tmp(number_pfds);

    number_pfds.clear();
    for (auto tmp_number_pfd : tmp){
        number_pfds.insert(pair<int, int*>(tmp_number_pfd.first - 1, tmp_number_pfd.second));
    }
}