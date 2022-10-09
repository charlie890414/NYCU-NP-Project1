import subprocess

def check_output(question, answer):
    ret = subprocess.getoutput(f'echo \'{question}\'| ./npshell')
    if not answer:
        answer = subprocess.getoutput(f'echo \'{question}\'| bash')
    assert ret == answer

def test_printenv():
    check_output('printenv PATH', 'bin:.')

def test_setenv():
    check_output('setenv PATH bin\nprintenv PATH', 'bin')

def test_ls():
    check_output('ls bin', None)

def test_cat():
    check_output('cat test.html', None)
        