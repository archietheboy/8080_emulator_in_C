#include <stdio.h>
#include <stdlib.h>

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

/*
 * 'codebuffer' is a valid pointer to 8080 binary code
 * 'pc' is the current offset into the code
 */
int e8080_dissasemble_opcode(unsigned char *codebuffer, int pc)
{
	unsigned char *code = &codebuffer[pc];
	int opbytes = 1; // initializing to 1 as most of the structions have size 1
	printf("%04x\t\t", pc); // printing offset into the code as a 16-bit hexadecimal address
	const char *fmt;
	switch (*code)
	{
		case 0x40: // 01000000
			/*
				<Move register>
				MOV r1,r2
					(r1) <- (r2)
			*/
			fmt = "MOV	B,B";
			break;
		case 0x41: // 01000001
			fmt = "MOV	B,C";
			break;
		case 0x42: // 01000010
			fmt = "MOV	B,D";
			break;
		case 0x43: // 01000011
			fmt = "MOV	B,E";
			break;
		case 0x44: // 01000100
			fmt = "MOV	B,H";
			break;
		case 0x45: // 01000101
			fmt = "MOV	B,L";
			break;
		case 0x47: // 01000111
			fmt = "MOV	B,A";
			break;
		case 0x48: // 01001000
			fmt = "MOV	C,B";
			break;
		case 0x49: // 01001001
			fmt = "MOV	C,C";
			break;
		case 0x4a: // 01001010
			fmt = "MOV	C,D";
			break;
		case 0x4b: // 01001011
			fmt = "MOV	C,E";
			break;
		case 0x4c: // 01001100
			fmt = "MOV	C,H";
			break;
		case 0x4d: // 01001101
			fmt = "MOV	C,L";
			break;
		case 0x4f: // 01001111
			fmt = "MOV	C,A";
			break;
		case 0x50: // 01010000
			fmt = "MOV	D,B";
			break;
		case 0x51: // 01010001
			fmt = "MOV	D,C";
			break;
		case 0x52: // 01010010
			fmt = "MOV	D,D";
			break;
		case 0x53: // 01010011
			fmt = "MOV	D,E";
			break;
		case 0x54: // 01010100
			fmt = "MOV	D,H";
			break;
		case 0x55: // 01010101
			fmt = "MOV	D,L";
			break;
		case 0x57: // 01010111
			fmt = "MOV	D,A";
			break;
		case 0x58: // 01011000
			fmt = "MOV	E,B";
			break;
		case 0x59: // 01011001
			fmt = "MOV	E,C";
			break;
		case 0x5a: // 01011010
			fmt = "MOV	E,D";
			break;
		case 0x5b: // 01011011
			fmt = "MOV	E,E";
			break;
		case 0x5c: // 01011100
			fmt = "MOV	E,H";
			break;
		case 0x5d: // 01011101
			fmt = "MOV	E,L";
			break;
		case 0x5f: // 01011111
			fmt = "MOV	E,A";
			break;
		case 0x60: // 01100000
			fmt = "MOV	H,B";
			break;
		case 0x61: // 01100001
			fmt = "MOV	H,C";
			break;
		case 0x62: // 01100010
			fmt = "MOV	H,D";
			break;
		case 0x63: // 01100011
			fmt = "MOV	H,E";
			break;
		case 0x64: // 01100100
			fmt = "MOV	H,H";
			break;
		case 0x65: // 01100101
			fmt = "MOV	H,L";
			break;
		case 0x67: // 01100111
			fmt = "MOV	H,A";
			break;
		case 0x68: // 01101000
			fmt = "MOV	L,B";
			break;
		case 0x69: // 01101001
			fmt = "MOV	L,C";
			break;
		case 0x6a: // 01101010
			fmt = "MOV	L,D";
			break;
		case 0x6b: // 01101011
			fmt = "MOV	L,E";
			break;
		case 0x6c: // 01101100
			fmt = "MOV	L,H";
			break;
		case 0x6d: // 01101101
			fmt = "MOV	L,L";
			break;
		case 0x6f: // 01101111
			fmt = "MOV	L,A";
			break;
		case 0x78: // 01111000
			fmt = "MOV	A,B";
			break;
		case 0x79: // 01111001
			fmt = "MOV	A,C";
			break;
		case 0x7a: // 01111010
			fmt = "MOV	A,D";
			break;
		case 0x7b: // 01111011
			fmt = "MOV	A,E";
			break;
		case 0x7c: // 01111100
			fmt = "MOV	A,H";
			break;
		case 0x7d: // 01111101
			fmt = "MOV	A,L";
			break;
		case 0x7f: // 01111111
			fmt = "MOV	A,A";
			break;
		case 0x46: // 01000110
			/*
				<Move from memory>
				MOV r,M
					(r) <- ((H)(L))
			*/
			fmt = "MOV	B,M";
			break;
		case 0x4e: // 01001110
			fmt = "MOV	C,M";
			break;
		case 0x56: // 01010110
			fmt = "MOV	D,M";
			break;
		case 0x5e: // 01011110
			fmt = "MOV	E,M";
			break;
		case 0x66: // 01100110
			fmt = "MOV	H,M";
			break;
		case 0x6e: // 01101110
			fmt = "MOV	L,M";
			break;
		case 0x7e: // 01111110
			fmt = "MOV	A,M";
			break;
		case 0x70: // 01110000
			/*
				<Move to memory>
				MOV M,r
					((H)(L)) <- (r)
			*/
			fmt = "MOV	M,B";
			break;
		case 0x71: // 01110001
			fmt = "MOV	M,C";
			break;
		case 0x72: // 01110010
			fmt = "MOV	M,D";
			break;
		case 0x73: // 01110011
			fmt = "MOV	M,E";
			break;
		case 0x74: // 01110100
			fmt = "MOV	M,H";
			break;
		case 0x75: // 01110101
			fmt = "MOV	M,L";
			break;
		case 0x77: // 01110111
			fmt = "MOV	M,A";
			break;
		case 0x06: // 00000110
			/*
				<Move immediate>
				MVI r,data
					(r) <- (byte2)
			*/
			fmt = "MVI	B,#0x%02x";
			opbytes = 2;
			break;
		case 0x0e: // 00001110
			fmt = "MVI	C,#0x%02x";
			opbytes = 2;
			break;
		case 0x16: // 00010110
			fmt = "MVI	D,#0x%02x";
			opbytes = 2;
			break;
		case 0x1e: // 00011110
			fmt = "MVI	E,#0x%02x";
			opbytes = 2;
			break;
		case 0x26: // 00100110
			fmt = "MVI	H,#0x%02x";
			opbytes = 2;
			break;
		case 0x2e: // 00101110
			fmt = "MVI	L,#0x%02x";
			opbytes = 2;
			break;
		case 0x3e: // 00111110
			fmt = "MVI	A,#0x%02x";
			opbytes = 2;
			break;
		case 0x36: // 00110110
			/*
				<Move to memory immediate>
				MVI M,data
					((H)(L)) <- (byte2)
			*/
			fmt = "MVI	M,#0x%02x";
			opbytes = 2;
			break;
		case 0x01: // 00000001
			/*
				<Load register pair immediate>
				LXI rp,data 16
					(rh) <- (byte3)
					(rl) <- (byte2)
			*/
			fmt = "LXI	B,#0x%02x%02x";
			opbytes = 3;
			break;
		case 0x11: // 00010001
			fmt = "LXI	D,#0x%02x%02x";
			opbytes = 3;
			break;
		case 0x21: // 00100001
			fmt = "LXI	H,#0x%02x%02x";
			opbytes = 3;
			break;
		case 0x31: // 00110001
			fmt = "LXI	SP,#0x%02x%02x";
			opbytes = 3;
			break;
		case 0x3a: // 00111010
			/*
				<Load accumulator direct>
				LDA addr
					(A) <- ((byte3)(byte2))
			*/
			fmt = "LDA	#$%02x%02x";
			opbytes = 3;
			break;
		case 0x32: // 00110010
			/*
				<Store accumulator direct>
				STA addr
					((byte3)(byte2)) <- (A)
			*/
			fmt = "STA	#$%02x%02x";
			opbytes = 3;
			break;
		case 0x2a: // 00101010
			/*
				<Load H and L direct>
				LHLD addr
					(L) <- ((byte3)(byte2))
					(H) <- ((byte3)(byte2) + 1)
			*/
			fmt = "LHLD	#$%02x%02x";
			opbytes = 3;
			break;
		case 0x22: // 00100010
			/*
				<Store H and L direct>
				SHLD addr
					((byte3)(byte2)) <- (L)
					((byte3)(byte2) + 1) <- (H)
			*/
			fmt = "SHLD	#$%02x%02x";
			opbytes = 3;
			break;
		case 0x0a: // 00001010
			/*
				<Load accumulator indirect>
				LDAX rp
					(A) <- ((rp))
			*/
			fmt = "LDAX	B";
			break;
		case 0x1a: // 00011010
			fmt = "LDAX	D";
			break;
		case 0x02: // 00000010
			/*
				<Store accumulator indirect>
				STAX rp
					((rp)) <- (A)
			*/
			fmt = "STAX	B";
			break;
		case 0x12: // 00010010
			fmt = "STAX	D";
			break;
		case 0xeb: // 11101011
			/*
				<Exchange H and L with D and E>
				XCHG
					(H) <-> (D)
					(L) <-> (E)
			*/
			fmt = "XCHG";
			break;
		case 0x80: // 10000000
			/*
				<Add register>
				ADD r
					(A) <- (A) + (r)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ADD	B";
			break;
		case 0x81: // 10000001
			fmt = "ADD	C";
			break;
		case 0x82: // 10000010
			fmt = "ADD	D";
			break;
		case 0x83: // 10000011
			fmt = "ADD	E";
			break;
		case 0x84: // 10000100
			fmt = "ADD	H";
			break;
		case 0x85: // 10000101
			fmt = "ADD	L";
			break;
		case 0x87: // 10000111
			fmt = "ADD	A";
			break;
		case 0x86: // 10000110
			/*
				<Add memory>
				ADD M
					(A) <- (A) + ((H)(L))
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ADD	M";
			break;
		case 0xc6: // 11000110
			/*
				<Add immediate>
				ADI data
					(A) <- (A) + (byte2)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ADI	#0x%02x";
			opbytes = 2;
			break;
		case 0x88: // 10001000
			/*
				<Add register with carry>
				ADC r
					(A) <- (A) + (r) + (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ADC	B";
			break;
		case 0x89: // 10001001
			fmt = "ADC	C";
			break;
		case 0x8a: // 10001010
			fmt = "ADC	D";
			break;
		case 0x8b: // 10001011
			fmt = "ADC	E";
			break;
		case 0x8c: // 10001100
			fmt = "ADC	H";
			break;
		case 0x8d: // 10001101
			fmt = "ADC	L";
			break;
		case 0x8f: // 10001111
			fmt = "ADC	A";
			break;
		case 0x8e: // 10001110
			/*
				<Add memory with carry>
				ADC M
					(A) <- (A) + ((H)(L)) + (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ADC	M";
			break;
		case 0xce: // 11001110
			/*
				<Add immediate with carry>
				ACI data
					(A) <- (A) + (byte2) + (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "ACI	#0x%02x";
			opbytes = 2;
			break;
		case 0x90: // 10010000
			/*
				<Subtract register>
				SUB r
					(A) <- (A) - (r)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SUB	B";
			break;
		case 0x91: // 10010001
			fmt = "SUB	C";
			break;
		case 0x92: // 10010010
			fmt = "SUB	D";
			break;
		case 0x93: // 10010011
			fmt = "SUB	E";
			break;
		case 0x94: // 10010100
			fmt = "SUB	H";
			break;
		case 0x95: // 10010101
			fmt = "SUB	L";
			break;
		case 0x97: // 10010111
			fmt = "SUB	A";
			break;
		case 0x96: // 10010110
			/*
				<Subtract memory>
				SUB M
					(A) <- (A) - ((H)(L))
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SUB	M";
			break;
		case 0xd6: // 11010110
			/*
				<Subtract immediate>
				SUI data
					(A) <- (A) - (byte2)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SUI	#0x%02x";
			opbytes = 2;
			break;
		case 0x98: // 10011000
			/*
				<Subtract register with borrow>
				SBB r
					(A) <- (A) - (r) - (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SBB	B";
			break;
		case 0x99: // 10011001
			fmt = "SBB	C";
			break;
		case 0x9a: // 10011010
			fmt = "SBB	D";
			break;
		case 0x9b: // 10011011
			fmt = "SBB	E";
			break;
		case 0x9c: // 10011100
			fmt = "SBB	H";
			break;
		case 0x9d: // 10011101
			fmt = "SBB	L";
			break;
		case 0x9f: // 10011111
			fmt = "SBB	A";
			break;
		case 0x9e: // 10011110
			/*
				<Subtract memory with borrow>
				SBB M
					(A) <- (A) - ((H)(L)) - (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SBB	M";
			break;
		case 0xde: // 11011110
			/*
				<Subtract immediate with borrow>
				SBI data
					(A) <- (A) - (byte2) - (CY)
				
				flags: Z,S,P,CY,AC
			*/
			fmt = "SBI	#0x%02x";
			opbytes = 2;
			break;
		case 0x04: // 00000100
			/*
				<Increment register>
				INR r
					(r) <- (r) + 1
				
				flags: Z,S,P,AC
			*/
			fmt = "INR	B";
			break;
		case 0x0c: // 00001100
			fmt = "INR	C";
			break;
		case 0x14: // 00010100
			fmt = "INR	D";
			break;
		case 0x1c: // 00011100
			fmt = "INR	E";
			break;
		case 0x24: // 00100100
			fmt = "INR	H";
			break;
		case 0x2c: // 00101100
			fmt = "INR	L";
			break;
		case 0x3c: // 00111100
			fmt = "INR	A";
			break;
		case 0x34: // 00110100
			/*
				<Increment memory>
				INR M
					((H)(L)) <- ((H)(L)) + 1
				
				flags: Z,S,P,AC
			*/
			fmt = "INR	M";
			break;
		case 0x05: // 00000101
			/*
				<Decrement register>
				DCR r
					(r) <- (r) - 1
				
				flags: Z,S,P,AC
			*/
			fmt = "DCR	B";
			break;
		case 0x0d: // 00001101
			fmt = "DCR	C";
			break;
		case 0x15: // 00010101
			fmt = "DCR	D";
			break;
		case 0x1d: // 00011101
			fmt = "DCR	E";
			break;
		case 0x25: // 00100101
			fmt = "DCR	H";
			break;
		case 0x2d: // 00101101
			fmt = "DCR	L";
			break;
		case 0x3d: // 00111101
			fmt = "DCR	A";
			break;
		case 0x35: // 00110101
			/*
				<Decrement memory>
				DCR M
					((H)(L)) <- ((H)(L)) - 1
				
				flags: Z,S,P,AC
			*/
			fmt = "DCR	M";
			break;
		case 0x03: // 00000011
			/*
				<Increment register pair>
				INX rp
					(rh)(rl) <- (rh)(rl) + 1
			*/
			fmt = "INX	B";
			break;
		case 0x13: // 00010011
			fmt = "INX	D";
			break;
		case 0x23: // 00100011
			fmt = "INX	H";
			break;
		case 0x33: // 00110011
			fmt = "INX	SP";
			break;
		case 0x0b: // 00001011
			/*
				<Decrement register pair>
				DCX rp
					(rh)(rl) <- (rh)(rl) - 1
			*/
			fmt = "DCX	B";
			break;
		case 0x1b: // 00011011
			fmt = "DCX	D";
			break;
		case 0x2b: // 00101011
			fmt = "DCX	H";
			break;
		case 0x3b: // 00111011
			fmt = "DCX	SP";
			break;
		case 0x09: // 00001001
			/*
				<Add register pair to H and L>
				DAD rp
					(H)(L) <- (H)(L) + (rh)(rl)
				
				flags: CY
			*/
			fmt = "DAD	B";
			break;
		case 0x19: // 00011001
			fmt = "DAD	D";
			break;
		case 0x29: // 00101001
			fmt = "DAD	H";
			break;
		case 0x39: // 00111001
			fmt = "DAD	SP";
			break;
		case 0x27: // 00100111
			/*
				<Decimal adjust accumulator>
				DAA
				
				flags: Z,S,P,CY,AC
				note: 
						The eight-bit number in the accumulator is adjusted to form two four-bit Binary-Coded-Decimal digits by the following process:
							1. If the value of the least significant 4 bits of the accumulator is greater than 9 or if the AC flag is set, 6 is added to the accumulator.
							2. If the value of the most 4 bits of the accumulator is now greater than 9, or if the CY flag is set, 6 is added to the most significant 4 bits of the accumulator.
			*/
			fmt = "DAA";
			break;
		case 0xa0: // 10100000
			/*
				<AND register>
				ANA r
					(A) <- (A) & (r)
				
				flags: Z,S,P,CY,AC
				note: The CY flag is cleared.
			*/
			fmt = "ANA	B";
			break;
		case 0xa1: // 10100001
			fmt = "ANA	C";
			break;
		case 0xa2: // 10100010
			fmt = "ANA	D";
			break;
		case 0xa3: // 10100011
			fmt = "ANA	E";
			break;
		case 0xa4: // 10100100
			fmt = "ANA	H";
			break;
		case 0xa5: // 10100101
			fmt = "ANA	L";
			break;
		case 0xa7: // 10100111
			fmt = "ANA	A";
			break;
		case 0xa6: // 10100110
			/*
				<AND memory>
				ANA M
					(A) <- (A) & ((H)(L))
				
				flags: Z,S,P,CY,AC
				note: The CY flag is cleared.
			*/
			fmt = "ANA	M";
			break;
		case 0xe6: // 11100110
			/*
				<AND immediate>
				ANI data
					(A) <- (A) & (byte2)
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "ANI	#0x%02x";
			opbytes = 2;
			break;
		case 0xa8: // 10101000
			/*
				<Exclusive OR register>
				XRA r
					(A) <- (A) XOR (r)
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "XRA	B";
			break;
		case 0xa9: // 10101001
			fmt = "XRA	C";
			break;
		case 0xaa: // 10101010
			fmt = "XRA	D";
			break;
		case 0xab: // 10101011
			fmt = "XRA	E";
			break;
		case 0xac: // 10101100
			fmt = "XRA	H";
			break;
		case 0xad: // 10101101
			fmt = "XRA	L";
			break;
		case 0xaf: // 10101111
			fmt = "XRA	A";
			break;
		case 0xae: // 10101110
			/*
				<Exclusive OR memory>
				XRA M
					(A) <- (A) XOR ((H)(L))
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "XRA	M";
			break;
		case 0xee: // 11101110
			/*
				<Exclusive OR immediate>
				XRI data
					(A) <- (A) XOR (byte2)
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "XRI	#0x%02x";
			opbytes = 2;
			break;
		case 0xb0: // 10110000
			/*
				<OR register>
				ORA r
					(A) <- (A) OR (r)
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "ORA	B";
			break;
		case 0xb1: // 10110001
			fmt = "ORA	C";
			break;
		case 0xb2: // 10110010
			fmt = "ORA	D";
			break;
		case 0xb3: // 10110011
			fmt = "ORA	E";
			break;
		case 0xb4: // 10110100
			fmt = "ORA	H";
			break;
		case 0xb5: // 10110101
			fmt = "ORA	L";
			break;
		case 0xb7: // 10110111
			fmt = "ORA	A";
			break;
		case 0xb6: // 10110110
			/*
				<OR memory>
				ORA M
					(A) <- (A) OR ((H)(L))
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "ORA	M";
			break;
		case 0xf6: // 11110110
			/*
				<OR immediate>
				ORI data
					(A) <- (A) OR (byte2)
				
				flags: Z,S,P,CY,AC
				note: The CY and AC flags are cleared.
			*/
			fmt = "ORI	#0x%02x";
			opbytes = 2;
			break;
		case 0xb8: // 10111000
			/*
				<Compare register>
				CMP r
					(A) - (r)
				
				flags: Z,S,P,CY,AC
				note: The Z flag is set to 1 if (A) = (r). The CY flag is set to 1 if (A) < (r).
			*/
			fmt = "CMP	B";
			break;
		case 0xb9: // 10111001
			fmt = "CMP	C";
			break;
		case 0xba: // 10111010
			fmt = "CMP	D";
			break;
		case 0xbb: // 10111011
			fmt = "CMP	E";
			break;
		case 0xbc: // 10111100
			fmt = "CMP	H";
			break;
		case 0xbd: // 10111101
			fmt = "CMP	L";
			break;
		case 0xbf: // 10111111
			fmt = "CMP	A";
			break;
		case 0xbe: // 10111110
			/*
				<Compare memory>
				CMP M
					(A) - ((H)(L))
				
				flags: Z,S,P,CY,AC
				note: The Z flag is set to 1 if (A) = ((H)(L)). The CY flag is set to 1 if (A) < ((H)(L)).
			*/
			fmt = "CMP	M";
			break;
		case 0xfe: // 11111110
			/*
				<Compare immediate>
				CPI data
					(A) - (byte2)
				
				flags: Z,S,P,CY,AC
				note: The Z flag is set to 1 if (A) = (byte2). The CY flag is set to 1 if (A) < (byte2).
			*/
			fmt = "CPI	#0x%02x";
			opbytes = 2;
			break;
		case 0x07: // 00000111
			/*
				<Rotate left>
				RLC
					(A[n+1]) <- (A[n]); (A[0]) <- (A[7])
					(CY) <- (A[7])
				
				flags: CY
			*/
			fmt = "RLC";
			break;
		case 0x0f: // 00001111
			/*
				<Rotate right>
				RRC
					(A[n]) <- (A[n+1]); (A[7]) <- (A[0])
					(CY) <- (A[0])
				
				flags: CY
			*/
			fmt = "RRC";
			break;
		case 0x17: // 00010111
			/*
				<Rotate left through carry>
				RAL
					(A[n+1]) <- (A[n]); (CY) <- (A[7])
					(A[0]) <- (CY)
				
				flags: CY
			*/
			fmt = "RAL";
			break;
		case 0x1f: // 00011111
			/*
				<Rotate right through carry>
				RAR
					(A[n]) <- (A[n+1]); (CY) <- (A[0])
					(A[7]) <- (CY)
				
				flags: CY
			*/
			fmt = "RAR";
			break;
		case 0x2f: // 00101111
			/*
				<Complement accumulator>
				CMA
					(A) <- (_A_)
			*/
			fmt = "CMA";
			break;
		case 0x3f: // 00111111
			/*
				<Complement carry>
				CMC
					(CY) <- (_CY_)
			*/
			fmt = "CMC";
			break;
		case 0x37: // 00110111
			/*
				<Set carry>
				STC
					(CY) <- 1
			*/
			fmt = "STC";
			break;
		case 0xc3: // 11000011
			/*
				<Jump>
				JMP addr
					(PC) <- (byte3)(byte2)
			*/
			fmt = "JMP	$%02x%02x";
			opbytes = 3;
			break;
		case 0xc2: // 11000010
			/*
				<Conditional jump>
				Jcondition addr
					If (CCC), (PC) <- (byte3)(byte2)
			*/
			fmt = "JNZ	$%02x%02x";
			opbytes = 3;
			break;
		case 0xca: // 11001010
			fmt = "JZ	$%02x%02x";
			opbytes = 3;
			break;
		case 0xd2: // 11010010
			fmt = "JNC	$%02x%02x";
			opbytes = 3;
			break;
		case 0xda: // 11011010
			fmt = "JC	$%02x%02x";
			opbytes = 3;
			break;
		case 0xe2: // 11100010
			fmt = "JPO	$%02x%02x";
			opbytes = 3;
			break;
		case 0xea: // 11101010
			fmt = "JPE	$%02x%02x";
			opbytes = 3;
			break;
		case 0xf2: // 11110010
			fmt = "JP	$%02x%02x";
			opbytes = 3;
			break;
		case 0xfa: // 11111010
			fmt = "JM	$%02x%02x";
			opbytes = 3;
			break;
		case 0xcd: // 11001101
			/*
				<Call>
				CALL addr
					((SP) - 1) <- (PCH)
					((SP) - 2) <- (PCL)
					(SP) <- (SP) - 2
					(PC) <- (byte3)(byte2)
			*/
			fmt = "CALL	$%02x%02x";
			opbytes = 3;
			break;
		case 0xc4: // 11000100
			/*
				<Conditional call>
				Ccondition addr
					If (CCC), 
					((SP) - 1) <- (PCH)
					((SP) - 2) <- (PCL)
					(SP) <- (SP) - 2
					(PC) <- (byte3)(byte2)
			*/
			fmt = "CNZ	$%02x%02x";
			opbytes = 3;
			break;
		case 0xcc: // 11001100
			fmt = "CZ	$%02x%02x";
			opbytes = 3;
			break;
		case 0xd4: // 11010100
			fmt = "CNC	$%02x%02x";
			opbytes = 3;
			break;
		case 0xdc: // 11011100
			fmt = "CC	$%02x%02x";
			opbytes = 3;
			break;
		case 0xe4: // 11100100
			fmt = "CPO	$%02x%02x";
			opbytes = 3;
			break;
		case 0xec: // 11101100
			fmt = "CPE	$%02x%02x";
			opbytes = 3;
			break;
		case 0xf4: // 11110100
			fmt = "CP	$%02x%02x";
			opbytes = 3;
			break;
		case 0xfc: // 11111100
			fmt = "CM	$%02x%02x";
			opbytes = 3;
			break;
		case 0xc9: // 11001001
			/*
				<Return>
				RET
					(PCL) <- ((SP))
					(PCH) <- ((SP) + 1)
					(SP) <- (SP) + 2
			*/
			fmt = "RET";
			break;
		case 0xc0: // 11000000
			/*
				<Conditional return>
				Rcondition
					If (CCC), 
					(PCL) <- ((SP))
					(PCH) <- ((SP) + 1)
					(SP) <- (SP) + 2
			*/
			fmt = "RNZ";
			break;
		case 0xc8: // 11001000
			fmt = "RZ";
			break;
		case 0xd0: // 11010000
			fmt = "RNC";
			break;
		case 0xd8: // 11011000
			fmt = "RC";
			break;
		case 0xe0: // 11100000
			fmt = "RPO";
			break;
		case 0xe8: // 11101000
			fmt = "RPE";
			break;
		case 0xf0: // 11110000
			fmt = "RP";
			break;
		case 0xf8: // 11111000
			fmt = "RM";
			break;
		case 0xc7: // 11000111
			/*
				<Restart>
				RST n
					((SP) - 1) <- (PCH)
					((SP) - 2) <- (PCL)
					(SP) <- (SP) - 2
					(PC) <- 8*(NNN)
			*/
			fmt = "RST	0";
			break;
		case 0xcf: // 11001111
			fmt = "RST	1";
			break;
		case 0xd7: // 11010111
			fmt = "RST	2";
			break;
		case 0xdf: // 11011111
			fmt = "RST	3";
			break;
		case 0xe7: // 11100111
			fmt = "RST	4";
			break;
		case 0xef: // 11101111
			fmt = "RST	5";
			break;
		case 0xf7: // 11110111
			fmt = "RST	6";
			break;
		case 0xff: // 11111111
			fmt = "RST	7";
			break;
		case 0xe9: // 11101001
			/*
				<Jump H and L indirect - move H and L to PC>
				PCHL
					(PCH) <- (H)
					(PCL) <- (L)
			*/
			fmt = "PCHL";
			break;
		case 0xc5: // 11000101
			/*
				<Push>
				PUSH rp
					((SP) - 1) <- (rh)
					((SP) - 2) <- (rl)
					(SP) <- (SP) - 2
				note: Register pair rp = SP may not be specified.
			*/
			fmt = "PUSH	B";
			break;
		case 0xd5: // 11010101
			fmt = "PUSH	D";
			break;
		case 0xe5: // 11100101
			fmt = "PUSH	H";
			break;
		case 0xf5: // 11110101
			/*
				<Push processor status word>
				PUSH PSW
					((SP) - 1) <- (A)
					((SP) - 2)[0] <- (CY), ((SP) - 2)[1] <- 1
					((SP) - 2)[2] <- (P), ((SP) - 2)[3] <- 0
					((SP) - 2)[4] <- (AC), ((SP) - 2)[5] <- 0
					((SP) - 2)[6] <- (Z), ((SP) - 2)[7] <- (S)
					(SP) <- (SP) - 2
			*/
			fmt = "PUSH	PSW";
			break;
		case 0xc1: // 11000001
			/*
				<Pop>
				POP rp
					(rh) <- ((SP) + 1)
					(rl) <- ((SP) + 2)
					(SP) <- (SP) + 2
				note: Register pair rp = SP may not be specified.
			*/
			fmt = "POP	B";
			break;
		case 0xd1: // 11010001
			fmt = "POP	D";
			break;
		case 0xe1: // 11100001
			fmt = "POP	H";
			break;
		case 0xf1: // 11110001
			/*
				<Pop processor status word>
				POP PSW
					(CY) <- ((SP))[0]
					(P) <- ((SP))[2]
					(AC) <- ((SP))[4]
					(Z) <- ((SP))[6]
					(S) <- ((SP))[7]
					(A) <- ((SP) + 1)
					(SP) <- (SP) + 2
			*/
			fmt = "POP	PSW";
			break;
		case 0xe3: // 11100011
			/*
				<Exchange stack top with H and L>
				XTHL
					(L) <-> ((SP))
					(H) <-> ((SP) + 1)
			*/
			fmt = "XTHL";
			break;
		case 0xf9: // 11111001
			/*
				<Move HL to SP>
				SPHL
					(SP) <- (H)(L)
			*/
			fmt = "SPHL";
			break;
		case 0xdb: // 11011011
			/*
				<Input>
				IN port
					(A) <- (data)
			*/
			fmt = "IN	#0x%02x";
			opbytes = 2;
			break;
		case 0xd3: // 11010011
			/*
				<Output>
				OUT port
					(data) <- (A)
			*/
			fmt = "OUT	#0x%02x";
			opbytes = 2;
			break;
		case 0xfb: // 11111011
			/*
				<Enable interrupts>
				EI
				note: The interrupt system is enabled following the execution of the next instruction.
			*/
			fmt = "EI";
			break;
		case 0xf3: // 11110011
			/*
				<Disable interrupts>
				DI
				note: The interrupt system is disabled immediately following the execution of the DI instruction.
			*/
			fmt = "DI";
			break;
		case 0x76: // 01110110
			/*
				<Halt>
				HLT
				note: The processor is stopped. The registers and flags are unnafected.
			*/
			fmt = "HLT";
			break;
		case 0x00: // 00000000
			/*
				<No op>
				NOP
				note: No operation is performed. The registers and flags are unnafected.
			*/
			fmt = "NOP";
			break;
		case 0x08: // 00001000
		case 0x10: // 00010000
		case 0x18: // 00011000
		case 0x20: // 00100000
		case 0x28: // 00101000
		case 0x30: // 00110000
		case 0x38: // 00111000
		case 0xcb: // 11001011
		case 0xd9: // 11011001
		case 0xdd: // 11011101
		case 0xed: // 11101101
		case 0xfd: // 11111101
		default:
			fmt = "ERROR";
			break;
	}
	switch (opbytes)
	{
		case 1:
			printf("%02x      \t\t", code[0]);
			printf(fmt);
			break;
		case 2:
			printf("%02x %02x   \t\t", code[0], code[1]);
			printf(fmt, code[1]);
			break;
		case 3:
			printf("%02x %02x %02x\t\t", code[0], code[1], code[2]);
			printf(fmt, code[2], code[1]);
			break;
	}
	printf("\n");
	return opbytes;
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

	int pc = 0;
	while ( pc < fsize )
	{
		pc += e8080_dissasemble_opcode(buffer, pc);
	}
	return 0;
}
