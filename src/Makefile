CC=gcc

CFLAGS=-pthread

LDIR =../lib

DEPS = ../deps/*/*.h ../deps/*/*.c

SERVER=server/*.h server/*.c

CLIENT=client/*.h client/*.c

OUTFILE_SERVER = $(LDIR)/server.o

OUTFILE_CLIENT = $(LDIR)/client.o

all: clean server client

server:
	mkdir -p $(LDIR)
	$(CC) -o $(OUTFILE_SERVER) $(CFLAGS) $(DEPS) $(SERVER)

client:
	mkdir -p $(LDIR)
	$(CC) -o $(OUTFILE_CLIENT) $(CFLAGS) $(DEPS) $(CLIENT)

.PHONY: server client clean

# clean the object files in ODIR and clean all core generated files including temp files (*~)
clean:
	rm -rf $(LDIR)/*.o
