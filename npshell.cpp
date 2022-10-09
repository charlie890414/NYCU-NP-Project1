#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <regex>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <algorithm>

#include "lib/stringUtil.h"
#include "lib/systemUtil.h"
#include "lib/helper.h"

using namespace std;

int main() {
    string cmdsStr;
    map<int, int*> number_pfds;
    while (getCommand(cmdsStr)) {
        vector < string > cmds = splitStr(cmdsStr, "\\s+(\\!\\d+)|(\\|\\d+)|(\\|)\\s+");
        vector < smatch > pipes = matchStr(cmdsStr, "(\\!\\d+)|(\\|\\d+)|(\\|)");
        // printIter(cmds);
        printIter(pipes);

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

                int round = -1;
                if (i < pipes.size() && pipes[i].str().size() > 1){
                    string pipper = pipes[i].str();
                    round = stoi(pipper.substr(1, pipper.size()-1));
                }

                // old fd or prepare new fd
                int* pfd;
                if(number_pfds.contains(round)){
                    pfd = number_pfds[round];
                } else {
                    pfd = new int[2];
                    if (pipe(pfd) < 0)
                        return -1;
                }
                cout<<pfd[0]<<endl;
                cout<<pfd[1]<<endl;

                pid = fork();
                if (pid < 0) {
                    /* fork error */
                    return 0;
                } else if (pid == 0) {
                    for (auto number_pfd : number_pfds){
                        if(number_pfd.first == 0){
                            dup2(number_pfd.second[0], STDIN_FILENO);
                            close(number_pfd.second[1]);
                        }
                    }

                    /* child process */
                    if (pipes.size() == 0) {
                        // nothing to do
                    }
                    else if(i < pipes.size() && pipes[i].str().size() == 1 && pipes[i].str()[0] == '|'){
                        if (i == 0){
                            close(pfds[i][0]);
                            dup2(pfds[i][1], STDOUT_FILENO);
                        }
                        else{
                            close(pfds[i][0]);
                            dup2(pfds[i][1], STDOUT_FILENO);
                            dup2(pfds[i-1][0], STDIN_FILENO);
                            close(pfds[i-1][1]);
                        }
                    }
                    else if(i < pipes.size() && pipes[i].str().size() > 1 && pipes[i].str()[0] == '|'){
                        close(pfd[0]);
                        dup2(pfd[1], STDOUT_FILENO);
                    }
                    else if(i < pipes.size() && pipes[i].str().size() > 1 && pipes[i].str()[0] == '!'){
                        close(pfd[0]);
                        dup2(pfd[1], STDOUT_FILENO);
                        dup2(pfd[1], STDERR_FILENO);
                    }
                    else{
                        dup2(pfds[i-1][0], STDIN_FILENO);
                        close(pfds[i-1][1]);
                    }

                    vector<string>::iterator iter = find(cmd.begin(), cmd.end(), ">");
                    if(iter != cmd.cend()){
                        string filename = cmd[distance(cmd.begin(), iter) + 1];
                        int outfile = open(filename.c_str(), O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
                        dup2(outfile, STDOUT_FILENO);
                        cmd = vector<string>(cmd.begin(), iter);
                    }

                    execute(cmd);
                } else {
                    /* parent process */
                    if(i == cmds.size() - 1)
                        waitpid(pid, &status, 0);
                    
                    if (pipes.size() == 0) {
                        // nothing to do
                    }
                    else if(i < pipes.size() && pipes[i].str().size() == 1 && pipes[i].str()[0] == '|'){
                        if (i == 0){
                            close(pfds[i][1]);
                        }
                        else{
                            close(pfds[i][1]);
                            close(pfds[i-1][0]);
                        }
                    }
                    else if(i < pipes.size() && pipes[i].str().size() > 1 && pipes[i].str()[0] == '|'){
                        // close(pfd[1]);
                        number_pfds.insert(pair<int, int*>(round, pfd));
                    }
                    else if(i < pipes.size() && pipes[i].str().size() > 1 && pipes[i].str()[0] == '!'){
                        // close(pfd[1]);
                        number_pfds.insert(pair<int, int*>(round, pfd));
                    }
                    else{
                        close(pfds[i-1][0]);
                    }

                    number_pfds.erase(0);
                    map<int, int*> tmp(number_pfds);

                    for (auto number_pfd : tmp){
                        number_pfds.erase(number_pfd.first);
                        number_pfds.insert(pair<int, int*>(number_pfd.first - 1, number_pfd.second));
                    }

                    printIter(number_pfds);
                }
            }
        }
    }
    return 0;
}