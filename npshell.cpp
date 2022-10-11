#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <regex>
#include <map>
#include <list>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <algorithm>
#include <errno.h>

#include "lib/stringUtil.h"
#include "lib/systemUtil.h"
#include "lib/helper.h"

using namespace std;

list<int *> free_pfds;
map<pid_t, int *> holding_pfds;

void recycle()
{
    map<int, int *> shadow(holding_pfds);
    for (pair<pid_t, int *> shadow_pfds : shadow)
    {
        pid_t pid = shadow_pfds.first;
        int status;
        waitpid(pid, &status, WNOHANG);
        if (WIFEXITED(status))
        {
            free_pfds.emplace_back(holding_pfds[pid]);
            holding_pfds.erase(pid);
        }
    }
}

int main()
{
    string cmdsStr;

    // create total pfds first
    for (int i = 0; i < 500; i++)
    {
        int *pfd = new int[2];
        if (pipe(pfd) < 0)
        {
            cout << strerror(errno) << endl;
            return -1;
        }
        free_pfds.emplace_back(pfd);
    }

    map<int, int *> number_pfds;
    map<int, int *> pfds;
    pid_t pid;

    int round = -1;
    int status;
    int *number_pfd;
    while (getCommand(cmdsStr))
    {
        bool START_OF_CMD = true;
        vector<string> cmds = splitStr(cmdsStr, "\\s+(\\!\\d+)|(\\|\\d+)|(\\|)\\s+");
        vector<string> pipes = matchStr(cmdsStr, "(\\!\\d+)|(\\|\\d+)|(\\|)");
        // printIter(cmds);
        // printIter(pipes);

        vector<int> pid_list;
        bool is_countdown = false;
        for (int i = 0; i < cmds.size(); i++)
        {
            string cmdStr = cmds[i];
            vector<string> cmd = splitStr(cmdStr, "\\s+");
            // printIter(cmd);

            if (i < pipes.size())
            {
                while (free_pfds.empty())
                {
                    usleep(1000);
                    recycle();
                }
                pfds[i] = free_pfds.front();
                free_pfds.pop_front();
            }

            if (cmd[0] == "")
            {
                is_countdown = true;
                continue;
            }
            else if (cmd[0] == "exit")
                exit();
            else if (cmd[0] == "setenv")
                setenv(cmd[1], cmd[2]);
            else if (cmd[0] == "printenv")
                printenv(cmd[1]);
            else
            {
                if (i < pipes.size() && pipes[i].size() > 1)
                {
                    string pipper = pipes[i];
                    round = stoi(pipper.substr(1, pipper.size() - 1));

                    // old fd or prepare new fd
                    if (number_pfds.contains(round))
                    {
                        number_pfd = number_pfds[round];
                    }
                    else
                    {
                        while (free_pfds.empty())
                        {
                            usleep(1000);
                            recycle();
                        }
                        number_pfd = free_pfds.front();
                        free_pfds.pop_front();
                    }
                }

                // don't know yet
                if (number_pfds.contains(0))
                {
                    close(number_pfds[0][1]);
                }

                while (holding_pfds.size() > 500)
                {
                    usleep(1000);
                    recycle();
                }

                pid = fork();
                if (pid < 0)
                {
                    /* fork error */
                    cout << strerror(errno) << endl;
                    return -1;
                }

                if (pid == 0)
                {
                    if (number_pfds.contains(0))
                    {
                        dup2(number_pfds[0][0], STDIN_FILENO);
                        close(number_pfds[0][1]);
                    }

                    /* child process */
                    if (pipes.size() == 0)
                    {
                        // nothing to do
                    }
                    else if (i < pipes.size() && pipes[i].size() == 1 && pipes[i][0] == '|')
                    {
                        if (START_OF_CMD)
                        {
                            close(pfds[i][0]);
                            dup2(pfds[i][1], STDOUT_FILENO);
                        }
                        else
                        {
                            close(pfds[i][0]);
                            dup2(pfds[i][1], STDOUT_FILENO);
                            dup2(pfds[i - 1][0], STDIN_FILENO);
                            close(pfds[i - 1][1]);
                        }
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '|')
                    {
                        close(number_pfd[0]);
                        dup2(number_pfd[1], STDOUT_FILENO);
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            dup2(pfds[i - 1][0], STDIN_FILENO);
                            close(pfds[i - 1][1]);
                        }
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '!')
                    {
                        close(number_pfd[0]);
                        dup2(number_pfd[1], STDOUT_FILENO);
                        dup2(number_pfd[1], STDERR_FILENO);
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            dup2(pfds[i - 1][0], STDIN_FILENO);
                            close(pfds[i - 1][1]);
                        }
                    }
                    else if (!START_OF_CMD)
                    {
                        dup2(pfds[i - 1][0], STDIN_FILENO);
                        close(pfds[i - 1][1]);
                    }

                    vector<string>::iterator iter = find(cmd.begin(), cmd.end(), ">");
                    if (iter != cmd.cend())
                    {
                        string filename = cmd[distance(cmd.begin(), iter) + 1];
                        int outfile = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
                        dup2(outfile, STDOUT_FILENO);
                        cmd = vector<string>(cmd.begin(), iter);
                    }

                    execute(cmd);
                }
                else
                {
                    /* parent process */
                    holding_pfds[pid] = pfds[i];
                    is_countdown = false;
                    number_pfds.erase(0);

                    pid_list.emplace_back(pid);

                    if (pipes.size() == 0)
                    {
                        // nothing to do
                    }
                    else if (i < pipes.size() && pipes[i].size() == 1 && pipes[i][0] == '|')
                    {
                        if (START_OF_CMD)
                        {
                            close(pfds[i][1]);
                            START_OF_CMD = false;
                        }
                        else
                        {
                            close(pfds[i][1]);
                            close(pfds[i - 1][0]);
                        }
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '|')
                    {
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            close(pfds[i - 1][0]);
                        }
                        number_pfds[round] = number_pfd;
                        countdown(number_pfds);
                        START_OF_CMD = true;
                        is_countdown = true;
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '!')
                    {
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            close(pfds[i - 1][0]);
                        }
                        number_pfds[round] = number_pfd;
                        countdown(number_pfds);
                        START_OF_CMD = true;
                        is_countdown = true;
                    }
                    else if (!START_OF_CMD)
                    {
                        close(pfds[i - 1][0]);
                    }
                }
            }
        }
        if (!is_countdown)
            countdown(number_pfds);

        // only wait to show output
        if (pid_list.size() > 0 && pipes.size() < cmds.size())
        {
            waitpid(pid_list[pid_list.size() - 1], &status, 0);
        }
        // printIter(number_pfds);
    }
    for (int *pfd : free_pfds)
    {
        delete pfd;
    }
    waitpid(-1, &status, 0);
    return 0;
}