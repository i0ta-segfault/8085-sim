/* This is the parser, it creates an abstract syntax tree
 * the ast will have the tokens as nodes
 * some abstract "Program" token will be considered
 * as root node
 * 
 * std::vector<Token*> tokens;
 * say, tokens = [LABEL, INSTRUCTION, REGISTER, SEPARATOR, IMMEDIATE, NEWLINE]
 * 
 *                              PROGRAM
 *                            /         \
 *                         LABEL        NULL
 *                           |
 *                         INSTR
 *                         /   \
 *                        REG  IMM 
 * 
 * newline encountered, go to depth 1 and add nodes and proceed from there
 * 
 * order of execution (parsing) of the tree will be counter-clockwise
 */
#include <iostream>
#include "Token.hpp"


void craftAST(){

}


int main(){

    return 0;
}