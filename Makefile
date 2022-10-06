.PHONY: command

all: npshell

npshell: npshell.cpp
	g++ npshell.cpp -o npshell

command:
	$(foreach file, $(wildcard command/*.cpp), g++ -c $(file) -o bin/$(basename $(basename file) .cpp);)
	cp /bin/ls /bin/cat bin/
