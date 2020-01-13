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
	printf("Instruction: 0x%04x", state->pc);
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
	state->cf.s = ((result & 0x80) != 0);
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
	state->cf.s = ((result & 0x80) != 0);
	state->cf.p = byte_parity( result & 0xff );
	state->cf.ac = (auxiliary > 0xf);
	state->a = result & 0xff;
}

void inr_8080(struct State8080 *state, uint8_t *reg)
{
	uint8_t res = *reg;
	res += 1;
	state->cf.z = (res == 0);
	state->cf.s = res >> 7;
	state->cf.p = byte_parity( res );
	state->cf.ac = ((*reg & 0xf) == 0xf);
	*reg = res;
}

void dcr_8080(struct State8080 *state, uint8_t *reg)
{
	uint8_t res = *reg;
	res -= 1;
	state->cf.z = (res == 0);
	state->cf.s = res >> 7;
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

void dad_8080(struct State8080 *state, uint8_t pair)
{
	uint32_t res = (uint32_t)combine_two_8bit(state->l, state->h);
	switch ( pair )
	{
		case 0x0:
			res += (uint32_t)combine_two_8bit(state->c, state->b);
			break;
		case 0x1:
			res += (uint32_t)combine_two_8bit(state->e, state->d);
			break;
		case 0x2:
			res += (uint32_t)combine_two_8bit(state->l, state->h);
			break;
		case 0x3:
			res += (uint32_t)state->sp;
			break;
		default:
			printf("$Failed\n");
			break;
	}
	state->cf.cy = (res > 0xffff);
	state->l = (res & 0xff);
	state->h = (res >> 8) & 0xff;
}

void and_8080(struct State8080 *state, uint8_t reg)
{
	state->a &= reg;
	state->cf.cy = 0;
	state->cf.z = (state->a == 0);
	state->cf.s = state->a >> 7;
	state->cf.p = byte_parity(state->a);
}

void xor_8080(struct State8080 *state, uint8_t reg)
{
	state->a ^= reg;
	state->cf.cy = 0;
	state->cf.ac = 0;
	state->cf.z = (state->a == 0);
	state->cf.s = state->a >> 7;
	state->cf.p = byte_parity(state->a);
}

void or_8080(struct State8080 *state, uint8_t reg)
{
	state->a |= reg;
	state->cf.cy = 0;
	state->cf.ac = 0;
	state->cf.z = (state->a == 0);
	state->cf.s = state->a >> 7;
	state->cf.p = byte_parity(state->a);
}

void cmp_8080(struct State8080 *state, uint8_t reg)
{
	uint8_t tmp = state->a - reg;
	state->cf.cy = (state->a < reg);
	state->cf.z = (state->a == reg);
	state->cf.s = (tmp == 0);
	state->cf.p = byte_parity(tmp);
	state->cf.ac = ((state->a & 0xf) < (reg & 0xf));
}

void jmp_8080(struct State8080 *state, uint8_t byte1, uint8_t byte2, uint8_t cond)
{
	uint8_t test;
	switch (cond)
	{
		case 0: test = (state->cf.z == 0); break;
		case 1: test = state->cf.z; break;
		case 2: test = (state->cf.cy == 0); break;
		case 3: test = state->cf.cy; break;
		case 4: test = (state->cf.p == 0); break;
		case 5: test = state->cf.p; break;
		case 6: test = (state->cf.s == 0); break;
		case 7: test = state->cf.s; break;
		default: test = 1; break;
	}
	if (test)
	{
		state->pc = combine_two_8bit(byte1, byte2);
	}
	else
	{
		state->pc += 2;
	}
}

void call_8080(struct State8080 *state, uint8_t byte1, uint8_t byte2, uint8_t cond)
{
	uint8_t test;
	switch (cond)
	{
		case 0: test = (state->cf.z == 0); break;
		case 1: test = state->cf.z; break;
		case 2: test = (state->cf.cy == 0); break;
		case 3: test = state->cf.cy; break;
		case 4: test = (state->cf.p == 0); break;
		case 5: test = state->cf.p; break;
		case 6: test = (state->cf.s == 0); break;
		case 7: test = state->cf.s; break;
		default: test = 1; break;
	}
	if (test)
	{
		state->pc += 2; // ret
		state->memory[state->sp - 1] = state->pc >> 8;
		state->memory[state->sp - 2] = state->pc & 0xff;
		state->sp -= 2;
		state->pc = combine_two_8bit(byte1, byte2);
	}
	else
	{
		state->pc += 2;
	}
}

void ret_8080(struct State8080 *state, uint8_t cond)
{
	uint8_t test;
	switch (cond)
	{
		case 0: test = (state->cf.z == 0); break;
		case 1: test = state->cf.z; break;
		case 2: test = (state->cf.cy == 0); break;
		case 3: test = state->cf.cy; break;
		case 4: test = (state->cf.p == 0); break;
		case 5: test = state->cf.p; break;
		case 6: test = (state->cf.s == 0); break;
		case 7: test = state->cf.s; break;
		default: test = 1; break;
	}
	if (test)
	{
		state->pc = combine_two_8bit(state->memory[state->sp], state->memory[state->sp + 1]);
		state->sp += 2;
	}
}

void rst_8080(struct State8080 *state, uint8_t nnn)
{
	state->pc++;
	state->memory[state->sp - 1] = state->pc >> 8;
	state->memory[state->sp - 2] = state->pc & 0xff;
	state->sp -= 2;
	state->pc = (uint16_t)(nnn << 3);
}

int emulate_8080(struct State8080 *state)
{
	uint8_t *state_mem = state->memory;
	unsigned char *opcode = &state_mem[state->pc];	// '->' has higher precedence than '&'
	printf("opcode:\t0x%02x", *opcode);
	switch (*opcode)
	{
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
			state->pc++;
			break;
		case 0x0e:
			state->c = opcode[1];
			state->pc++;
			break;
		case 0x16:
			state->d = opcode[1];
			state->pc++;
			break;
		case 0x1e:
			state->e = opcode[1];
			state->pc++;
			break;
		case 0x26:
			state->h = opcode[1];
			state->pc++;
			break;
		case 0x2e:
			state->l = opcode[1];
			state->pc++;
			break;
		case 0x3e:
			state->a = opcode[1];
			state->pc++;
			break;

		// MVI M,data
		case 0x36:	
			state_mem[combine_two_8bit(state->l, state->h)] = opcode[1];
			state->pc++;
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

		// LDA addr
		case 0x3a:
			state->a = state_mem[combine_two_8bit(opcode[1], opcode[2])];
			state->pc += 2;
			break;

		// STA addr
		case 0x32:
			state_mem[combine_two_8bit(opcode[1], opcode[2])] = state->a;
			state->pc += 2;
			break;

		// LHLD addr
		case 0x2a:
			state->l = state_mem[combine_two_8bit(opcode[1], opcode[2])];
			state->h = state_mem[combine_two_8bit(opcode[1], opcode[2]) + 1];
			state->pc += 2;
			break;

		// SHLD addr
		case 0x22:
			state_mem[combine_two_8bit(opcode[1], opcode[2])] = state->l;
			state_mem[combine_two_8bit(opcode[1], opcode[2]) + 1] = state->h;
			state->pc += 2;
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
			{
				uint8_t tmp1, tmp2;
				tmp1 = state->h;
				tmp2 = state->l;
				state->h = state->d;
				state->l = state->e;
				state->d = tmp1;
				state->e = tmp2;
			}
			break;

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
			state->pc++;
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
			state->pc++;
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
			state->pc++;
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
			state->pc++;
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

		// DAD rp
		case 0x09:
			dad_8080(state, 0);
			break;
		case 0x19:
			dad_8080(state, 1);
			break;
		case 0x29:
			dad_8080(state, 2);
			break;
		case 0x39:
			dad_8080(state, 3);
			break;

		// DAA
		case 0x27:
			if ( ((state->a & 0xf) > 9) || state->cf.ac)
			{
				state->cf.ac = 1;
				state->a += 6;
			}
			else
			{
				state->cf.ac = 0;
			}
			
			if ( ((state->a  & 0xf0) > 0x90) || state->cf.cy)
			{
				state->cf.cy = 1;
				state->a = ((state->a & 0xf0) + 0x60) | (state->a & 0xf);
			}
			state->cf.s = state->a >> 7;
			state->cf.p = byte_parity( state->a );
			state->cf.z = (state->a == 0);
			break;

		// ANA r
		case 0xa0:
			and_8080(state, state->b);
			break;
		case 0xa1:
			and_8080(state, state->c);
			break;
		case 0xa2:
			and_8080(state, state->d);
			break;
		case 0xa3:
			and_8080(state, state->e);
			break;
		case 0xa4:
			and_8080(state, state->h);
			break;
		case 0xa5:
			and_8080(state, state->l);
			break;
		case 0xa7:
			and_8080(state, state->a);
			break;

		// ANA M
		case 0xa6:
			and_8080(state, state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// ANI data
		case 0xe6:
			and_8080(state, opcode[1]);
			state->cf.ac = 0;
			state->pc++;
			break;

		// XRA r
		case 0xa8:
			xor_8080(state, state->b);
			break;
		case 0xa9:
			xor_8080(state, state->c);
			break;
		case 0xaa:
			xor_8080(state, state->d);
			break;
		case 0xab:
			xor_8080(state, state->e);
			break;
		case 0xac:
			xor_8080(state, state->h);
			break;
		case 0xad:
			xor_8080(state, state->l);
			break;
		case 0xaf:
			xor_8080(state, state->a);
			break;

		// XRA M
		case 0xae:
			xor_8080(state, state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// XRI data
		case 0xee:
			xor_8080(state, opcode[1]);
			state->pc++;
			break;

		// ORA r
		case 0xb0:
			or_8080(state, state->b);
			break;
		case 0xb1:
			or_8080(state, state->c);
			break;
		case 0xb2:
			or_8080(state, state->d);
			break;
		case 0xb3:
			or_8080(state, state->e);
			break;
		case 0xb4:
			or_8080(state, state->h);
			break;
		case 0xb5:
			or_8080(state, state->l);
			break;
		case 0xb7:
			or_8080(state, state->a);
			break;

		// ORA M
		case 0xb6:
			or_8080(state, state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// ORI data
		case 0xf6:
			or_8080(state, opcode[1]);
			state->pc++;
			break;

		// CMP r
		case 0xb8:
			cmp_8080(state, state->b);
			break;
		case 0xb9:
			cmp_8080(state, state->c);
			break;
		case 0xba:
			cmp_8080(state, state->d);
			break;
		case 0xbb:
			cmp_8080(state, state->e);
			break;
		case 0xbc:
			cmp_8080(state, state->h);
			break;
		case 0xbd:
			cmp_8080(state, state->l);
			break;
		case 0xbf:
			cmp_8080(state, state->a);
			break;

		// CMP M
		case 0xbe:
			cmp_8080(state, state_mem[combine_two_8bit(state->l, state->h)]);
			break;

		// CPI data
		case 0xfe:
			cmp_8080(state, opcode[1]);
			state->pc++;
			break;

		// RLC
		case 0x07:
			state->cf.cy = state->a >> 7;
			state->a = (state->a << 1) | state->cf.cy;
			break;

		// RRC
		case 0x0f:
			state->cf.cy = state->a & 0x01;
			state->a = (state->cf.cy << 7) | (state->a >> 1);
			break;

		// RAL
		case 0x17:
			{
				uint8_t tmp = state->a >> 7;
				state->a = (state->a << 1) | state->cf.cy;
				state->cf.cy = tmp;
			}
			break;

		// RAR
		case 0x1f:
			{
				uint8_t tmp = state->a & 0x01;
				state->a = (state->cf.cy << 7) | (state->a >> 1);
				state->cf.cy = tmp;
			}
			break;

		// CMA
		case 0x2f:
			state->a = ~state->a;
			break;

		// CMC
		case 0x3f:
			state->cf.cy = ~state->cf.cy;
			break;

		// STC
		case 0x37:
			state->cf.cy = 1;
			break;

		// JMP addr
		case 0xc3:
			state->pc = combine_two_8bit(opcode[1], opcode[2]);
			break;

		// Jcondition addr
		case 0xc2:
			jmp_8080(state, opcode[1], opcode[2], 0);
			break;
		case 0xca:
			jmp_8080(state, opcode[1], opcode[2], 1);
			break;
		case 0xd2:
			jmp_8080(state, opcode[1], opcode[2], 2);
			break;
		case 0xda:
			jmp_8080(state, opcode[1], opcode[2], 3);
			break;
		case 0xe2:
			jmp_8080(state, opcode[1], opcode[2], 4);
			break;
		case 0xea:
			jmp_8080(state, opcode[1], opcode[2], 5);
			break;
		case 0xf2:
			jmp_8080(state, opcode[1], opcode[2], 6);
			break;
		case 0xfa:
			jmp_8080(state, opcode[1], opcode[2], 7);
			break;

		// CALL addr
		case 0xcd:
			call_8080(state, opcode[1], opcode[2], 8);
			break;

		// Ccondition addr
		case 0xc4:
			call_8080(state, opcode[1], opcode[2], 0);
			break;
		case 0xcc:
			call_8080(state, opcode[1], opcode[2], 1);
			break;
		case 0xd4:
			call_8080(state, opcode[1], opcode[2], 2);
			break;
		case 0xdc:
			call_8080(state, opcode[1], opcode[2], 3);
			break;
		case 0xe4:
			call_8080(state, opcode[1], opcode[2], 4);
			break;
		case 0xec:
			call_8080(state, opcode[1], opcode[2], 5);
			break;
		case 0xf4:
			call_8080(state, opcode[1], opcode[2], 6);
			break;
		case 0xfc:
			call_8080(state, opcode[1], opcode[2], 7);
			break;

		// RET addr
		case 0xc9:
			ret_8080(state, 8);
			break;

		// Rcondition addr
		case 0xc0:
			ret_8080(state, 0);
			break;
		case 0xc8:
			ret_8080(state, 1);
			break;
		case 0xd0:
			ret_8080(state, 2);
			break;
		case 0xd8:
			ret_8080(state, 3);
			break;
		case 0xe0:
			ret_8080(state, 4);
			break;
		case 0xe8:
			ret_8080(state, 5);
			break;
		case 0xf0:
			ret_8080(state, 6);
			break;
		case 0xf8:
			ret_8080(state, 7);
			break;

		// RST n
		case 0xc7:
			rst_8080(state, 0);
			break;
		case 0xcf:
			rst_8080(state, 1);
			break;
		case 0xd7:
			rst_8080(state, 2);
			break;
		case 0xdf:
			rst_8080(state, 3);
			break;
		case 0xe7:
			rst_8080(state, 4);
			break;
		case 0xef:
			rst_8080(state, 5);
			break;
		case 0xf7:
			rst_8080(state, 6);
			break;
		case 0xff:
			rst_8080(state, 7);
			break;

		// PCHL
		case 0xe9:
			state->pc = combine_two_8bit(state->l, state->h);
			break;

		// PUSH rp
		case 0xc5:
			state_mem[state->sp - 1] = state->b;
			state_mem[state->sp - 2] = state->c;
			state->sp -= 2;
			break;
		case 0xd5:
			state_mem[state->sp - 1] = state->d;
			state_mem[state->sp - 2] = state->e;
			state->sp -= 2;
			break;
		case 0xe5:
			state_mem[state->sp - 1] = state->h;
			state_mem[state->sp - 2] = state->l;
			state->sp -= 2;
			break;

		// PUSH PSW
		case 0xf5:
			state_mem[state->sp - 1] = state->a;
			state_mem[state->sp - 2] = (state->cf.s << 7) | (state->cf.z << 6) | (state->cf.ac << 4) | (state->cf.p << 2) | 0x2 | state->cf.cy;
			state->sp -= 2;
			break;

		// POP rp
		case 0xc1:
			state->c = state_mem[state->sp];
			state->b = state_mem[state->sp + 1];
			state->sp += 2;
			break;
		case 0xd1:
			state->c = state_mem[state->sp];
			state->b = state_mem[state->sp + 1];
			state->sp += 2;
			break;
		case 0xe1:
			state->c = state_mem[state->sp];
			state->b = state_mem[state->sp + 1];
			state->sp += 2;
			break;

		// POP PSW
		case 0xf1:
			{
				uint8_t word = state_mem[state->sp];
				state->cf.cy = word & 0x1;
				state->cf.p = (word >> 2) & 0x1;
				state->cf.ac = (word >> 4) & 0x1;
				state->cf.z = (word >> 6) & 0x1;
				state->cf.s = (word >> 7) & 0x1;
			}
			state->a = state_mem[state->sp + 1];
			state->sp += 2;
			break;

		// XTHL
		case 0xe3:
			{
				uint8_t tmp = state_mem[state->sp];
				state_mem[state->sp] = state->l;
				state->l = tmp;
				tmp = state_mem[state->sp + 1];
				state_mem[state->sp + 1] = state->h;
				state->h = tmp;
			}
			break;

		// SPHL
		case 0xf9:
			state->sp = combine_two_8bit(state->l, state->h);
			break;

		// IN port
		case 0xdb:
			printf("IN    #0x%02x", opcode[1]);
			state->pc++;
			break;

		// OUT port
		case 0xd3:
			printf("OUT   #0x%02x", opcode[1]);
			state->pc++;
			break;

		// EI
		case 0xfb:
			printf("EI");
			state->int_enable = 1;
			break;

		// DI
		case 0xf3:
			unassigned_instruction(state);
			break;

		// HLT
		case 0x76:
			unassigned_instruction(state);
			break;

		// NOP
		case 0x00:
			break;

		default:
			puts("missing instruction!!!");
			break;
	}
	printf("\tC=%d,P=%d,S=%d,Z=%d\n", state->cf.cy, state->cf.p, state->cf.s, state->cf.z);
	printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->sp);      
	state->pc++;
	return 0;
}

void initialize_state(struct State8080 *state, uint16_t pc, uint8_t *buffer)
{
	state->pc = pc;
	state->cf.ac = 0;
	state->cf.cy = 0;
	state->cf.p = 0;
	state->cf.s = 0;
	state->cf.z = 0;
	state->a = 0;
	state->b = 0;
	state->c = 0;
	state->d = 0;
	state->e = 0;
	state->h = 0;
	state->l = 0;
	state->int_enable = 0;
	state->sp = 0;
	state->memory = buffer;
}

int main(int argc, char *argv[])
{
	FILE *f = fopen(argv[1], "rb"); // open binary file in read-only mode
	if ( f == NULL )
	{
		printf("error: Could not open %s\n", argv[1]);
		exit(1);
	}

	// Get the file size and read it into a memory buffer
	fseek(f, 0L, SEEK_END); // Set position of pointer to end of file
	int fsize = ftell(f); // Get position of file pointer, i.e. how big the file is
	fseek(f, 0L, SEEK_SET); // Reset position

	unsigned char *buffer = malloc(fsize); // Allocate memory equal to the size of the file

	fread(buffer, fsize, 1, f); // read into buffer the first byte of f
	fclose(f);

	struct State8080 state;
	initialize_state(&state, 0, buffer);
	while ( state.pc < fsize )
	{
		emulate_8080(&state);
	}
	return 0;
}
