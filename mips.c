#include "mips.h"


// Swaps bytes in word (from little to big endian)
// passes through if MIPS_BIG_ENDIAN is NOT set (no change for a little endian processor)
inline unsigned int byteSwap(unsigned int word)
{
	#ifdef MIPS_BIG_ENDIAN
	return (word >> 24) + (word << 24) + ((word << 8) & 0x00FF0000) + ((word >> 8) & 0x0000FF00);
	#else
	return word;
	#endif
}

// Initialize our MIPS processor
MIPS *initMIPS(size_t memorySize, unsigned int codeOffset, unsigned int ptrGFX, unsigned int ptrArrowKeys, unsigned int ptrMouse)
{
	MIPS *m = malloc(sizeof(MIPS));
	m->memory = malloc(memorySize);
	m->memorySize = memorySize;
	m->pc = byteSwap(codeOffset);
	m->codeOffset = codeOffset;
	m->registers[0] = 0;
	m->ptrGFX = ptrGFX;
	m->mmapGFX = 0;
	m->mouseY = 0;
	m->ptrArrowKeys = ptrArrowKeys;
	m->ptrMouse = ptrMouse;
	return m;
}

// Load a program into memory at address offset
int loadProgram(MIPS *m, char *file, int offset)
{
	FILE *fp = fopen(file,"rb");
	if (!fp) 
		return 0;
	fseek (fp,0,SEEK_END);
  	size_t fSize = ftell(fp);
	if(fSize > (m->memorySize - offset))
	{
		fclose(fp);
		return 0;
	}
  	rewind(fp);
	fread(m->memory + offset, sizeof(char), fSize, fp);
	fclose(fp);
	return 1;
}

// Free our CPU
void freeMIPS(MIPS *m)
{
	free(m->memory);
	free(m);
}

inline unsigned char getOpcode(unsigned int instr)
{
	return (unsigned char) ((instr >> 26) & 0x3F);
}

inline unsigned char getRs(unsigned int instr)
{
	return (unsigned char) ((instr >> 21) & 0x1F);
}

inline unsigned char getRt(unsigned int instr)
{
	return (unsigned char) ((instr >> 16) & 0x1F);
}

inline unsigned char getRd(unsigned int instr)
{
	return (unsigned char) ((instr >> 11) & 0x1F);
}

inline unsigned int getJumpAddr(unsigned int instr)
{
	return (instr & 0x3FFFFFF);
}

inline short getImmediate(unsigned int instr)
{
	return (short)(instr & 0xFFFF);
}

inline unsigned char getShamt(unsigned int instr)
{
	return ((instr >> 6) & 0x1F);
}

inline unsigned char getFunc(unsigned int instr)
{
	return (instr & 0x3F);
}

inline unsigned int halfSwap(unsigned int word)
{
	#ifdef MIPS_BIG_ENDIAN
	return (word << 16) + (word >> 16);
	#else
	return word;
	#endif
}

inline unsigned char leastSignificantByte(unsigned int word)
{
	#ifdef MIPS_BIG_ENDIAN
	return (unsigned char)(word >> 24);
	#else
	return (unsigned char)word;
	#endif
}

inline unsigned short leastSignificantHalf(unsigned int word)
{
	#ifdef MIPS_BIG_ENDIAN
	return (unsigned short)(word >> 16);
	#else
	return (unsigned short)word;
	#endif
}

// Emulate 1 cycle of our CPU
unsigned int emulate(MIPS *m)
{
	int bPC = byteSwap(m->pc);

	// instruction out of range
	if(bPC > (m->memorySize - 4))
	{
		printf("Instruction out of range.  PC = %i\n",byteSwap(m->pc));
		return -1;
	}

	unsigned int instr = byteSwap(*((unsigned int*)((char *)m->memory + bPC)));
	unsigned char opcode = getOpcode(instr);

	
	m->pc = byteSwap(bPC + 4);

	m->registers[0] = 0;

	int rsAddr = getRs(instr);
	int rtAddr = getRt(instr);
	int rs = m->registers[rsAddr];
	int rt = m->registers[rtAddr];

	unsigned char fcode;
	int rdAddr;
	unsigned int swappedRs;


	switch(opcode)
	{
		case OPCODE_R:
			fcode = getFunc(instr);
			rdAddr = getRd(instr);
									
			switch(fcode)
			{
				case FCODE_ADD:
					m->registers[rdAddr] = byteSwap(byteSwap(rs) + byteSwap(rt));
					// trigger overflow interrupt?
				break;
				case FCODE_ADDU:
					m->registers[rdAddr] = byteSwap(byteSwap(rs) + byteSwap(rt));

				break;
				case FCODE_AND:
					m->registers[rdAddr] = rs & rt;
				break;
				case FCODE_BREAK:
				break;
				case FCODE_DIV:
					m->lo = byteSwap(byteSwap(rs) / byteSwap(rt));
					m->hi = byteSwap(byteSwap(rs) % byteSwap(rt));

					// trigger overflow interrupt?
				break;
				case FCODE_DIVU:
					m->lo = byteSwap(byteSwap(rs) / byteSwap(rt));
					m->hi = byteSwap(byteSwap(rs) % byteSwap(rt));

				break;
				case FCODE_JALR:
					// save PC
					m->registers[31] = m->pc;
					m->pc = rs;
				break;
				case FCODE_JR:
					m->pc = rs;
				break;
				case FCODE_MFHI:
					m->registers[rdAddr] = m->hi;
				break;
				case FCODE_MFLO:
					m->registers[rdAddr] = m->lo;
				break;
				case FCODE_MTHI:
					m->hi = rs;
				break;
				case FCODE_MTLO:
					m->lo = rs;
				break;
				case FCODE_MULT:
					m->lo =  byteSwap(byteSwap(rs) * byteSwap(rt));
					// trigger overflow interrupt?
				break;
				case FCODE_MULTU:
					m->lo =  byteSwap(byteSwap(rs) * byteSwap(rt));
				break;
				case FCODE_NOR:
					m->registers[rdAddr] = ~(rs | rt);
				break;
				case FCODE_OR:
					m->registers[rdAddr] = rs | rt;
				break;
				case FCODE_SLL:
					m->registers[rdAddr] = byteSwap(byteSwap(rt) << getShamt(instr)); 
				break;
				case FCODE_SLLV:
					m->registers[rdAddr] = byteSwap(byteSwap(rt) << byteSwap(rs));
				break;
				case FCODE_SLT:
					m->registers[rdAddr] = byteSwap((unsigned int)((int)byteSwap(rs) < (int)byteSwap(rt)));
				break;
				case FCODE_SLTU:
					m->registers[rdAddr] = byteSwap((byteSwap(rs) < byteSwap(rt)));
				break;
				case FCODE_SRA:
					m->registers[rdAddr] = byteSwap((int)byteSwap(rt) >> (int)getShamt(instr)); 
				break;
				case FCODE_SRAV:
					m->registers[rdAddr] = byteSwap((int)byteSwap(rt) >> (int)byteSwap(rs)); 
				break;
				case FCODE_SRL:
					m->registers[rdAddr] = byteSwap(byteSwap(rt) >> getShamt(instr)); 
				break;
				case FCODE_SRLV:
					m->registers[rdAddr] = byteSwap(byteSwap(rt) >> byteSwap(rs));
				break;
				case FCODE_SUB:
					// overflow exception?
					m->registers[rdAddr] = byteSwap(byteSwap(rs) - byteSwap(rt));
				break;
				case FCODE_SUBU:
					m->registers[rdAddr] = byteSwap(byteSwap(rs) - byteSwap(rt));
				break;
				case FCODE_SYSCALL:
				break;
				case FCODE_XOR:
					m->registers[rdAddr] = rs ^ rt;
				break;
			}
		break;

		case OPCODE_ADDI:
			m->registers[rtAddr] = byteSwap(byteSwap(rs) + getImmediate(instr));
			// trigger overflow interrupt?
		break;
		case OPCODE_ADDIU:
			m->registers[rtAddr] = byteSwap(byteSwap(rs) + getImmediate(instr));
		break;
		case OPCODE_ANDI:
			m->registers[rtAddr] = rs & byteSwap(getImmediate(instr));
		break;
		case OPCODE_BEQ:
			// no branch delay slot
			if(rs == rt)
			{
				m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
			}
		break;
		case OPCODE_BGEZ_BLTZ:
			// BGEZ
			if(rtAddr == 1)
			{
				if((int)byteSwap(rs) >= 0)
					m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
			}
			// BLTZ
			else if(rtAddr == 0)
			{
				if((int)byteSwap(rs) < 0)
					m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
			}
		break;
		case OPCODE_BGTZ:
			if((int)byteSwap(rs) > 0)
				m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
		break;
		case OPCODE_BLEZ:
			if((int)byteSwap(rs) <= 0)
				m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
		break;
		case OPCODE_BNE:
			if(rs != rt)
				m->pc = byteSwap((unsigned int)(((int)byteSwap(m->pc) +  (((int)(getImmediate(instr)))<<2))));
		break;
		case OPCODE_LB:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			int byteSignExtend = (int)((char *)m->memory)[swappedRs];
			m->registers[rtAddr] = byteSwap((unsigned int)byteSignExtend);
		break;
		case OPCODE_LBU:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			// no sign extension (unsigned -> unsigned casting)
			m->registers[rtAddr] = byteSwap((unsigned int)((unsigned char *)m->memory)[swappedRs]);
		break;
		case OPCODE_LH:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			int halfSignExtend = (int)*((short *)((char *)m->memory+swappedRs));
			m->registers[rtAddr] = halfSwap((unsigned int)halfSignExtend);
		break;
		case OPCODE_LHU:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			m->registers[rtAddr] = halfSwap((unsigned int)*((unsigned short *)((char *)m->memory+swappedRs)));
		break;
		case OPCODE_LUI:
			m->registers[rtAddr] &= byteSwap(getImmediate(instr)<<16);
		break;
		case OPCODE_LW:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			
			// memory mapped IO - massive h4x, but this works
			// problem - is that these addresses overlap!
			// thus we need to have only byte 0xFD and nothing else
			// 0xFD - mouse button
			if(swappedRs == m->ptrArrowKeys) 
				m->registers[rtAddr] = byteSwap(m->arrowKeys);
			// 0xFC - mouse y coordinate
			else if(swappedRs == 0xFC)
				m->registers[rtAddr] = byteSwap(m->mouseY);
			// 0xFB - mouse x coordinate
			else if(swappedRs == 0xFB)
				m->registers[rtAddr] = byteSwap(m->mouseX);
			else
				m->registers[rtAddr] = *((unsigned int*)((char *)m->memory+swappedRs));
		break;
		//case OPCODE_LWCL:
		//break;
		case OPCODE_ORI:
			m->registers[rtAddr] = rs | byteSwap((unsigned short)getImmediate(instr));
		break;
		case OPCODE_SB:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			((unsigned char *)m->memory)[swappedRs] = leastSignificantByte(rt);
		break;
		case OPCODE_SLTI:
			m->registers[rtAddr] = byteSwap((unsigned int)((int)byteSwap(rs) < (int)getImmediate(instr)));
		break;
		case OPCODE_SLTIU:
			m->registers[rtAddr] = byteSwap(byteSwap(rs) < getImmediate(instr));
		break;
		case OPCODE_SH:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			*((unsigned short *)((char *)m->memory+swappedRs)) = leastSignificantHalf(rt);
		break;
		case OPCODE_SW:
			// no virtual memory implementation...yet!
			// fault - return 0 if we're out of range
			swappedRs = (int)byteSwap(rs)+(int)getImmediate(instr);
			if(swappedRs > m->memorySize) return 0;
			*((unsigned int *)((char *)m->memory+swappedRs)) = rt;
			if(swappedRs == m->ptrGFX)
				m->mmapGFX = 1;
		break;
		//case OPCODE_SWCL:
		//break;
		case OPCODE_XORI:
			m->registers[rtAddr] = rs ^ byteSwap(getImmediate(instr));
		break;
		case OPCODE_J:
			m->pc = byteSwap(m->codeOffset + ((byteSwap(m->pc) & 0xf0000000) | (getJumpAddr(instr) << 2)));
		break;
		case OPCODE_JAL:
			m->registers[31] = m->pc;
			m->pc = byteSwap(m->codeOffset + ((byteSwap(m->pc) & 0xf0000000) | (getJumpAddr(instr) << 2)));
		break;
	}	

	/*
	printf("a0: %i\n", byteSwap(m->registers[4]));

	printf("%i %i %i\n",byteSwap(m->registers[8]),byteSwap(m->registers[9]),byteSwap(m->registers[10]));*/

	return 1;
}
