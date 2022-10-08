.PHONY: command

OBJ_FILE = lib/stringUtil.o lib/systemUtil.o lib/helper.o lib/builditin.o

all: npshell

npshell: npshell.cpp
	$(foreach file, $(OBJ_FILE), g++ -c $(basename $(file)).cpp -o $(file) -std=c++2a;)
	g++ $(OBJ_FILE) npshell.cpp -o npshell -std=c++2a

command:
	mkdir -p bin
	$(foreach file, $(wildcard command/*.cpp), g++ -c $(file) -o bin/$(basename $(notdir $(file)));)
	cp /bin/ls /bin/cat bin/
