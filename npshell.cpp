#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <regex>
#include <map>
#include <unistd.h>
#include <sys/wait.h>

#include "lib/stringUtil.h"
#include "lib/systemUtil.h"
#include "lib/helper.h"

using namespace std;
int main() {
    string cmdsStr;
    while (getCommand(cmdsStr)) {
        vector < string > cmds = splitStr(cmdsStr, "\\s+\\|\\s+");
        vector < smatch > pipes = matchStr(cmdsStr, "\\|");
        // printIter(cmds);
        // printIter(pipes);

        vector<int *> pfds(cmds.size());
        for(int* &pfd: pfds){
            pfd = new int[2];
            if (pipe(pfd) < 0)
                return -1;
        }

        for (int i = 0; i < cmds.size(); i++) {
            string cmdStr = cmds[i];
            vector < string > cmd = splitStr(cmdStr, "\\s+");
            // printIter(cmd);
            if (cmd[0] == "")
                continue;
            else if (cmd[0] == "exit")
                exit();
            else if (cmd[0] == "setenv")
                setenv(cmd[1], cmd[2]);
            else if (cmd[0] == "printenv")
                printenv(cmd[1]);
            else {
                pid_t pid;
                int status;

                pid = fork();
                if (pid < 0) {
                    /* fork error */
                    return 0;
                } else if (pid == 0) {
                    /* child process */
                    if (i == 0){
                        if(i != cmds.size()-1){
                            close(pfds[i][0]);
                            dup2(pfds[i][1], STDOUT_FILENO);
                        }
                    }
                    else if(i == cmds.size()-1){
                        dup2(pfds[i-1][0], STDIN_FILENO);
                        close(pfds[i-1][1]);
                    }
                    else{
                        close(pfds[i][0]);
                        dup2(pfds[i][1], STDOUT_FILENO);
                        dup2(pfds[i-1][0], STDIN_FILENO);
                        close(pfds[i-1][1]);
                    }

                    execute(cmd);
                } else {
                    /* parent process */
                    if(i == cmds.size() - 1)
                        waitpid(pid, &status, 0);
                    
                    // not sure but work
                    if (i == 0){
                        if(i != cmds.size()-1){
                            close(pfds[i][1]);
                        }
                    }
                    else if(i == cmds.size()-1){
                        close(pfds[i-1][0]);
                    }
                    else{
                        close(pfds[i][1]);
                        close(pfds[i-1][0]);
                    }
                }
            }
        }
    }
    cout << endl;
    return 0;
}