.PHONY: command

all: npshell

npshell: npshell.cpp
	g++ -c npshell.cpp -o npshell

command:
	$(foreach file, $(wildcard command/*.cpp), g++ -c $(file) -o bin/$(basename $(basename file) .cpp);)
