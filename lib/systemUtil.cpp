#include "systemUtil.h"

using namespace std;

map<string, string> envp = {{"PATH", "bin:."}};

void setenv(string var, string value)
{
    envp[var] = value;
}

#if !__linux__
extern char **environ;

void clearenv()
{
    vector<string> data;
    for (auto it = environ; it && *it; ++it)
    {
        string entry(*it);
        auto equalsPosition = entry.find('=');
        if (equalsPosition == string::npos || equalsPosition == 0)
        {
            continue;
        }
        else
        {
            data.emplace_back(entry.substr(0, equalsPosition));
        }
    }

    for (auto s : data)
    {
        unsetenv(s.c_str());
    }
}
#endif

void printenv(string var)
{
    if (envp.contains(var))
        cout << envp[var] << endl;
}

istream &getCommand(string &cmdStr)
{
    // if (isatty(fileno(stdin)))
    cout << "% ";
    istream &ret = getline(cin, cmdStr);
    // if (isatty(fileno(stdin)) && !ret)
    //     cout << endl;
    return ret;
}

void execute(vector<string> cmd)
{
    // create env value
    clearenv();
    for_each(begin(envp), end(envp),
             [](pair<const string, string> raw)
             {
                 char *c_raw = new char[raw.first.length() + 1 + raw.second.length() + 1];
                 strcpy(c_raw, (raw.first + "=" + raw.second).c_str());
                 putenv(c_raw);
             });

    // create arg value
    vector<char *> *argv = new vector<char *>();
    transform(begin(cmd), end(cmd), back_inserter(*argv),
              [](string raw)
              {
                  char *c_raw = new char[raw.length() + 1];
                  strcpy(c_raw, raw.c_str());
                  return c_raw;
              });
    argv->push_back(nullptr);

    // execute command
    if (execvp(cmd[0].c_str(), argv->data()) == -1)
    {
        cerr << "Unknown command: [" << cmd[0] << "]." << endl;
        exit(0);
    }
}

void exit()
{
    exit(0);
}

list<int *> free_pfds;
map<pid_t, int *> holding_pfds;

void init_free_pfds()
{
    // create total pfds
    for (int i = 0; i < 500; i++)
    {
        int *pfd = new int[2];
        if (pipe(pfd) < 0)
        {
            cout << strerror(errno) << endl;
            return;
        }
        free_pfds.emplace_back(pfd);
    }
}

int *get_free_pfds()
{
    while (free_pfds.empty())
    {
        // wait for any child process.
        waitpid(-1, NULL, 0);
        recycle_holding_pfds();
    }
    int *ret = free_pfds.front();
    free_pfds.pop_front();
    return ret;
}

void hold_pfds(pid_t pid, int *pfds)
{
    holding_pfds[pid] = pfds;
}

void recycle_holding_pfds()
{
    map<pid_t, int *> shadow(holding_pfds);
    for (auto shadow_pfds : shadow)
    {
        int status;
        waitpid(shadow_pfds.first, &status, WNOHANG);
        if (WIFEXITED(status))
        {
            free_pfds.emplace_back(shadow[shadow_pfds.first]);
            holding_pfds.erase(shadow_pfds.first);
        }
    }
}

void countdown(map<int, int *> &number_pfds)
{
    map<int, int *> shadow(number_pfds);

    number_pfds.clear();
    for (auto shadow_number_pfd : shadow)
    {
        number_pfds.insert(pair<int, int *>(shadow_number_pfd.first - 1, shadow_number_pfd.second));
    }
}