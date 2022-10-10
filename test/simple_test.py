import os
import subprocess

def run_cmd(shell, question, gen_filename=None):
    ret = subprocess.getoutput(f'echo \'{question}\'| {shell}')
    if gen_filename:
        os.remove(gen_filename) 
    return ret

def check_output(question, answer, gen_filename=None):
    ret = run_cmd('./npshell', question, gen_filename)
    if answer == None:
        answer = run_cmd('bash', question, gen_filename)
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

def test_pipe():
    check_output("ls | cat", None)

# maybe need to setup env before test
def test_many_pipe():
    check_output("ls" + " | bin/number" * 1000, None)

def test_number_pipe1():
    check_output("bin/removetag test.html |2\nbin/removetag test.html |1\nbin/number", "   1 \n   2 Test\n   3 This is a test program\n   4 for ras.\n   5 \n   6 \n   7 Test\n   8 This is a test program\n   9 for ras.\n  10 ")

def test_inline_number_pipe1():
    check_output("bin/removetag test.html |2 bin/removetag test.html |1\nbin/number", "   1 \n   2 Test\n   3 This is a test program\n   4 for ras.\n   5 \n   6 \n   7 Test\n   8 This is a test program\n   9 for ras.\n  10 ")

def test_number_pipe2():
    check_output("bin/removetag0 test.html !1\nbin/number", "   1 Error: illegal tag \"!test.html\"\n   2 \n   3 Test\n   4 This is a test program\n   5 for ras.\n   6 ")