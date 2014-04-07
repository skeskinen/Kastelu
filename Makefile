CC=g++
CFLAGS=-Wall --std=c++11 -g
LDFLAGS=-lwt -lwthttp -lwtdbo -lwtdbosqlite3
SRCS=main.cpp app.cpp stat_wdgt.cpp line_wdgt.cpp config_wdgt.cpp prog_wdgt.cpp dbo.cpp worker.cpp datatypes.cpp sunriset.cpp
OBJS = $(SRCS:.cpp=.o)

EXEC=kastelu

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(OBJS)

run: all
	./${EXEC} --docroot . --http-address 127.0.0.1 --http-port 8080

debug: all
	gdb --args ./${EXEC} --docroot . --http-address localhost --http-port 8080

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(EXEC)

