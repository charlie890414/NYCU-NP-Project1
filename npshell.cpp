#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "lib/stringUtil.h"
#include "lib/systemUtil.h"
#include "lib/helper.h"

using namespace std;

int main()
{
    string cmdsStr;

    map<int, int *> number_pfds;
    map<int, int *> pfds;
    init_free_pfds();

    while (getCommand(cmdsStr))
    {
        bool START_OF_CMD = true;
        vector<string> cmds = splitStr(cmdsStr, "\\s+(\\!\\d+)|(\\|\\d+)|(\\|)\\s+");
        vector<string> pipes = matchStr(cmdsStr, "(\\!\\d+)|(\\|\\d+)|(\\|)");
        // printIter(cmds);
        // printIter(pipes);

        vector<int> pid_list;
        bool IS_COUNTDOWN = false;
        for (int i = 0; i < cmds.size(); i++)
        {
            string cmdStr = cmds[i];
            vector<string> cmd = splitStr(cmdStr, "\\s+");
            // printIter(cmd);

            if (cmd[0] == "")
            {
                IS_COUNTDOWN = true;
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
                pid_t pid;
                int round = -1;

                // need pipe
                if (i < pipes.size() && pipes[i].size() == 1)
                {
                    pfds[i] = get_free_pfds();
                }

                // need number pipe
                if (i < pipes.size() && pipes[i].size() > 1)
                {
                    string pipper = pipes[i];
                    round = stoi(pipper.substr(1, pipper.size() - 1));

                    // prepare new fd
                    if (!number_pfds.contains(round))
                    {
                        number_pfds[round] = get_free_pfds();
                    }
                }

                // don't know yet
                if (number_pfds.contains(0))
                {
                    close(number_pfds[0][1]);
                }

                while ((pid = fork()) < 0)
                {
                    // wait for any child process.
                    waitpid(-1, NULL, 0);
                }

                if (pid == 0)
                {
                    /* child process */
                    if (number_pfds.contains(0))
                    {
                        dup2(number_pfds[0][0], STDIN_FILENO);
                        close(number_pfds[0][1]);
                    }

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
                        close(number_pfds[round][0]);
                        dup2(number_pfds[round][1], STDOUT_FILENO);
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            dup2(pfds[i - 1][0], STDIN_FILENO);
                            close(pfds[i - 1][1]);
                        }
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '!')
                    {
                        close(number_pfds[round][0]);
                        dup2(number_pfds[round][1], STDOUT_FILENO);
                        dup2(number_pfds[round][1], STDERR_FILENO);
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

                    hold_pfds(pid, pfds[i]);
                    IS_COUNTDOWN = false;
                    number_pfds.erase(0);
                    pfds.erase(i - 2);

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
                        countdown(number_pfds);
                        START_OF_CMD = true;
                        IS_COUNTDOWN = true;
                    }
                    else if (i < pipes.size() && pipes[i].size() > 1 && pipes[i][0] == '!')
                    {
                        if (i > 0 && pipes[i - 1] == "|")
                        {
                            close(pfds[i - 1][0]);
                        }
                        countdown(number_pfds);
                        START_OF_CMD = true;
                        IS_COUNTDOWN = true;
                    }
                    else if (!START_OF_CMD)
                    {
                        close(pfds[i - 1][0]);
                    }
                }
            }
        }
        if (!IS_COUNTDOWN)
            countdown(number_pfds);

        // only wait cmd which need to show output
        if (pid_list.size() > 0 && pipes.size() < cmds.size())
        {
            waitpid(pid_list[pid_list.size() - 1], NULL, 0);
        }
        // printIter(number_pfds);
    }
    cleanup();
    while (wait(NULL) > 0);
    return 0;
}