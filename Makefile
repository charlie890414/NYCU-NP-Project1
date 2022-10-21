CC = clang++
CFLAGS = -O2 -std=c++2a

OBJFILES = lib/stringUtil.o lib/systemUtil.o lib/helper.o

all: npshell

npshell: npshell.cpp ${OBJFILES}                         
	${CC} ${OBJFILES} ${CFLAGS} $< -o $@
%.o: %.cpp %.h                      
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: command
command:
	mkdir -p bin
	$(foreach file, $(wildcard command/*.cpp), ${CC} $(file) -o bin/$(basename $(notdir $(file))) ${CFLAGS};)
	cp /bin/ls /bin/cat bin/

.PHONY: clean
clean:                             
	@rm -rf lib/*.o   