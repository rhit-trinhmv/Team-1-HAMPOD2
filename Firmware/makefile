firmware.elf: firmware.o
	cc -Wall firmware.o -o firmware.elf -lwiringPi -lpthread
firmware.o: firmware.c
	cc $(CFLAGS) -Wall -c firmware.c -o firmware.o -lwiringPi -lpthread
firmware.c:
	@echo "Missing firmware.c file. Repull the git repo"

debug:
	@echo "Building Debug version"
	cc -DDEBUG -Wall -g -c firmware.c -o firmware.o -lwiringPi -lpthread
	cc -Wall firmware.o -o firmware.elf -lwiringPi -lpthread

clean:
	@rm *.o
	@rm *.elf
