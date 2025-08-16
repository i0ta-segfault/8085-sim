# 8085 sim

Tbh this is really just a shot at implementing Intel's 8085 $ \mu $p. I have been successful in achieving only a subset of the processor's functions.
Out of the 256 instructions in the Instr. Set of the microprocessor, nearly 150 have been implemented.

Using the program is quite simple actually, but if you need help running the application, enter:

```bash
./emulator --help
```
This should bring up a "man" page for the application!

If you wanna compile the binaries for yourself just run :
```bash
make clean
```
```bash
make
```
Once complete, run :

```bash
./emulator            #run the application
```

The commands are case sensitive (they are only valid in uppercase)
