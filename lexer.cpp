/* This is a lexer written by me in CPP (my first time using CPP so extensively for a project)
 * The implementation is probably not the cleanest or the best but it somewhat works for the limited
 * mnemonics defined.
 * The terms mnemonic and instruction have been incorrectly used interchangeably in the project.
 */

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

enum TokenType {
    INVALID,
    INSTRUCTION,
    REGISTER,
    IMMEDIATE,
    SEPARATOR, //to dentify stuff like commas
    NEWLINE,
    LABEL,
    ENDOFFILE // will have to look into how to properly implement this, also check if it's even needed
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
    XOR, SUB, INR, DCR
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

// prints the token value and type
void printToken(const Token& token) {
    std::string typeName;

    // added for debugging
    // label tokenization caused a whole plethora of errors
    switch (token.type) {
        case INVALID: typeName = "INVALID"; break;
        case INSTRUCTION: typeName = "INSTRUCTION"; break;
        case REGISTER: typeName = "REGISTER"; break;
        case IMMEDIATE: typeName = "IMMEDIATE"; break;
        case SEPARATOR: typeName = "SEPARATOR"; break;
        case NEWLINE: typeName = "NEWLINE"; break;
        case LABEL: typeName = "LABEL"; break;
        case ENDOFFILE: typeName = "ENDOFFILE"; break;
        default: typeName = "UNKNOWN"; break;
    }

    std::cout << "Type: " << typeName << " (" << token.type << "), Value: " << token.value << std::endl;
}

// function to create a token of the Token struct
Token* createToken(TokenType type, const std::string& value) {
    return new Token(type, value);
}

// will return the relevant register enum by using the string as a key
// unordered map implemented to find register
Registers findRegisterEnum(const std::string& value){
    std::unordered_map<std::string, Registers> regMap = {
        {"A",A}, {"B",B}, {"C",C}, {"D",D}, {"E",E},
        {"H",H}, {"L",L}, {"SP",SP}, {"PC",PC}, {"FLAG",FLAG}
    };
    // auto is like var, automatic type inference
    // found is an iterator, iterators in C++ containers act like pointers
    auto found = regMap.find(value);
    // -> first stores the key part in an unordered map and
    // -> second stores the value part
    return found != regMap.end() ? found->second : INVALID_REGISTER;
}

// will return the relevant instruction enum by using the string as a key
// unordered map implemented to find instruction
Instructions findInstructionEnum(const std::string& value){
    std::unordered_map<std::string, Instructions> insMap = {
         {"MOV", MOV}, {"ADD", ADD}, {"MVI", MVI}, {"AND", AND}, {"ANI", ANI}, 
         {"XOR", XOR}, {"SUB", SUB}, {"INR", INR}, {"DCR", DCR}
    };
    // auto is like var, automatic type inference
    // found is an iterator, iterators in C++ containers act like pointers
    auto found = insMap.find(value);
    // -> first stores the key part in an unordered map and
    // -> second stores the value part
    return found != insMap.end() ? found->second : INVALID_INSTRUCTION;
}

// coulda written this to recursivly traverse the tokens
// right now it returns a vec of tokens of type Token*
std::vector<Token*> tokenize(const char* str) {
    std::vector<Token*> tokens;

    while (*str) {
        // skip any whitespaces present at the start
        while (*str && std::isspace(*str) && *str != '\n') {
            str++;
        }

        // check if we're dealing with instructions or registers or labels
        if (std::isalnum(*str) || *str == ':') {
            std::string value;
            bool isLabel = false;

            // append alphanum elements plus colon for label
            while (std::isalnum(*str) || *str == ':') {
                if (*str == ':') isLabel = true;
                value += *str;
                str++;
            }

            if (isLabel) {
                tokens.push_back(createToken(LABEL, value));
            } 
            else {
                // Check if it is an instruction
                Instructions instr = findInstructionEnum(value);
                if (instr != INVALID_INSTRUCTION) {
                    tokens.push_back(createToken(INSTRUCTION, value));
                } 
                else {
                    // Check if it is a register
                    Registers reg = findRegisterEnum(value);
                    if (reg != INVALID_REGISTER) {
                        // invalid register
                        tokens.push_back(createToken(REGISTER, value));
                    } 
                    else {
                        // invalid token type
                        tokens.push_back(createToken(INVALID, value));
                    }
                }
            }
            continue;
        }

        // check if we're dealing with digits (immediate values)
        else if (std::isdigit(*str)) {
            std::string value;
            while (std::isdigit(*str)) {
                value += *str;
                str++;
            }
            tokens.push_back(createToken(IMMEDIATE, value));
        } 
        
        // check if a comma separator has been encountered
        else if (*str == ',') {
            tokens.push_back(createToken(SEPARATOR, ","));
            str++;
        } 
        
        // check if a newline is encountered
        else if (*str == '\n'){
            tokens.push_back(createToken(NEWLINE, "\n"));
            str++;
        }

        // if none of the above criteria are met, keep iterating
        else {
            str++;
        }
    }
    return tokens;
}

int main(){

    // open file
    std::ifstream file("code.txt");

    // check if file opened
    if(!file.is_open()){
        std::cerr<<"File could not be opened"<<std::endl;
        return 1;
    }

    // read file into some string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    // close file
    file.close();
    fileContent += "\n";

    // c.str() will convert from std::string to char*
    std::vector<Token*> tokens = tokenize(fileContent.c_str());
    
    for (Token* token : tokens) {
        printToken(*token);
        delete token; // free memory
    }

    return 0;
}