SOURCES := CSP.cpp
# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SOURCES:.cpp=.o)

all: CSP
# Compile the binary 't' by calling the compiler with cflags, lflags, and any libs (if defined) and the list of objects.

CSP: CSP.cpp
	g++ CSP.cpp -o CSP
clean:
	$(RM) *.o *.d CSP
