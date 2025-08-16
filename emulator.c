#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

int haltEncountered = false;

// Define the 8085's registers
unsigned char A, B, C, D, E, H, L, F;
unsigned short PC, SP;

// Define the 8085's memory
unsigned char memory[65536];

// Flag bit positions in the F register
#define CARRY_FLAG 0x01
#define AUX_CARRY_FLAG 0x10
#define PARITY_FLAG 0x04
#define ZERO_FLAG 0x40
#define SIGN_FLAG 0x80

// Macro to set/clear flag bits
#define SET_FLAG(f) (F |= (f))
#define CLEAR_FLAG(f) (F &= ~(f))

// Macro to check the parity of a byte
#define CHECK_PARITY(x) (__builtin_parity(x) == 0)

// Function to update flags based on the result of an operation
void update_flags(unsigned char result)
{
    if (result == 0)
    {
        SET_FLAG(ZERO_FLAG);
    }
    else
    {
        CLEAR_FLAG(ZERO_FLAG);
    }

    if (result & 0x80)
    {
        SET_FLAG(SIGN_FLAG);
    }
    else
    {
        CLEAR_FLAG(SIGN_FLAG);
    }

    if (CHECK_PARITY(result))
    {
        SET_FLAG(PARITY_FLAG);
    }
    else
    {
        CLEAR_FLAG(PARITY_FLAG);
    }
}

// Function to update flags for subtraction instructions
void update_flags_subtraction(unsigned char original_A, unsigned char value){
    unsigned char result = original_A - value;

    if (result == 0) {
        SET_FLAG(ZERO_FLAG);
    } else {
        CLEAR_FLAG(ZERO_FLAG);
    }

    if (result & 0x80) {
        SET_FLAG(SIGN_FLAG);
    } else {
        CLEAR_FLAG(SIGN_FLAG);
    }

    if (CHECK_PARITY(result)) {
        SET_FLAG(PARITY_FLAG);
    } else {
        CLEAR_FLAG(PARITY_FLAG);
    }

    if (original_A < value) {
        SET_FLAG(CARRY_FLAG);
    } else {
        CLEAR_FLAG(CARRY_FLAG);
    }

    if ((original_A & 0x0F) < (value & 0x0F)) {
        SET_FLAG(AUX_CARRY_FLAG);
    } else {
        CLEAR_FLAG(AUX_CARRY_FLAG);
    }
}

// Function to print the state of the registers and memory with spacing and instruction count
void print_state(int instruction_count)
{
    printf("\nInstruction %03d:\n", instruction_count);
    printf("____________________\n");
    printf("| Register | Value |\n");
    printf("|----------|-------|\n");
    printf("| A        | %02X    |\n", A);
    printf("| B        | %02X    |\n", B);
    printf("| C        | %02X    |\n", C);
    printf("| D        | %02X    |\n", D);
    printf("| E        | %02X    |\n", E);
    printf("| H        | %02X    |\n", H);
    printf("| L        | %02X    |\n", L);
    printf("| F        | %02X    |\n", F);
    printf("| PC       | %04X  |\n", PC);
    printf("| SP       | %04X  |\n", SP);
    printf("--------------------\n");
    printf("____________________\n");
    printf("| Memory   | Value |\n");
    printf("|----------|-------|\n");
    for (int i = 0; i < 16; i++)
    {
        printf("| %04X     | %02X    |\n", i, memory[i]);
    }
    printf("____________________\n");
}

// Function to emulate an instruction and print its state
void emulate_instruction(int *instruction_count)
{
    unsigned char opcode = memory[PC];
    PC++;

    unsigned short address;
    unsigned char data;

    printf("Executing opcode: %02X\n", opcode);

    switch (opcode)
    {
    case 0x00: // NOP
        printf("NOP\n");
        break;
    
    case 0x06: // MVI B, data
        B = memory[PC++];
        printf("MVI B, %02X\n", B);
        break;
    case 0x0E: // MVI C, data
        C = memory[PC++];
        printf("MVI C, %02X\n", C);
        break;
    case 0x16: // MVI D, data
        D = memory[PC++];
        printf("MVI D, %02X\n", D);
        break;
    case 0x1E: // MVI E, data
        E = memory[PC++];
        printf("MVI E, %02X\n", E);
        break;
    case 0x26: // MVI H, data
        H = memory[PC++];
        printf("MVI H, %02X\n", H);
        break;
    case 0x2E: // MVI L, data
        L = memory[PC++];
        printf("MVI L, %02X\n", L);
        break;
    case 0x3E: // MVI A, data
        A = memory[PC++];
        printf("MVI A, %02X\n", A);
        break;
    
    case 0x80: // ADD B
        A += B;
        update_flags(A);
        printf("ADD B\n");
        break;
    case 0x81: // ADD C
        A += C;
        update_flags(A);
        printf("ADD C\n");
        break;
    case 0x86: // ADD M
        A += memory[(H << 8) | L];
        update_flags(A);
        printf("ADD M\n");
        break;
    case 0xC6: // ADI data
        A += memory[PC++];
        update_flags(A);
        printf("ADI %02X\n", A);
        break;
    case 0x87: // ADD A
        A += A;
        update_flags(A);
        printf("ADD A\n");
        break;
    
    case 0x7F: // MOV A, A
        A = A;
        printf("MOV A, A\n");
        break;
    case 0x78: // MOV A, B
        A = B;
        printf("MOV A, B\n");
        break;
    case 0x79: // MOV A, C
        A = C;
        printf("MOV A, C\n");
        break;
    case 0x7A: // MOV A, D
        A = D;
        printf("MOV A, D\n");
        break;
    case 0x7B: // MOV A, E
        A = E;
        printf("MOV A, E\n");
        break;
    case 0x7C: // MOV A, H
        A = H;
        printf("MOV A, H\n");
        break;
    case 0x7D: // MOV A, L
        A = L;
        printf("MOV A, L\n");
        break;
    case 0x7E: // MOV A, M
        A = memory[(H << 8) | L];
        printf("MOV A, M\n");
        break;

    case 0x90: // SUB B
        A -= B;
        update_flags_subtraction(A, B);
        printf("SUB B\n");
        break;
    case 0x91: // SUB C
        A -= C;
        update_flags_subtraction(A, C);
        printf("SUB C\n");
        break;
    case 0x92: // SUB D
        A -= D;
        update_flags_subtraction(A, D);
        printf("SUB D\n");
        break;
    case 0x93: // SUB E
        A -= E;
        update_flags_subtraction(A, E);
        printf("SUB E\n");
        break;
    case 0x94: // SUB H
        A -= H;
        update_flags_subtraction(A, H);
        printf("SUB H\n");
        break;
    case 0x95: // SUB L
        A -= L;
        update_flags_subtraction(A, L);
        printf("SUB L\n");
        break;
    case 0x96: // SUB M
        A -= memory[(H << 8) | L];
        update_flags_subtraction(A, memory[(H << 8) | L]);
        printf("SUB M\n");
        break;
    case 0x97: // SUB A
        A -= A;
        update_flags_subtraction(A, A);
        printf("SUB A\n");
        break;
    case 0xD6: // SUI data
        A -= memory[PC++];
        update_flags_subtraction(A, memory[PC]);
        printf("SUI %02X\n", A);
        break;

    case 0x01: // LXI B, data16
        C = memory[PC++];
        B = memory[PC++];
        printf("LXI B, %02X%02X\n", B, C);
        break;
    case 0x11: // LXI D, data16
        E = memory[PC++];
        D = memory[PC++];
        printf("LXI D, %02X%02X\n", D, E);
        break;
    case 0x21: // LXI H, data16
        L = memory[PC++];
        H = memory[PC++];
        printf("LXI H, %02X%02X\n", H, L);
        break;

    case 0x32: // STA addr
        address = memory[PC++];  // lower byte
        address |= memory[PC++] << 8;   // upper byte
        memory[address] = A;
        printf("STA %04X\n", address);
        break;

    case 0xA0: // ANA B
        printf("ANA B\n");
        A = A & B;
        update_flags(A);
        break;
    case 0xA1: // ANA C
        printf("ANA C\n");
        A = A & C;
        update_flags(A);
        break;
    case 0xA2: // ANA D
        printf("ANA D\n");
        A = A & D;
        update_flags(A);
        break;
    case 0xA3: // ANA E
        printf("ANA E\n");
        A = A & E;
        update_flags(A);
        break;
    case 0xA4: // ANA H
        printf("ANA H\n");
        A = A & H;
        update_flags(A);
        break;
    case 0xA5: // ANA L
        printf("ANA L\n");
        A = A & L;
        update_flags(A);
        break;
    case 0xA6: // ANA M
        printf("ANA M\n");
        A = A & memory[(H << 8) | L];
        update_flags(A);
        break;
    case 0xA7: // ANA A
        printf("ANA A\n");
        A = A & A;
        update_flags(A);
        break;

    case 0xA8: // XRA B
        printf("XRA B\n");
        A = A ^ B;
        update_flags(A);
        break;
    case 0xA9: // XRA C
        printf("XRA C\n");
        A = A ^ C;
        update_flags(A);
        break;
    case 0xAA: // XRA D
        printf("XRA D\n");
        A = A ^ D;
        update_flags(A);
        break;
    case 0xAB: // XRA E
        printf("XRA E\n");
        A = A ^ E;
        update_flags(A);
        break;
    case 0xAC: // XRA H
        printf("XRA H\n");
        A = A ^ H;
        update_flags(A);
        break;
        break;
    case 0xAD: // XRA L
        printf("XRA L\n");
        A = A ^ L;
        update_flags(A);
        break;
    case 0xAE: // XRA M
        printf("XRA M\n");
        A = A ^ memory[(H << 8) | L];
        update_flags(A);
        break;
    case 0xAF: // XRA A
        printf("XRA A\n");
        A = A ^ A;
        update_flags(A);
        break;

    case 0xB0: // ORA B
        printf("ORA B\n");
        A = A | B;
        update_flags(A);
        break;
    case 0xB1: // ORA C
        printf("ORA C\n");
        A = A | C;
        update_flags(A);
        break;
    case 0xB2: // ORA D
        printf("ORA D\n");
        A = A | D;
        update_flags(A);
        break;
    case 0xB3: // ORA E
        printf("ORA E\n");
        A = A | E;
        update_flags(A);
        break;
    case 0xB4: // ORA H
        printf("ORA H\n");
        A = A | H;
        update_flags(A);
        break;
    case 0xB5: // ORA L
        printf("ORA L\n");
        A = A | L;
        update_flags(A);
        break;
    case 0xB6: // ORA M
        printf("ORA M\n");
        A = A | memory[(H << 8) | L];
        update_flags(A);
        break;
    case 0xB7: // ORA A
        printf("ORA A\n");
        A = A | A;
        update_flags(A);
        break;

    case 0xB8: // CMP B
        update_flags_subtraction(A, B);
        printf("CMP B\n");
        break;
    case 0xB9: // CMP C
        update_flags_subtraction(A, C);
        printf("CMP C\n");
        break;
    case 0xBA: // CMP D
        update_flags_subtraction(A, D);
        printf("CMP D\n");
        break;
    case 0xBB: // CMP E
        update_flags_subtraction(A, E);
        printf("CMP E\n");
        break;
    case 0xBC: // CMP H
        update_flags_subtraction(A, H);
        printf("CMP H\n");
        break;
    case 0xBD: // CMP L
        update_flags_subtraction(A, L);
        printf("CMP L\n");
        break;
    case 0xBE: // CMP M
        update_flags_subtraction(A, memory[(H << 8) | L]);
        printf("CMP M\n");
        break;
    case 0xBF: // CMP A
        update_flags_subtraction(A, A);
        printf("CMP A\n");
        break;

    case 0x04: // INR B
        B++;
        update_flags(B);
        CLEAR_FLAG(CARRY_FLAG); // Carry flag is unaffected
        printf("INR B\n");
        break;
    case 0x0C: // INR C
        C++;
        update_flags(C);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR C\n");
        break;
    case 0x14: // INR D
        D++;
        update_flags(D);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR D\n");
        break;
    case 0x1C: // INR E
        E++;
        update_flags(E);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR E\n");
        break;
    case 0x24: // INR H
        H++;
        update_flags(H);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR H\n");
        break;
    case 0x2C: // INR L
        L++;
        update_flags(L);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR L\n");
        break;
    case 0x34: // INR M
        // Increment memory at address (H << 8 | L)
        memory[(H << 8) | L]++;
        update_flags(memory[(H << 8) | L]);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR M\n");
        break;
    case 0x3C: // INR A
        A++;
        update_flags(A);
        CLEAR_FLAG(CARRY_FLAG);
        printf("INR A\n");
        break;

    case 0x05: // DCR B
        B--;
        update_flags(B);
        if ((B & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR B\n");
        break;
    case 0x0D: // DCR C
        C--;
        update_flags(C);
        if ((C & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR C\n");
        break;
    case 0x15: // DCR D
        D--;
        update_flags(D);
        if ((D & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR D\n");
        break;
    case 0x1D: // DCR E
        E--;
        update_flags(E);
        if ((E & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR E\n");
        break;
    case 0x25: // DCR H
        H--;
        update_flags(H);
        if ((H & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR H\n");
        break;
    case 0x2D: // DCR L
        L--;
        update_flags(L);
        if ((L & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR L\n");
        break;
    case 0x3D: // DCR A
        A--;
        update_flags(A);
        if ((A & 0x0F) == 0x0F) { // Borrow from lower nibble
        SET_FLAG(AUX_CARRY_FLAG);
        } else {
            CLEAR_FLAG(AUX_CARRY_FLAG);
        }
        printf("DCR A\n");
        break;

    case 0x76: // HLT
        printf("HLT encountered. Exiting.\n");
        haltEncountered = true;
        break;
    default:
        printf("Unimplemented opcode: %02X\n", opcode);
        break;
    }

    (*instruction_count)++;
    print_state(*instruction_count);
}

int main(int argc, char** argv)
{
    if(argc == 2){
        if(strcmp(argv[1], "--help") == 0){
            printf("This is a 8085 uP emulator written in C.\n");
            printf("The complete instruction set has not been implemented yet.\n");
            printf("Just run the program and begin typing in the instructions.\n");
            printf("Separate each instruction by pressing an Enter key.\n");
            printf("Once all done, enter the instruction \'HLT\' to terminate the input stream.\n") ;
        }
        else{
            printf("Incorrect flag. Valid flag \'--help\'\n");
        }
        return 0;
    }
    // Initialize the 8085's registers and memory
    A = B = C = D = E = H = L = F = 0;
    PC = 0x0000; // Program counter starts at 0
    SP = 0xFFFF; // Stack pointer starts at top of memory

    // User input for the program
    printf("Enter 8085 assembly instructions (end with 'HLT'):\n");

    char input[256];
    int address = 0;
    while (1)
    {
        fgets(input, sizeof(input), stdin);
        if (strncmp(input, "HLT", 3) == 0)
        {
            memory[address++] = 0x76; // HLT opcode
            break;
        }
        else if (strncmp(input, "MVI B,", 6) == 0)
        {
            memory[address++] = 0x06; // MVI B opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI C,", 6) == 0)
        {
            memory[address++] = 0x0E; // MVI C opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI D,", 6) == 0)
        {
            memory[address++] = 0x16; // MVI D opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI E,", 6) == 0)
        {
            memory[address++] = 0x1E; // MVI E opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI H,", 6) == 0)
        {
            memory[address++] = 0x26; // MVI H opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI L,", 6) == 0)
        {
            memory[address++] = 0x2E; // MVI L opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "MVI A,", 6) == 0)
        {
            memory[address++] = 0x3E; // MVI A opcode
            memory[address++] = (unsigned char)strtol(input + 6, NULL, 16);
        }
        else if (strncmp(input, "ADD B", 5) == 0)
        {
            memory[address++] = 0x80; // ADD B opcode
        }
        else if (strncmp(input, "ADD C", 5) == 0)
        {
            memory[address++] = 0x81; // ADD C opcode
        }
        else if (strncmp(input, "ADD M", 5) == 0)
        {
            memory[address++] = 0x86; // ADD M opcode
        }
        else if (strncmp(input, "ADD A", 5) == 0)
        {
            memory[address++] = 0x87; // ADD M opcode
        }
        else if (strncmp(input, "ADI ", 4) == 0)
        {
            memory[address++] = 0xC6; // ADI opcode
            memory[address++] = (unsigned char)strtol(input + 4, NULL, 16);
        }
        else if (strncmp(input, "MOV A, A", 8) == 0)
        {
            memory[address++] = 0x7F; // MOV A, A opcode
        }
        else if (strncmp(input, "MOV A, B", 8) == 0)
        {
            memory[address++] = 0x78; // MOV A, B opcode
        }
        else if (strncmp(input, "MOV A, C", 8) == 0)
        {
            memory[address++] = 0x79; // MOV A, C opcode
        }
        else if (strncmp(input, "MOV A, D", 8) == 0)
        {
            memory[address++] = 0x7A; // MOV A, D opcode
        }
        else if (strncmp(input, "MOV A, E", 8) == 0)
        {
            memory[address++] = 0x7B; // MOV A, E opcode
        }
        else if (strncmp(input, "MOV A, H", 8) == 0)
        {
            memory[address++] = 0x7C; // MOV A, H opcode
        }
        else if (strncmp(input, "MOV A, L", 8) == 0)
        {
            memory[address++] = 0x7D; // MOV A, L opcode
        }
        else if (strncmp(input, "MOV A, M", 8) == 0)
        {
            memory[address++] = 0x7E; // MOV A, M opcode
        }
        else if (strncmp(input, "SUB B", 5) == 0)
        {
            memory[address++] = 0x90; // SUB B opcode
        }
        else if (strncmp(input, "SUB C", 5) == 0)
        {
            memory[address++] = 0x91; // SUB C opcode
        }
        else if (strncmp(input, "SUB D", 5) == 0)
        {
            memory[address++] = 0x92; // SUB D opcode
        }
        else if (strncmp(input, "SUB E", 5) == 0)
        {
            memory[address++] = 0x93; // SUB E opcode
        }
        else if (strncmp(input, "SUB H", 5) == 0)
        {
            memory[address++] = 0x94; // SUB H opcode
        }
        else if (strncmp(input, "SUB L", 5) == 0)
        {
            memory[address++] = 0x95; // SUB L opcode
        }
        else if (strncmp(input, "SUB A", 5) == 0)
        {
            memory[address++] = 0x97; // SUB M opcode
        }
        else if (strncmp(input, "SUI ", 4) == 0)
        {
            memory[address++] = 0xD6; // SUI opcode
            memory[address++] = (unsigned char)strtol(input + 4, NULL, 16);
        }
        else if (strncmp(input, "ANA B", 5) == 0)
        {
            memory[address++] = 0xA0; // AND B opcode
        }
        else if (strncmp(input, "ANA C", 5) == 0)
        {
            memory[address++] = 0xA1; // AND C opcode
        }
        else if (strncmp(input, "ANA D", 5) == 0)
        {
            memory[address++] = 0xA2; // AND D opcode
        }
        else if (strncmp(input, "ANA E", 5) == 0)
        {
            memory[address++] = 0xA3; // AND E opcode
        }
        else if (strncmp(input, "ANA H", 5) == 0)
        {
            memory[address++] = 0xA4; // AND H opcode
        }
        else if (strncmp(input, "ANA L", 5) == 0)
        {
            memory[address++] = 0xA5; // AND L opcode
        }
        else if (strncmp(input, "ANA A", 5) == 0)
        {
            memory[address++] = 0xA7; // AND M opcode
        }
        else if (strncmp(input, "XRA B", 5) == 0)
        {
            memory[address++] = 0xA8; // XOR B opcode
        }
        else if (strncmp(input, "XRA C", 5) == 0)
        {
            memory[address++] = 0xA9; // XOR C opcode
        }
        else if (strncmp(input, "XRA D", 5) == 0)
        {
            memory[address++] = 0xAA; // XOR D opcode
        }
        else if (strncmp(input, "XRA E", 5) == 0)
        {
            memory[address++] = 0xAB; // XOR E opcode
        }
        else if (strncmp(input, "XRA H", 5) == 0)
        {
            memory[address++] = 0xAC; // XOR H opcode
        }
        else if (strncmp(input, "XRA L", 5) == 0)
        {
            memory[address++] = 0xAD; // XOR L opcode
        }
        else if (strncmp(input, "XRA A", 5) == 0)
        {
            memory[address++] = 0xAF; // XOR M opcode
        }
        else if (strncmp(input, "ORA B", 5) == 0)
        {
            memory[address++] = 0xB0; // OR B opcode
        }
        else if (strncmp(input, "ORA C", 5) == 0)
        {
            memory[address++] = 0xB1; // OR C opcode
        }
        else if (strncmp(input, "ORA D", 5) == 0)
        {
            memory[address++] = 0xB2; // OR D opcode
        }
        else if (strncmp(input, "ORA E", 5) == 0)
        {
            memory[address++] = 0xB3; // OR E opcode
        }
        else if (strncmp(input, "ORA H", 5) == 0)
        {
            memory[address++] = 0xB4; // OR H opcode
        }
        else if (strncmp(input, "ORA L", 5) == 0)
        {
            memory[address++] = 0xB5; // OR L opcode
        }
        else if (strncmp(input, "ORA A", 5) == 0)
        {
            memory[address++] = 0xB7; // OR B opcode
        }
        else if (strncmp(input, "CMP B", 5) == 0)
        {
            memory[address++] = 0xB8; // OR B opcode
        }
        else if (strncmp(input, "CMP C", 5) == 0)
        {
            memory[address++] = 0xB9; // OR C opcode
        }
        else if (strncmp(input, "CMP D", 5) == 0)
        {
            memory[address++] = 0xBA; // OR D opcode
        }
        else if (strncmp(input, "CMP E", 5) == 0)
        {
            memory[address++] = 0xBB; // OR E opcode
        }
        else if (strncmp(input, "CMP H", 5) == 0)
        {
            memory[address++] = 0xBC; // OR H opcode
        }
        else if (strncmp(input, "CMP L", 5) == 0)
        {
            memory[address++] = 0xBD; // OR L opcode
        }
        else if (strncmp(input, "CMP A", 5) == 0)
        {
            memory[address++] = 0xBF; // OR M opcode
        }
        else if (strncmp(input, "INR B", 5) == 0)
        {
            memory[address++] = 0x04; // INR B opcode
        }
        else if (strncmp(input, "INR C", 5) == 0)
        {
            memory[address++] = 0x0C; // INR C opcode
        }
        else if (strncmp(input, "INR D", 5) == 0)
        {
            memory[address++] = 0x14; // INR D opcode
        }
        else if (strncmp(input, "INR E", 5) == 0)
        {
            memory[address++] = 0x1C; // INR E opcode
        }
        else if (strncmp(input, "INR H", 5) == 0)
        {
            memory[address++] = 0x24; // INR H opcode
        }
        else if (strncmp(input, "INR L", 5) == 0)
        {
            memory[address++] = 0x2C; // INR L opcode
        }
        else if (strncmp(input, "INR A", 5) == 0)
        {
            memory[address++] = 0x3C; // INR A opcode
        }
        else if (strncmp(input, "DCR B", 5) == 0)
        {
            memory[address++] = 0x05; // DCR B opcode
        }
        else if (strncmp(input, "DCR C", 5) == 0)
        {
            memory[address++] = 0x0D; // DCR C opcode
        }
        else if (strncmp(input, "DCR D", 5) == 0)
        {
            memory[address++] = 0x15; // DCR D opcode
        }
        else if (strncmp(input, "DCR E", 5) == 0)
        {
            memory[address++] = 0x1D; // DCR E opcode
        }
        else if (strncmp(input, "DCR H", 5) == 0)
        {
            memory[address++] = 0x25; // DCR H opcode
        }
        else if (strncmp(input, "DCR L", 5) == 0)
        {
            memory[address++] = 0x2D; // DCR L opcode
        }
        else if (strncmp(input, "DCR A", 5) == 0)
        {
            memory[address++] = 0x3D; // DCR A opcode
        }
        else if (strncmp(input, "LXI B,", 6) == 0)
        {
            memory[address++] = 0x01; // LXI B opcode
            unsigned short data16 = (unsigned short)strtol(input + 6, NULL, 16);
            memory[address++] = data16 & 0xFF;        // Low-order byte
            memory[address++] = (data16 >> 8) & 0xFF; // High-order byte
        }
        else if (strncmp(input, "LXI D,", 6) == 0)
        {
            memory[address++] = 0x11; // LXI D opcode
            unsigned short data16 = (unsigned short)strtol(input + 6, NULL, 16);
            memory[address++] = data16 & 0xFF;        // Low-order byte
            memory[address++] = (data16 >> 8) & 0xFF; // High-order byte
        }
        else if (strncmp(input, "LXI H,", 6) == 0)
        {
            memory[address++] = 0x21; // LXI H opcode
            unsigned short data16 = (unsigned short)strtol(input + 6, NULL, 16);
            memory[address++] = data16 & 0xFF;        // Low-order byte
            memory[address++] = (data16 >> 8) & 0xFF; // High-order byte
        }
        else if (strncmp(input, "STA ", 4) == 0)
        {
            memory[address++] = 0x32; // STA opcode
            unsigned short addr = (unsigned short)strtol(input + 4, NULL, 16);
            memory[address++] = addr & 0xFF;        // Low-order byte
            memory[address++] = (addr >> 8) & 0xFF; // High-order byte
        }
        else
        {
            printf("Unknown instruction: %s\n", input);
        }
    }

    int instruction_count = 0;
    while (!haltEncountered)
    {
        emulate_instruction(&instruction_count);
        usleep(500000);
    }

    return 0;
}
