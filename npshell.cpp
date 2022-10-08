#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <unistd.h>
#include <sys/wait.h>

#include "lib/builditin.h"
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
        for (string cmdStr: cmds) {
            vector < string > cmd = splitStr(cmdStr, "\\s+");
            // printIter(cmd);
            if (cmd[0] == "exit")
                exit();
            else if (cmd[0] == "setenv")
                setenv(cmd[1], cmd[2]);
            else if (cmd[0] == "printenv")
                printenv(cmd[1]);
            else {
                char buffer[1024] = {
                    0
                };
                int len;
                int pfd[2];
                int status;
                pid_t pid;
                if (pipe(pfd) < 0)
                    return -1;

                pid = fork();
                if (pid < 0) {
                    /* fork error */
                    return 0;
                } else if (pid == 0) {
                    /* child process */
                    dup2(pfd[1], STDOUT_FILENO);
                    close(pfd[0]);
                    clearenv();
                    putenv((char * )"PATH=bin:.");
                    char *
                        const envp[] = {
                            const_cast < char * > (cmd[0].c_str()),
                            NULL
                        };
                    if (execvp(cmd[0].c_str(), envp) == -1) {
                        cerr << "Unknown command: [" << cmd[0] << "]." << endl;
                    }
                    exit(0);
                } else {
                    /* parent process */
                    close(pfd[1]);

                    while ((len = read(pfd[0], buffer, 1023)) > 0) {
                        buffer[len] = '\0';
                        printf("%s", buffer);
                    }

                    waitpid((pid_t) pid, & status, 0);
                }
            }
        }
    }
    cout << endl;
    return 0;
}