all : emulator

emulator: emulator.c
	gcc emulator.c -o emulator
	
clean:
	rm -rf emulator
