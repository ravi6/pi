# Build OBOGS controller PID 
# Ravi Saripalli
#
CC = gcc
CFLAGS = -Wall -O -c -g -Wno-implicit-function-declaration

SRC = util.c i2c.c mcp3424.c mcp4725.c 
INC = $(SRC:.c=.h) common.h 
OBJ = $(SRC:.c=.o)

all:  pid 
 
#Implicit Rule to make object files
%.o : %.c %.h common.h
	@echo Compiling $<
	$(CC)  $(CFLAGS)  $< -o $@

libdev.a : $(OBJ)
	@echo $(OBJ)
	ar rc libdev.a $(OBJ)

pid :  pid.c common.h libdev.a
	gcc pid.c -L. -ldev -lm  -o pid
clean :
	rm $(OBJ) libdev.a pid
