CC= g++
CFLAGS = -Wall -g
LDFLAGS = -lrt -lm

all: init reader writer cleanall

init: init.cpp os3.h
	$(CC) $(FLAGS) -o init init.cpp $(LDFLAGS)

reader: reader.cpp os3.h 
	$(CC) $(FLAGS) -o reader reader.cpp $(LDFLAGS)

writer: writer.cpp os3.h
	$(CC) $(FLAGS) -o writer writer.cpp $(LDFLAGS)

cleanall: cleanall.cpp os3.h
	$(CC) $(FLAGS) -o cleanall cleanall.cpp $(LDFLAGS)

clean:
	@- $(RM) init reader writer cleanall

distclean: clean
