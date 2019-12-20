#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct ConditionFlags {
    uint8_t z:1;    // zero
    uint8_t s:1;    // sign
    uint8_t p:1;    // parity
    uint8_t cy:1;   // carry
    uint8_t ac:1;   // auxiliary carry
    uint8_t pad:3;  // padding; this makes the struct 8 bit long
};

struct State8080 {
    uint8_t *memory;
    uint8_t int_enable;
    struct ConditionFlags cf;
    uint8_t a;                  // accumulator
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t h;
    uint8_t l;
    uint16_t sp;                // stack pointer
    uint16_t pc;                // program counter
};  // sizeof is 24 (2 bytes of padding)

void unassigned_instruction(struct State8080 *state){
    // Needs to undo pc as it has already advanced
    puts("Error: unassigned instruction");
    exit(EXIT_FAILURE);
}

int emulate_8080(struct State8080 *state){
    
}

int main(int argc, char **argv)
{
    size_t size1 = sizeof(struct ConditionFlags);
    size_t size2 = sizeof(struct State8080);
    printf("uint8_t pointer size: %zu\n", sizeof(uint8_t *));
    printf("Size of struct: %zu\n", size1);
    printf("Size of state: %zu\n", size2);
    return 0;
}