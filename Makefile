.PHONY: command

all: npshell

npshell: npshell.cpp
	g++ npshell.cpp -o npshell -std=c++2a

command:
	mkdir -p bin
	$(foreach file, $(wildcard command/*.cpp), g++ -c $(file) -o bin/$(basename $(notdir $(file)) .cpp);)
	cp /bin/ls /bin/cat bin/
