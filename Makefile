all : emulator

emulator: emulator.c
	gcc emulator.c -o emulator -lncurses

clean:
	rm -rf emulator
