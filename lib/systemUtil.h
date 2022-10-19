#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void setenv(string var, string value);

#if !__linux__
void clearenv();
#endif

void printenv(string var);

istream &getCommand(string &cmdStr);

void execute(vector<string> cmd);

void exit();

void init_free_pfds();

int *get_free_pfds();

void hold_pfds(pid_t pid, int *pfds);

void recycle_holding_pfds();

void countdown(map<int, int *> &number_pfds);

void countdown(map<int, pid_t> &number_pids);

void cleanup();