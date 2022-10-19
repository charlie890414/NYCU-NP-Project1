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
    map<int, pid_t> number_pids;
    map<int, int *> pfds;

    while (getCommand(cmdsStr))
    {
        bool START_OF_CMD = true;
        bool IS_COUNTDOWN = false;
        vector<string> cmds = splitStr(cmdsStr, "\\s+(\\!\\d+)|(\\|\\d+)|(\\|)\\s+");
        vector<string> pipes = matchStr(cmdsStr, "(\\!\\d+)|(\\|\\d+)|(\\|)");
        // printIter(cmds);
        // printIter(pipes);

        pid_t last_pid;
        for (int i = 0; i < cmds.size(); i++)
        {
            string cmdStr = cmds[i];
            vector<string> cmd = splitStr(cmdStr, "\\s+");
            // printIter(cmd);

            if (cmd[0] == "exit")
                exit();
            else if (cmd[0] == "setenv")
                setenv(cmd[1], cmd[2]);
            else if (cmd[0] == "printenv")
                printenv(cmd[1]);
            else
            {
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

                // close for used
                if (number_pfds.contains(0))
                {
                    close(number_pfds[0][1]);
                }

                if (round != -1)
                {
                    waitpid(number_pids[round], NULL, 0);
                }

                while ((last_pid = fork()) < 0)
                {
                    // wait for any child process end.
                    waitpid(-1, NULL, 0);
                }

                if (last_pid == 0)
                {
                    /* child process */

                    // set number pipe stdin
                    if (number_pfds.contains(0))
                    {
                        dup2(number_pfds[0][0], STDIN_FILENO);
                        close(number_pfds[0][1]);
                    }

                    // set pipe input/output
                    if (pipes.size() == 0)
                    {
                        // nothing to do
                    }
                    else if (i < pipes.size() && pipes[i].size() == 1 && pipes[i][0] == '|')
                    {
                        // first cmd don't need to get stdin
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
                    // set stdout/stderr to pipe
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
                    // not pipe behind check if need input
                    else if (!START_OF_CMD)
                    {
                        dup2(pfds[i - 1][0], STDIN_FILENO);
                        close(pfds[i - 1][1]);
                    }

                    // file redirection
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
                    IS_COUNTDOWN = false;

                    // cleanup number pipe
                    if (number_pfds.contains(0))
                    {
                        delete number_pfds[0];
                        number_pfds.erase(0);
                        number_pids.erase(0);
                    }

                    if (round != -1)
                    {
                        number_pids[round] = last_pid;
                    }

                    // close fd if need
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
                        countdown(number_pids);
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
                        countdown(number_pids);
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
        {
            countdown(number_pfds);
            countdown(number_pids);
        }

        // only wait cmd which need to show output
        if (pipes.size() < cmds.size())
        {
            waitpid(last_pid, NULL, 0);
        }

        // cleanup
        for (auto &[_, pfd] : pfds)
        {
            delete pfd;
        }
        pfds.clear();
        // printIter(number_pfds);
    }
    return 0;
}