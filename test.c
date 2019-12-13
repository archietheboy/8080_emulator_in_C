enum
{
	REG_A = 0x7,	// 111b
	REG_B = 0x0,	// 000b
	REG_C = 0x1,	// 001b
	REG_D = 0x2,	// 010b
	REG_E = 0x3,	// 011b
	REG_H = 0x4,	// 100b
	REG_L = 0x5,	// 101b
	REG_M = 0x6	// 110b
};

char register_name(unsigned char bitpattern)
{
	switch (bitpattern)
	{
		case REG_A: return 'A';
		case REG_B: return 'B';
		case REG_C: return 'C';
		case REG_D: return 'D';
		case REG_E: return 'E';
		case REG_H: return 'H';
		case REG_L: return 'L';
		case REG_M: return 'M';
		default: return '?';
	}
}

enum
{
	RP_BC = 0x0,	// 00b
	RP_DE = 0x1,	// 01b
	RP_HL = 0x2,	// 10b
	RP_SP = 0x3	// 11b
};

enum
{
	COND_NZ = 0x0,	// not zero (Z=0)
	COND_Z  = 0x1,	// zero (Z=1)
	COND_NC = 0x2,	// no carry (CY=0)
	COND_C  = 0x3,	// carry (CY=1)
	COND_PO = 0x4,	// parity odd (P=0)
	COND_PE = 0x5,	// parity even (P=1)
	COND_P  = 0x6,	// plus (S=0)
	COND_M  = 0x7	// minus (S=1)
};

char *condition_name(unsigned char bitpattern)
{
	switch (bitpattern)
	{
		case COND_NZ: return "NZ";
		case COND_Z: return "Z";
		case COND_NC: return "NC";
		case COND_C: return "C";
		case COND_PO: return "PO";
		case COND_PE: return "PE";
		case COND_P: return "P";
		case COND_M: return "M";
		default: return "?";
	}
}

int 8080_dissasembly_opcode(unsigned char *codebuffer, int pc)
{
	unsigned char *code = &codebuffer[pc];
	int opbytes = 1; /* initializing to 1 as most of the structions have size 1 */
	printf("%04x ", pc); /* printing offset into the code as a 16-bit hexadecimal address */
	switch ( *code )
	{
		case 0x07: // 00000111
			/* RLC		(Rotate left)
				(A[n+1]) <- (A[n]); (A[0]) <- (A[7])
				(CY) <- (A[7])
				Notes:
					- The low order bit and the CY flag are both set to the value shifted out of the high order bit position.
					- Only the CY flag is affected.
			 */
			printf("RLC");
			break;
		case 0x0f: // 00001111
			/* RRC		(Rotate right)
				(A[n]) <- (A[n+1]); (A[7]) <- (A[0])
				(CY) <- (A[0])
				Notes:
					- The high order bit and the CY flag are both set to the value shifted out of the low order bit position.
					- Only the CY flag is affected.
			 */
			printf("RRC");
			break;
		case 0x17: // 00010111
			/* RAL		(Rotate left through carry)
				(A[n+1]) <- (A[n]); (CY) <- (A[7])
				(A[0]) <- (CY)
				Notes:
					- The low order bit is set equal to the CY flag and the CY flag is set to the value shifted out of the high order bit.
					- Only the CY flag is affected.
			 */
			printf("RAL");
			break;
		case 0x1f: // 00011111
			/* RAR		(Rotate right through carry)
				(A[n]) <- (A[n+1]); (CY) <- (A[0])
				(A[7]) <- (CY)
				Notes:
					- The high order bit is set equal to the CY flag and the CY flag is set to the value shited out of the low order bit.
					- Only the CY flag is affected.
			 */
			printf("RAR");
			break;
		case 0x27: // 00100111
			/* DAA		(Decimal Adjust Accumulator)
				The eight-bit number in the accumulator is adjusted to form two four-bit Binary-Coded-Decimal digits by the following process:
				1. If the value of the least significant 4 bits of the accumulator is greater than 9 OR if the AC flag is set, 6 is added to the accumulator.
				2. If the value of the most significant 4 bits is now greater than 9, OR if the CY flag is set, 6 is added to the most significant 4 bits of the accumulator.
			 */
			printf("DAA");
			break;
		case 0x2a: // 00101010
			/* LHLD addr	(Load H and L direct)
				(L) <- ((byte3)(byte2))
				(H) <- ((byte3)(byte2)+1)
			 */
			printf("LHLD   $%02x%02x", code[2], code[1]);
			opbytes = 3;
			break;
		case 0x2f: // 00101111
			/* CMA		(Complement accumulator)
				(A) <- (_A_)
				Note: No flags are affected.
			 */
			printf("CMA");
			break;
		case 0x32: // 00110010
			/* STA addr	(Store Accumulator direct)
				((byte3)(byte2)) <- (A)
			 */
			printf("STA    $%02x%02x", code[2], code[1]);
			opbytes = 3;
			break;
		case 0x34: // 00110100
			/* INR M	(Increment memory)
				((H)(L)) <- ((H)(L)) + 1
				Note: All condition flags except CY are affected.
			 */
			printf("INR    M");
			break;
		case 0x35: // 00110101
			/* DCR M	(Decrement memory)
				((H)(L)) <- ((H)(L)) - 1
				Note: All condition flags except CY are affected.
			 */
			printf("DCR    M");
			break;
		case 0x36: // 00110110
			/* MVI M,data	(Move to memory immediate)
				((H)(L)) <- (byte 2)
			 */
			printf("MVI    M,#0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0x3a: // 00111010
			/* LDA addr	(Load Accumulator direct)
				(A) <- ((byte3)(byte2))
			 */	
			printf("LDA    $%02x%02x", code[2], code[1]);
			opbytes = 3;
			break;
		case 0x37: // 00110111
			/* STC		(Set carry)
				(CY) <- 1
				Note: No other flags are affected.
			 */
			printf("STC");
			break;
		case 0x3f: // 00111111
			/* CMC		(Complement carry)
				(CY) <- (_CY_)
				Note: No other flags are affected.
			 */
			printf("CMC");
			break;
		case 0x86: // 10000110
			/* ADD M	(Add memory)
				(A) <- (A) + ((H)(L))
			 */
			printf("ADD    M");
			break;
		case 0x8e: // 10001110
			/* ADC M	(Add memory with carry)
				(A) <- ((H)(L)) + (CY)
			 */
			printf("ADC    M");
			break;
		case 0x96: // 10010110
			/* SUB M	(Subtract memory)
				(A) <- (A) - ((H)(L)
			 */
			printf("SUB    M");
			break;
		case 0x9e: // 10011110
			/* SBB M	(Subtract memory with borrow)
				(A) <- (A) - ((H)(L)) - (CY)
			 */
			printf("SBB    M");
			break;
		case 0xa6: // 10100110
			/* ANA M	(AND memory)
				(A) <- (A) & ((H)(L))
			 */
			printf("ANA    M");
			break;
		case 0xae: // 10101110
			/* XRA M	(Exclusive OR memory)
				(A) <- (A) XOR ((H)(L))
				Note: The CY and AC flags are cleared.
			 */
			printf("XRA    M");
			break;
		case 0xb6: // 10110110
			/* ORA M	(OR memory)
				(A) <- (A) V ((H)(L))
				Note: The CY and AC flags are cleared.
			 */
			printf("ORA    M");
			break;
		case 0xbe: // 10111110
			/* CMP M	(Compare memory)
				(A) - ((H)(L))
				Notes:
					- The condition flags are set as a result of the subtraction.
					- The Z flag is set to 1 if (A) = ((H)(L)). The CY flag is set to 1 if (A) < ((H)(L)).
			 */
			printf("CMP    M");
			break;
		case 0xc3: // 11000011
			/* JMP addr	(Jump)
				(PC) <- (byte3)(byte2)
			 */
			printf("JMP    $%02x%02x", code[2], code[1]);
			opbytes = 3;
			break;
		case 0xc6: // 11000110
			/* ADI data	(Add immediate)
				(A) <- (A) + (byte2)
			 */
			printf("ADI	#0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xc9: // 11001001
			/* RET		(Return)
			 	(PCL) <- ((SP))
				(PCH) <- ((SP) + 1)
				(SP) <- (SP) + 2
			 */
			printf("RET");
			break;
		case 0xcd: // 11001101
			/* CALL addr	(Call)
				((SP) - 1) <- (PCH)
				((SP) - 2) <- (PCL)
				(SP) <- (SP) - 2
				(PC) <- (byte3)(byte2)
			 */
			printf("CALL   $%02x%02x", code[2], code[1]);
			opbytes = 3;
			break;
		case 0xce: // 11001110
			/* ACI data	(Add immediate with carry)
				(A) <- (A) + (byte2) + (CY)
			 */
			printf("ACI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xd6: // 11010110
			/* SUI data	(Subtract immediate)
				(A) <- (A) - (byte2)
			 */
			printf("SUI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xde: // 11011110
			/* SBI data	(Substract immediate with borrow)
				(A) <- (A) - (byte2) - (CY)
			 */
			printf("SBI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xe6: // 11100110
			/* ANI data	(AND immediate)
				(A) <- (A) & (byte2)
				Note: The CY and AC flags are cleared.
			 */
			printf("ANI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xe9: // 11101001
			/* PCHL		(Jump H and L indirect - move H and L to PC)
				(PCH) <- (H)
				(PCL) <- (L)
			 */
		case 0xeb: // 11101011
			/* XCHG		(Exchange H and L with D and E)
				(H) <- (D)
				(L) <- (E)
			 */
			printf("XCHG");
			break;
		case 0xee: // 11101110
			/* XRI data	(Exclusive OR immediate)
				(A) <- (A) XOR (byte2)
				Note: The CY and AC flags are cleared.
			 */
			printf("XRI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xf6: // 11110110
			/* ORI data	(OR immediate)
				(A) <- (A) V (byte2)
				Note: The CY and AC flags are cleared.
			 */
			printf("ORI    #0x%02x", code[1]);
			opbytes = 2;
			break;
		case 0xf9: // 11111001
			/* SPHL		(Move HL to SP)
				(SP) <- (H)(L)
			 */
			printf("SPHL");
			break;
		case 0xfe: // 11111110
			/* CPI data	(Compare immediate)
				(A) - (byte2)
				Notes:
					- The condition flags are set by the result of the subtraction.
					- The Z flag is set to 1 if (A) = (byte2). The CY flag is set to 1 if  (A) < (byte2).
			 */
		default:
			break;
	}
}

/*
 * 'codebuffer' is a valid pointer to 8080 binary code
 * 'pc' is the current offset into the code
 */
int 8080_dissasemble_opcode(unsigned char *codebuffer, int pc)
{
	unsigned char *code = &codebuffer[pc];
	int opbytes = 1; /* initializing to 1 as most of the structions have size 1 */
	printf("%04x ", pc); /* printing offset into the code as a 16-bit hexadecimal address */
	switch (*code)
	{
		case 0x00: printf("NOP"); break;
		case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opbytes = 3; break; /* byte[3] first, byte[2] last */
		case 0x02: printf("STAX   B"); break;
		case 0x03: printf("INX    B"); break;
		case 0x04: printf("INR    B"); break;
		case 0x05: printf("DCR    B"); break;
		case 0x06: printf("MVI    B,#$%02x", code[1]); opbytes = 2; break;
		case 0x07: printf("RLC"); break;
		case 0x08:
		case 0x09: printf("DAD    B"); break;
		case 0x0a: printf("LDAX   B"); break;
		case 0x0b: printf("DCX    B"); break;
		case 0x0c: printf("INR    C"); break;
		case 0x0d: printf("DCR    C"); break;
		case 0x0e: printf("MVI    C,#$%02x", code[1]); opbytes = 2; break;
		case 0x0f: printf("RRC"); break;
		case 0x10:
		case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x12: printf("STAX   D"); break;
		case 0x13: printf("INX    D"); break;
		case 0x14: printf("INR    D"); break;
		case 0x15: printf("DCR    D"); break;
		case 0x16: printf("MVI    D,#$%02x", code[1]); opbytes = 2; break;
		case 0x17: printf("RAL"); break;
		case 0x18:
		case 0x19: printf("DAD    D"); break;
		case 0x1a: printf("LDAX   D"); break;
		case 0x1b: printf("DCX    D"); break;
		case 0x1c: printf("INR    E"); break;
		case 0x1d: printf("DCR    E"); break;
		case 0x1e: printf("MVI    E,#$%02x", code[1]); opbytes = 2; break;
		case 0x1f: printf("RAR"); break;
		case 0x20:
		case 0x21: printf("LXI    H,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x22: printf(""); break;
		case 0x23: printf(""); break;
		case 0x24: printf(""); break;
		case 0x25: printf(""); break;
		case 0x26: printf(""); break;
		case 0x27: printf(""); break;
		case 0x28: printf(""); break;
		case 0x29: printf(""); break;
		case 0x2a: printf(""); break;
		case 0x2b: printf(""); break;
		case 0x2c: printf(""); break;
		case 0x2d: printf(""); break;
		case 0x2e: printf(""); break;
		case 0x2f: printf(""); break;
		case 0x30: printf(""); break;
		case 0x31: printf(""); break;
		case 0x32: printf(""); break;
		case 0x33: printf(""); break;
		case 0x34: printf(""); break;
		case 0x35: printf(""); break;
		case 0x36: printf(""); break;
		case 0x37: printf(""); break;
		case 0x38: printf(""); break;
		case 0x39: printf(""); break;
		case 0x3a: printf(""); break;
		case 0x3b: printf(""); break;
		case 0x3c: printf(""); break;
		case 0x3d: printf(""); break;
		case 0x3e: printf(""); break;
		case 0x3f: printf(""); break;
		case 0x40: printf("MOV    B,B"); break;
		case 0x41: printf("MOV    B,C"); break;
		case 0x42: printf("MOV    B,D"); break;
		case 0x43: printf("MOV    B,E"); break;
		case 0x44: printf("MOV    B,H"); break;
		case 0x45: printf("MOV    B,L"); break;
		case 0x46: printf("MOV    B,M"); break; /* B <- (HL) */
		case 0x47: printf("MOV    B,A"); break;
		case 0x48: printf("MOV    C,B"); break;
		case 0x49: printf("MOV    C,C"); break;
		case 0x4a: printf("MOV    C,D"); break;
		case 0x4b: printf("MOV    C,E"); break;
		case 0x4c: printf("MOV    C,H"); break;
		case 0x4d: printf("MOV    C,L"); break;
		case 0x4e: printf("MOV    C,M"); break;
		case 0x4f: printf("MOV    C,A"); break;
		case 0x50: printf("MOV    D,B"); break;
		case 0x51: printf("MOV    D,C"); break;
		case 0x52: printf("MOV    D,D"); break;
		case 0x53: printf("MOV    D,E"); break;
		case 0x54: printf("MOV    D,H"); break;
		case 0x55: printf("MOV    D,L"); break;
		case 0x56: printf("MOV    D,M"); break;
		case 0x57: printf("MOV    D,A"); break;
		case 0x58: printf("MOV    E,B"); break;
		case 0x59: printf("MOV    E,C"); break;
		case 0x5a: printf("MOV    E,D"); break;
		case 0x5b: printf("MOV    E,E"); break;
		case 0x5c: printf("MOV    E,H"); break;
		case 0x5d: printf("MOV    E,L"); break;
		case 0x5e: printf("MOV    E,M"); break;
		case 0x5f: printf("MOV    E,A"); break;
		case 0x60: printf("MOV    H,B"); break;
		case 0x61: printf("MOV    H,C"); break;
		case 0x62: printf("MOV    H,D"); break;
		case 0x63: printf("MOV    H,E"); break;
		case 0x64: printf("MOV    H,H"); break;
		case 0x65: printf("MOV    H,L"); break;
		case 0x66: printf("MOV    H,M"); break;
		case 0x67: printf("MOV    H,A"); break;
		case 0x68: printf("MOV    L,B"); break;
		case 0x69: printf("MOV    L,C"); break;
		case 0x6a: printf("MOV    L,D"); break;
		case 0x6b: printf("MOV    L,E"); break;
		case 0x6c: printf("MOV    L,H"); break;
		case 0x6d: printf("MOV    L,L"); break;
		case 0x6e: printf("MOV    L,M"); break;
		case 0x6f: printf("MOV    L,A"); break;
		case 0x70: printf("MOV    M,B"); break;
		case 0x71: printf("MOV    M,C"); break;
		case 0x72: printf("MOV    M,D"); break;
		case 0x73: printf("MOV    M,E"); break;
		case 0x74: printf("MOV    M,H"); break;
		case 0x75: printf("MOV    M,L"); break;
		case 0x76: printf("HLT"); break;
		case 0x77: printf("MOV    M,A"); break;
		case 0x78: printf("MOV    A,B"); break;
		case 0x79: printf("MOV    A,C"); break;
		case 0x7a: printf("MOV    A,D"); break;
		case 0x7b: printf("MOV    A,E"); break;
		case 0x7c: printf("MOV    A,H"); break;
		case 0x7d: printf("MOV    A,L"); break;
		case 0x7e: printf("MOV    A,M"); break;
		case 0x7f: printf("MOV    A,A"); break;
		case 0x80: printf(""); break;
		case 0x81: printf(""); break;
		case 0x82: printf(""); break;
		case 0x83: printf(""); break;
		case 0x84: printf(""); break;
		case 0x85: printf(""); break;
		case 0x86: printf(""); break;
		case 0x87: printf(""); break;
		case 0x88: printf(""); break;
		case 0x89: printf(""); break;
		case 0x8a: printf(""); break;
		case 0x8b: printf(""); break;
		case 0x8c: printf(""); break;
		case 0x8d: printf(""); break;
		case 0x8e: printf(""); break;
		case 0x8f: printf(""); break;
		case 0x90: printf(""); break;
		case 0x91: printf(""); break;
		case 0x92: printf(""); break;
		case 0x93: printf(""); break;
		case 0x94: printf(""); break;
		case 0x95: printf(""); break;
		case 0x96: printf(""); break;
		case 0x97: printf(""); break;
		case 0x98: printf(""); break;
		case 0x99: printf(""); break;
		case 0x9a: printf(""); break;
		case 0x9b: printf(""); break;
		case 0x9c: printf(""); break;
		case 0x9d: printf(""); break;
		case 0x9e: printf(""); break;
		case 0x9f: printf(""); break;
		case 0xa0: printf(""); break;
		case 0xa1: printf(""); break;
		case 0xa2: printf(""); break;
		case 0xa3: printf(""); break;
		case 0xa4: printf(""); break;
		case 0xa5: printf(""); break;
		case 0xa6: printf(""); break;
		case 0xa7: printf(""); break;
		case 0xa8: printf(""); break;
		case 0xa9: printf(""); break;
		case 0xaa: printf(""); break;
		case 0xab: printf(""); break;
		case 0xac: printf(""); break;
		case 0xad: printf(""); break;
		case 0xae: printf(""); break;
		case 0xaf: printf(""); break;
		case 0xb0: printf(""); break;
		case 0xb1: printf(""); break;
		case 0xb2: printf(""); break;
		case 0xb3: printf(""); break;
		case 0xb4: printf(""); break;
		case 0xb5: printf(""); break;
		case 0xb6: printf(""); break;
		case 0xb7: printf(""); break;
		case 0xb8: printf(""); break;
		case 0xb9: printf(""); break;
		case 0xba: printf(""); break;
		case 0xbb: printf(""); break;
		case 0xbc: printf(""); break;
		case 0xbd: printf(""); break;
		case 0xbe: printf(""); break;
		case 0xbf: printf(""); break;
		case 0xc0: printf(""); break;
		case 0xc1: printf(""); break;
		case 0xc2: printf(""); break;
		case 0xc3: printf(""); break;
		case 0xc4: printf(""); break;
		case 0xc5: printf(""); break;
		case 0xc6: printf(""); break;
		case 0xc7: printf(""); break;
		case 0xc8: printf(""); break;
		case 0xc9: printf(""); break;
		case 0xca: printf(""); break;
		case 0xcb: printf(""); break;
		case 0xcc: printf(""); break;
		case 0xcd: printf(""); break;
		case 0xce: printf(""); break;
		case 0xcf: printf(""); break;
		case 0xd0: printf(""); break;
		case 0xd1: printf(""); break;
		case 0xd2: printf(""); break;
		case 0xd3: printf(""); break;
		case 0xd4: printf(""); break;
		case 0xd5: printf(""); break;
		case 0xd6: printf(""); break;
		case 0xd7: printf(""); break;
		case 0xd8: printf(""); break;
		case 0xd9: printf(""); break;
		case 0xda: printf(""); break;
		case 0xdb: printf(""); break;
		case 0xdc: printf(""); break;
		case 0xdd: printf(""); break;
		case 0xde: printf(""); break;
		case 0xdf: printf(""); break;
		case 0xe0: printf(""); break;
		case 0xe1: printf(""); break;
		case 0xe2: printf(""); break;
		case 0xe3: printf(""); break;
		case 0xe4: printf(""); break;
		case 0xe5: printf(""); break;
		case 0xe6: printf(""); break;
		case 0xe7: printf(""); break;
		case 0xe8: printf(""); break;
		case 0xe9: printf(""); break;
		case 0xea: printf(""); break;
		case 0xeb: printf(""); break;
		case 0xec: printf(""); break;
		case 0xed: printf(""); break;
		case 0xee: printf(""); break;
		case 0xef: printf(""); break;
		case 0xf0: printf(""); break;
		case 0xf1: printf(""); break;
		case 0xf2: printf(""); break;
		case 0xf3: printf(""); break;
		case 0xf4: printf(""); break;
		case 0xf5: printf(""); break;
		case 0xf6: printf(""); break;
		case 0xf7: printf(""); break;
		case 0xf8: printf(""); break;
		case 0xf9: printf(""); break;
		case 0xfa: printf(""); break;
		case 0xfb: printf(""); break;
		case 0xfc: printf(""); break;
		case 0xfd: printf(""); break;
		case 0xfe: printf(""); break;
		case 0xff: printf(""); break;

	}
	fputc("\n", stdout);
	return opbytes;
}

