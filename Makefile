CC = g++
CFLAGS = -O3 -std=c++2a

OBJFILES = lib/stringUtil.o lib/systemUtil.o lib/helper.o

all: npshell

npshell: npshell.cpp ${OBJFILES}                         
	${CC} ${OBJFILES} ${CFLAGS} $< -o $@
%.o: %.cpp %.h                      
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: command
command:
	mkdir -p bin
	$(foreach file, $(wildcard command/*.cpp), g++ $(file) -o bin/$(basename $(notdir $(file))) -std=c++2a;)
	cp /bin/ls /bin/cat bin/

.PHONY: clean
clean:                             
	@rm -rf lib/*.o   