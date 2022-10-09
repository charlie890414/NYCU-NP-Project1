import os
import subprocess

def run_cmd(question, gen_filename=None):
    ret = subprocess.getoutput(f'echo \'{question}\'| ./npshell')
    if gen_filename:
        os.remove(gen_filename) 
    return ret

def check_output(question, answer, gen_filename=None):
    ret = run_cmd(question, gen_filename)
    if answer == None:
        answer = run_cmd(question, gen_filename)
    assert ret == answer

def test_unknown():
    check_output('ctt', 'Unknown command: [ctt].')

def test_exit():
    check_output('exit', '')

def test_printenv():
    check_output('printenv PATH', 'bin:.')

def test_setenv():
    check_output('setenv PATH bin\nprintenv PATH', 'bin')

def test_ls():
    check_output('ls bin', None)

def test_cat_redirect():
    check_output('cat test.html > test1.txt\ncat test.txt', None, "test1.txt")
        
def test_removetag():
    check_output('bin/removetag test.html', None)

def test_removetag_redirect():
    check_output('bin/removetag test.html > test2.txt\ncat test2.txt', None, "test2.txt")

def test_removetag0():
    check_output('bin/removetag0 test.html', None)

def test_removetag0_redirect():
    check_output('bin/removetag0 test.html > test2.txt\ncat test2.txt', None, "test2.txt")