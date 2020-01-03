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
	return (byte2 << 8) | byte1;
}

uint8_t byte_parity(uint8_t byte)
{
	uint8_t count = 0;
	uint8_t sbyte = byte;
	while( sbyte )
	{
		if((sbyte & 0x1) == 1)
		{
			count++;
		}
		sbyte = sbyte >> 1;
	}
	return count;
}

void add_8080(struct State8080 *state, uint8_t reg, uint8_t carry)
{
	// do the math with higher precision so we can capture the carry out
	uint16_t result = (uint16_t)state->a + (uint16_t)reg;
	uint8_t auxiliary = ((state->a) & 0xf) + (reg & 0xf);
	if ( carry )
	{
		result += state->cf.cy;
		auxiliary += state->cf.cy;
	}
	// relational operators are not garanteed to return 1 (?)
	// does not matter since the flags are bit sized
	state->cf.z = ((result & 0xff) == 0);
	state->cf.cy = (result > 0xff);
	state->cf.s = ((result & 0x80) == 0);
	state->cf.p = byte_parity( result & 0xff );
	state->cf.ac = (auxiliary > 0xf);
	state->a = result & 0xff;
}

void sub_8080(struct State8080 *state, uint8_t reg, uint8_t carry)
{
	// do the math with higher precision so we can capture the carry out
	uint16_t result = (uint16_t)state->a - (uint16_t)reg;
	uint8_t auxiliary = ((state->a) & 0xf) - (reg & 0xf);
	if ( carry )
	{
		result -= state->cf.cy;
		auxiliary -= state->cf.cy;
	}
	// relational operators are not garanteed to return 1 (?)
	// does not matter since the flags are bit sized
	state->cf.z = ((result & 0xff) == 0);
	state->cf.cy = (result > 0xff);
	state->cf.s = ((result & 0x80) == 0);
	state->cf.p = byte_parity( result & 0xff );
	state->cf.ac = (auxiliary > 0xf);
	state->a = result & 0xff;
}

void inr_8080(struct State8080 *state, uint8_t *reg)
{
	uint8_t res = *reg;
	res += 1;
	state->cf.z = (res == 0);
	state->cf.s = ((res & 0x80) == 0);
	state->cf.p = byte_parity( res );
	state->cf.ac = ((*reg & 0xf) == 0xf);
	*reg = res;
}

void dcr_8080(struct State8080 *state, uint8_t *reg)
{
	uint8_t res = *reg;
	res -= 1;
	state->cf.z = (res == 0);
	state->cf.s = ((res & 0x80) == 0);
	state->cf.p = byte_parity( res );
	state->cf.ac = ((*reg & 0xf) == 0);
	*reg = res;
}

void inx_8080(struct State8080 *state, uint8_t pair)
{
	uint16_t pair_mem;
	switch ( pair )
	{
		case 0x0:
			pair_mem = combine_two_8bit(state->c, state->b);
			pair_mem++;
			state->c = pair_mem & 0xff;
			state->b = (pair_mem >> 8) & 0xff;
			break;
		case 0x1:
			pair_mem = combine_two_8bit(state->e, state->d);
			pair_mem++;
			state->e = pair_mem & 0xff;
			state->d = (pair_mem >> 8) & 0xff;
			break;
		case 0x2:
			pair_mem = combine_two_8bit(state->l, state->h);
			pair_mem++;
			state->l = pair_mem & 0xff;
			state->h = (pair_mem >> 8) & 0xff;
			break;
		case 0x3:
			state->sp++;
			break;
		default:
			printf("$Failed\n");
			break;
	}
}

void dcx_8080(struct State8080 *state, uint8_t pair)
{
	uint16_t pair_mem;
	switch ( pair )
	{
		case 0x0:
			pair_mem = combine_two_8bit(state->c, state->b);
			pair_mem--;
			state->c = pair_mem & 0xff;
			state->b = (pair_mem >> 8) & 0xff;
			break;
		case 0x1:
			pair_mem = combine_two_8bit(state->e, state->d);
			pair_mem--;
			state->e = pair_mem & 0xff;
			state->d = (pair_mem >> 8) & 0xff;
			break;
		case 0x2:
			pair_mem = combine_two_8bit(state->l, state->h);
			pair_mem--;
			state->h = pair_mem & 0xff;
			state->l = (pair_mem >> 8) & 0xff;
			break;
		case 0x3:
			state->sp--;
			break;
		default:
			printf("$Failed\n");
			break;
	}
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
			add_8080(state, state->b, 0);
			break;
		case 0x81:
			add_8080(state, state->c, 0);
			break;
		case 0x82:
			add_8080(state, state->d, 0);
			break;
		case 0x83:
			add_8080(state, state->e, 0);
			break;
		case 0x84:
			add_8080(state, state->h, 0);
			break;
		case 0x85:
			add_8080(state, state->l, 0);
			break;
		case 0x87:
			add_8080(state, state->a, 0);
			break;

		// ADD M
		case 0x86:
			add_8080(state, state_mem[combine_two_8bit(state->l, state->h)], 0);
			break;

		// ADI data
		case 0xc6:
			add_8080(state, opcode[1], 0);
			break;

		// ADC r
		case 0x88:
			add_8080(state, state->b, 1);
			break;
		case 0x89:
			add_8080(state, state->c, 1);
			break;
		case 0x8a:
			add_8080(state, state->d, 1);
			break;
		case 0x8b:
			add_8080(state, state->e, 1);
			break;
		case 0x8c:
			add_8080(state, state->h, 1);
			break;
		case 0x8d:
			add_8080(state, state->l, 1);
			break;
		case 0x8f:
			add_8080(state, state->a, 1);
			break;

		// ADC M
		case 0x8e:
			add_8080(state, state_mem[combine_two_8bit(state->l, state->h)], 1);
			break;

		// ACI data
		case 0xce:
			add_8080(state, opcode[1], 1);
			break;

		// SUB r
		case 0x90:
			sub_8080(state, state->b, 0);
			break;
		case 0x91:
			sub_8080(state, state->c, 0);
			break;
		case 0x92:
			sub_8080(state, state->d, 0);
			break;
		case 0x93:
			sub_8080(state, state->e, 0);
			break;
		case 0x94:
			sub_8080(state, state->h, 0);
			break;
		case 0x95:
			sub_8080(state, state->l, 0);
			break;
		case 0x97:
			sub_8080(state, state->a, 0);
			break;

		// SUB M
		case 0x96:
			sub_8080(state, state_mem[combine_two_8bit(state->l, state->h)], 0);
			break;

		// SUI data
		case 0xd6:
			sub_8080(state, opcode[1], 0);
			break;

		// SBB r
		case 0x98:
			sub_8080(state, state->b, 1);
			break;
		case 0x99:
			sub_8080(state, state->c, 1);
			break;
		case 0x9a:
			sub_8080(state, state->d, 1);
			break;
		case 0x9b:
			sub_8080(state, state->e, 1);
			break;
		case 0x9c:
			sub_8080(state, state->h, 1);
			break;
		case 0x9d:
			sub_8080(state, state->l, 1);
			break;
		case 0x9f:
			sub_8080(state, state->a, 1);
			break;

		// SBB M
		case 0x9e:
			sub_8080(state, state_mem[combine_two_8bit(state->l, state->h)], 1);
			break;

		// SBI data
		case 0xde:
			sub_8080(state, opcode[1], 1);
			break;

		// INR r
		case 0x04:
			inr_8080(state, &state->b);
			break;
		case 0x0c:
			inr_8080(state, &state->c);
			break;
		case 0x14:
			inr_8080(state, &state->d);
			break;
		case 0x1c:
			inr_8080(state, &state->e);
			break;
		case 0x24:
			inr_8080(state, &state->h);
			break;
		case 0x2c:
			inr_8080(state, &state->l);
			break;
		case 0x3c:
			inr_8080(state, &state->a);
			break;

		// INR M
		case 0x34:
			inr_8080(state, &state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// DCR r
		case 0x05:
			dcr_8080(state, &state->b);
			break;
		case 0x0d:
			dcr_8080(state, &state->c);
			break;
		case 0x15:
			dcr_8080(state, &state->d);
			break;
		case 0x1d:
			dcr_8080(state, &state->e);
			break;
		case 0x25:
			dcr_8080(state, &state->h);
			break;
		case 0x2d:
			dcr_8080(state, &state->l);
			break;
		case 0x3d:
			dcr_8080(state, &state->a);
			break;

		// DCR M
		case 0x35:
			dcr_8080(state, &state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// INX r
		case 0x03:
			inx_8080(state, 0);
			break;
		case 0x13:
			inx_8080(state, 1);
			break;
		case 0x23:
			inx_8080(state, 2);
			break;
		case 0x33:
			inx_8080(state, 3);
			break;

		// DCX r
		case 0x0b:
			dcx_8080(state, 0);
			break;
		case 0x1b:
			dcx_8080(state, 1);
			break;
		case 0x2b:
			dcx_8080(state, 2);
			break;
		case 0x3b:
			dcx_8080(state, 3);
			break;

		default:
			break;
	}
	state->pc++;
	return 0;
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
