#include <vector>

enum TokenType {
    INVALID,
    UNKNOWN,
    INSTRUCTION,
    REGISTER,
    IMMEDIATE,
    SEPARATOR, //to dentify stuff like commas
    NEWLINE,
    LABEL,
    ENDOFFILE, // will have to look into how to properly implement this, also check if it's even needed
    HEADNODE
};

// enum of registers in 8085
enum Registers{
    INVALID_REGISTER,
    A, B, C, D, E,
    H, L, SP, PC, FLAG
};

// enum of instructions in 8085
enum Instructions{
    INVALID_INSTRUCTION,
    MOV, ADD, MVI, AND, ANI,
    XOR, SUB, INR, DCR, JNZ, JC, JNC, JMP
};

// will hold the string of instruction and type
// eg:- MOV A, B
struct Token{
    TokenType type;
    std::string value;

    Token(TokenType t, std::string v){
        type = t;
        value = v;
    }
};