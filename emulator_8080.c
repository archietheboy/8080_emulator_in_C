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

void unassigned_instruction(struct State8080 *state)
{
	// Needs to undo pc as it has already advanced
	puts("Error: unassigned instruction");
	exit(EXIT_FAILURE);
}

uint8_t combine_two_8bit(uint8_t byte1, uint8_t byte2)
{
	return (byte2 << 8) | (byte1 & 0xff);
}

int emulate_8080(struct State8080 *state)
{
	uint8_t *state_mem = state->memory;
	unsigned char *opcode = &state_mem[state->pc];	// '->' has higher precedence than '&'

	switch (*opcode)
	{
		// NOP
		case 0x00:
			break;

		// LXI rp,data16
		case 0x01:
			state->b = opcode[2];
			state->c = opcode[1];
			state->pc+=2;			// advance PC to the end of instruction
			break;
		case 0x11:
			state->d = opcode[2];
			state->e = opcode[1];
			state->pc+=2;
			break;
		case 0x21:
			state->h = opcode[2];
			state->l = opcode[1];
			state->pc+=2;
		case 0x31:
			state->sp = combine_two_8bit(opcode[1], opcode[2]);
			state->pc+=2;
			break;
		
		// MOV r1,r2
		case 0x40:
			break;
		case 0x41:
			state->b = state->c;
			break;
		case 0x42:
			state->b = state->d;
			break;
		case 0x43:
			state->b = state->e;
			break;
		case 0x44:
			state->b = state->h;
			break;
		case 0x45:
			state->b = state->l;
			break;
		case 0x47:
			state->b = state->a;
		   break;
		case 0x48:
			state->c = state->b;
	 		break;
		case 0x49:
			break;
		case 0x4a:
			state->c = state->d;
			break;
		case 0x4b:
			state->c = state->e;
			break;
		case 0x4c:
			state->c = state->h;
			break;
		case 0x4d:
			state->c = state->l;
			break;
		case 0x4f:
			state->c = state->a;
			break;
		case 0x50:
			state->d = state->b;
			break;
		case 0x51:
			state->d = state->c;
			break;
		case 0x52:
			break;
		case 0x53:
			state->d = state->e;
			break;
		case 0x54:
			state->d = state->h;
			break;
		case 0x55:
			state->d = state->l;
		case 0x57:
			state->d = state->a;
			break;
		case 0x58:
			state->e = state->b;
			break;
		case 0x59:
			state->e = state->c;
			break;
		case 0x5a:
			state->e = state->d;
			break;
		case 0x5b:
			break;
		case 0x5c:
			state->e = state->h;
			break;
		case 0x5d:
			state->e = state->l;
			break;
		case 0x5f:
			state->e = state->a;
			break;
		case 0x60:
			state->h = state->b;
			break;
		case 0x61:
			state->h = state->c;
			break;
		case 0x62:
			state->h = state->d;
			break;
		case 0x63:
			state->h = state->e;
			break;
		case 0x64:
			break;
		case 0x65:
			state->h = state->l;
			break;
		case 0x67:
			state->h = state->a;
			break;
		case 0x68:
			state->l = state->b;
			break;
		case 0x69:
			state->l = state->c;
			break;
		case 0x6a:
			state->l = state->d;
			break;
		case 0x6b:
			state->l = state->e;
			break;
		case 0x6c:
			state->l = state->h;
			break;
		case 0x6d:
			break;
		case 0x6f:
			state->l = state->a;
			break;
		case 0x78:
			state->a = state->b;
			break;
		case 0x79:
			state->a = state->c;
			break;
		case 0x7a:
			state->a = state->d;
			break;
		case 0x7b:
			state->a = state->e;
			break;
		case 0x7c:
			state->a = state->h;
			break;
		case 0x7d:
			state->a = state->l;
			break;
		case 0x7f:
			break;
		
		// MOV r,M
		case 0x46:
			state->b = state_mem[combine_two_8bit(state->l, state->h)];
			break;	
		case 0x4e:
			state->c = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		case 0x56:
			state->d = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		case 0x5e:
			state->e = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		case 0x66:
			state->h = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		case 0x6e:
			state->l = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		case 0x76:
			break;
		case 0x7e:
			state->a = state_mem[combine_two_8bit(state->l, state->h)];
			break;
		
		// MOV M,r
		case 0x70:
			state_mem[combine_two_8bit(state->l, state->h)] = state->b;
			break;
		case 0x71:
			state_mem[combine_two_8bit(state->l, state->h)] = state->c;
			break;
		case 0x72:
			state_mem[combine_two_8bit(state->l, state->h)] = state->d;
			break;
		case 0x73:
			state_mem[combine_two_8bit(state->l, state->h)] = state->e;
			break;
		case 0x74:
			state_mem[combine_two_8bit(state->l, state->h)] = state->h;
			break;
		case 0x75:
			state_mem[combine_two_8bit(state->l, state->h)] = state->l;
			break;
		case 0x77:
			state_mem[combine_two_8bit(state->l, state->h)] = state->a;
			break;

		// MVI r,data
		case 0x06:
			state->b = opcode[1];
			break;
		case 0x0e:
			state->c = opcode[1];
			break;
		case 0x16:
			state->d = opcode[1];
			break;
		case 0x1e:
			state->e = opcode[1];
			break;
		case 0x26:
			state->h = opcode[1];
			break;
		case 0x2e:
			state->l = opcode[1];
			break;
		case 0x3e:
			state->a = opcode[1];
			break;

		// MVI M,data
		case 0x36:	
			state_mem[combine_two_8bit(state->l, state->h)] = opcode[1];
			break;
		
		// LDA addr
		case 0x3a:
			state->a = state_mem[combine_two_8bit(opcode[1], opcode[2])];
			break;
		
		// STA addr
		case 0x32:
			state_mem[combine_two_8bit(opcode[1], opcode[2])] = state->a;
			break;
		
		// LHLD addr
		case 0x2a:
			state->l = state_mem[combine_two_8bit(opcode[1], opcode[2])];
			state->h = state_mem[combine_two_8bit(opcode[1], opcode[2]) + 1];
			break;
		
		// SHLD addr
		case 0x22:
			state_mem[combine_two_8bit(opcode[1], opcode[2])] = state->l;
			state_mem[combine_two_8bit(opcode[1], opcode[2]) + 1] = state->h;
			break;

		// LDAX rp
		case 0x0a:
			state->a = state_mem[combine_two_8bit(state->c, state->b)];
			break;
		case 0x1a:
			state->a = state_mem[combine_two_8bit(state->e, state->d)];
			break;			

		// XCHG
		case 0xeb:
			state->h = state->d;
			state->l = state->e;

		// ADD r
		case 0x80:
			// do the math with higher precision so we can capture the carry out    
			uint16_t answer = 

		default:
			break;
	}
	state->pc++;
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
