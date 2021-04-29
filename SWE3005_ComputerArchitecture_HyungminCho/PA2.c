//reg0 손대려고하면 오류나야함.
//점프할 때 주소 값이 - 가 나오면 오류 나야함.
//그 다들 sw sb sh 할때 immediate 음수인 경우 처리하셨나요?
//Lui를 언사인드로 했어요
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int Data;
unsigned int Instruction = 0;
unsigned int PC = 0;
unsigned int Reg[32] = {
	0,
};
unsigned int Inst_Memory[64001];
//이쪽 데이터 메모리
unsigned int Data_Memory[64001];

Data Inst_31to26 = 0;
Data Inst_25to21 = 0;
Data Inst_20to16 = 0;
Data Inst_15to11 = 0;
Data Inst_15to00 = 0;
Data Inst_05to00 = 0;
Data Inst_25to00 = 0;

Data isZero = 0;
Data ALUResult = 0;
Data ALUControl = 0;

unsigned int Jump = 0;
unsigned int RegDst = 0;   //
unsigned int ALUSrc = 0;   //
unsigned int MemtoReg = 0; //
unsigned int RegWrite = 0; //
unsigned int MemRead = 0;  //
unsigned int MemWrite = 0; //
unsigned int Branch = 0;   //
unsigned int ALUOP1 = 0;   //
unsigned int ALUOP0 = 0;   //

Data ReadData = 0;
Data ReadData1 = 0;
Data ReadData2 = 0;
/*
Arithmetic/logical: add, sub, and, or, slt ,

Arithmetic/logical with immediate: addi , andi , ori , lui , slti

Memory access: lw , sw , lh lhu , sh , lb , lbu , sb

Control transfer: beq , bne , j

Shift instructions: sll , srl
*/

void setControlUnit(unsigned int OPcode);
void setRegisters(Data RegWrite, Data ReadRegister1, Data ReadRegister2, Data WriteRegister, Data WriteData);
void fetchInstruction(Data Inst);
void setALU(Data Se_Inst15to00);
void setAdressFunc();
Data SignExtention(Data input);
void setALUControl();
void setDataMemoryFunc();
void errorOccured();
Data complement(Data);

int main()
{

	//Initializing
	for (int i = 0; i < 64001; i++)
	{
		Inst_Memory[i] = 0xFF;
		Data_Memory[i] = 0xFF;
	}
}

/*
Arithmetic/logical: add, sub, and, or, slt ,

Arithmetic/logical with immediate: addi , andi , ori , lui , slti

Memory access: lw , sw , lh lhu , sh , lb , lbu , sb

Control transfer: beq , bne , j

Shift instructions: sll , srl
*/

void setControlUnit(unsigned int OPcode)
{
	unsigned int op0, op1, op2, op3, op4, op5, comb0, comb1, comb2, comb3;
	op5 = (OPcode & 0b100000) >> 5;
	op4 = (OPcode & 0b010000) >> 4;
	op3 = (OPcode & 0b001000) >> 3;
	op2 = (OPcode & 0b000100) >> 2;
	op1 = (OPcode & 0b000010) >> 1;
	op0 = (OPcode & 0b000001) >> 0;

	comb0 = !(op0 | op1 | op2 | op3 | op4 | op5);
	comb1 = (op5 & !op4 & !op3 & !op2 & op1 & op0);
	comb2 = (op5 & !op4 & op3 & !op2 & op1 & op0);
	comb3 = (!op5 & !op4 & !op3 & op2 & !op1 & !op0);

	Jump = !op5 & !op4 & !op3 & !op2 & op1 & !op0;
	RegDst = comb0;
	ALUSrc = comb1 | comb2;
	MemtoReg = comb1;
	RegWrite = comb0 | comb1;
	MemRead = comb1;
	MemWrite = comb2;
	Branch = comb3;
	ALUOP1 = comb1;
	ALUOP0 = comb3;
}

void setRegisters(Data RegWrite, Data ReadRegister1, Data ReadRegister2, Data WriteRegister, Data WriteData)
{
	ReadData1 = Data_Memory[ReadRegister1];
	ReadData2 = Data_Memory[ReadRegister2];

	//이 사이에 다른 펑션들이 돌아가야함. ALU DATAMEMORY등

	WriteData = MemWrite ? ReadData : ALUResult;

	if (RegWrite)
	{
		Data_Memory[WriteRegister] = WriteData;
	}
}

//0000 0000 0000 0000 0000 0000 0000 0000
void fetchInstruction(Data Inst)
{
	Instruction = Inst;
	Inst_31to26 = (Inst & 0b11111100000000000000000000000000) >> 26;
	Inst_25to21 = (Inst & 0b00000011111000000000000000000000) >> 21;
	Inst_20to16 = (Inst & 0b00000000000111110000000000000000) >> 16;
	Inst_15to11 = (Inst & 0b00000000000000001111100000000000) >> 11;
	Inst_15to00 = Inst & 0b000000000000000011111111111111111;
	Inst_05to00 = Inst & 0b111111;
	Inst_25to00 = Inst & 0b11111111111111111111111111;
}

void setALU(Data Se_Inst15to00)
{
	Data input1, input2;
	input1 = ReadData1;
	input2 = ALUSrc ? Se_Inst15to00 : ReadData2;

	if (input1 == input2)
		isZero = 1;
	else
		isZero = 0;

	Data Bin_Inst = Instruction;
	//다양한 작업
	//

	Data op, rs, rt, rd, shamt, funct, const_addr_16, address_26;
	//op코드를 통해 타입 구분해주고 타입 구분이   _ 북마크에 다 정리해놨으니 https://www.slideshare.net/tagbagtroj/mips-opcodes 참조
	//끝나면 가장 우선해 오류 체크부터 해준다.
	op = (Bin_Inst & 0b11111100000000000000000000000000) >> 26;

	if (op == 0) //R-Type
	{
		rs = (Bin_Inst & 0b00000011111000000000000000000000) >> 21;
		rt = (Bin_Inst & 0b00000000000111110000000000000000) >> 16;
		rd = (Bin_Inst & 0b00000000000000001111100000000000) >> 11;
		shamt = (Bin_Inst & 0b00000000000000000000011111000000) >> 6;
		funct = (Bin_Inst & 0b00000000000000000000000000111111);

		if (funct == 0b000000)
		{ //finalcheck
			printf("sll $%d, $%d, %d\n", rd, rt, shamt);
		}
		else if (funct == 0b000010)
		{ //finalcheck
			printf("srl $%d, $%d, %d\n", rd, rt, shamt);
		}


		else if (funct == 0b100000)
		{ //finalcheck
			printf("add $%d, $%d, $%d\n", rd, rs, rt);
			ALUResult=input1+input2;
		}

		else if (funct == 0b100010)
		{ //finalcheck
			printf("sub $%d, $%d, $%d\n", rd, rs, rt);
			ALUResult=input1+complement(input2);
		}

		else if (funct == 0b100100)
		{ //finalcheck
			printf("and $%d, $%d, $%d\n", rd, rs, rt);
			ALUResult=input1&input2;
		}
		else if (funct == 0b100101)
		{ //finalcheck
			printf("or $%d, $%d, $%d\n", rd, rs, rt);
			ALUResult=input1|input2;
		}

		else if (funct == 0b101010)
		{ //finalcheck
			printf("slt $%d, $%d, $%d\n", rd, rs, rt);
			ALUResult=input1<input2?1:0;
		}

		else
		{
			printf("unknown instruction\n");
		} //오류 생성}
	}
	else //not R_type
	{
		rs = (Bin_Inst & 0b00000011111000000000000000000000) >> 21;
		rt = (Bin_Inst & 0b00000000000111110000000000000000) >> 16;
		const_addr_16 = (Bin_Inst & 0b00000000000000001111111111111111);
		address_26 = (Bin_Inst & 0b00000011111111111111111111111111);
		//ALU Result만 갱신해주면 됨

		//load store
		if (op == 0b100000)
		{ //finalcheck
			printf("lb $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}
		else if (op == 0b100001)
		{ //finalcheck
			printf("lh $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}
		else if (op == 0b100011)
		{ //finalcheck
			printf("lw $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}
		else if (op == 0b100100)
		{																		 //finalcheck
			printf("lbu $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs); //update17
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}																		 //여기 띄어쓰기
		else if (op == 0b100101)
		{																		 //finalcheck
			printf("lhu $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs); //update17
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}																		 //여기 띄어쓰기
		else if (op == 0b101000)
		{ //finalcheck
			printf("sb $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}
		else if (op == 0b101001)
		{ //finalcheck
			printf("sh $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}
		else if (op == 0b101011)
		{ //finalcheck
			printf("sw $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
			ALUResult=SignExtention(Inst_15to00)*4+Data_Memory[rs];
		}

		//Itype
		else if (op == 0b001000)
		{ //finalcheck
			printf("addi $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16));
			ALUResult=Data_Memory[rs]+SignExtention(Inst_15to00);
		}
		else if (op == 0b001010)
		{																		  //finalcheck
			printf("slti $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16)); //update17
			ALUResult=
		}

		else if (op == 0b001100)
		{ //finalcheck
			printf("andi $%d, $%d, %d\n", rt, rs, const_addr_16);
			ALUResult=Data_Memory[rs]&SignExtention(Inst_15to00);
		}
		else if (op == 0b001101)
		{ //finalcheck
			printf("ori $%d, $%d, %d\n", rt, rs, const_addr_16);
			ALUResult=Data_Memory[rs]|SignExtention(Inst_15to00);
		}

		else if (op == 0b001111)
		{																//finalcheck
			printf("lui $%d, %d\n", rt, makeSigned(const_addr_16, 16)); //잻 악수탠투
			ALUResult=
		}

		//제로 익스텐드 imm만 주의
		else if (op == 0b000010)
		{ //finalcheck
			printf("j $%d, $%d\n", rt, address_26);
			ALUResult=
		}

		else if (op == 0b000100)
		{ //finalcheck
			printf("beq $%d, $%d, %d\n", rs, rt, makeSigned(const_addr_16, 16));
			ALUResult=
		}
		else if (op == 0b000101)
		{ //finalcheck
			printf("bne $%d, $%d, %d\n", rs, rt, makeSigned(const_addr_16, 16));
			ALUResult=
		}

		else
		{
			printf("unkwon instruction\n");
		}
	}
}

void setDataMemoryFunc()
{
	Data Adress, Wdata;
	Adress = ALUResult;
	Wdata = ReadData2;
	//로드 스토어 하는 게 W,B,H인지 잘 구분해야함!
	if (MemRead)
	{
		ReadData = Data_Memory[Adress];
	}

	if (MemWrite)
	{
		Data_Memory[Adress] = Wdata;
	}
}

void setAdressFunc()
{
	PC = PC + 4;
	Data ALUaddr;
	Data Addforjump = 0;
	Data resultMux1;
	Data resultMux2; 
	Addforjump += Inst_25to00 << 2;
	Addforjump += PC & 0b11110000000000000000000000000000;

	if ((int)SignExtention(Inst_15to00) < 0)
	{
		if (PC >= Inst_15to00)
			ALUaddr = PC - Inst_15to00;
		//else //메모리 참조 오류 난다.
	}
	else
	{
		ALUaddr = PC + Inst_15to00;
	}

	//mux1
	if (Branch && isZero) //beq ok
		resultMux1 = ALUaddr;
	else
		resultMux1 = PC;

	//mux2
	if (Jump)
		resultMux2 = Addforjump;
	else
		resultMux2 = resultMux1;

	PC = resultMux1;
}

Data SignExtention(Data input)
{
	Data signbit = input >> 15;
	Data ret = 0;
	if (signbit) //sign=1
	{
		ret = input | 0xffff0000;
	}
	else
	{
		ret = input;
	}
	return ret;
}

void setALUControl()
{

	//I타입은 어쩌라고?

	Data ALUop = ALUOP1 << 1 + ALUOP0;
	if (ALUop == 0b00) //lw,sw
	{
		ALUControl = 0b0010;
	}
	else if (ALUop == 0b01) //beq
	{
		ALUControl = 0b0110;
	}
	else if (ALUop == 0b10)
	{
		Data func = Instruction & 0b111111;
		if (func == 0b100000) //add
			ALUControl = 0b0010;
		else if (func == 0b100010) //subtract
			ALUControl = 0b0110;
		else if (func == 0b100100) //AND
			ALUControl = 0b0000;
		else if (func == 0b100101) //OR
			ALUControl = 0b0001;
		else if (func == 0b101010) //set on less than
			ALUControl = 0b0111;
		else //error
			errorOccured();
	}
	else //에러 체크
		errorOccured();
}

void errorOccured()
{
	printf("ERROR!");
	exit(1);
}

Data complement(Data input)
{
	return 1+~input;
}