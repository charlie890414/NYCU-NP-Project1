.PHONY: command

OBJ_FILE = lib/stringUtil.o lib/systemUtil.o lib/helper.o

all: npshell

npshell: npshell.cpp
	$(foreach file, $(OBJ_FILE), g++ -c $(basename $(file)).cpp -o $(file) -std=c++2a;)
	g++ $(OBJ_FILE) npshell.cpp -o npshell -std=c++2a

command:
	mkdir -p bin
	$(foreach file, $(wildcard command/*.cpp), g++ $(file) -o bin/$(basename $(notdir $(file))) -std=c++2a;)
	cp /bin/ls /bin/cat bin/
