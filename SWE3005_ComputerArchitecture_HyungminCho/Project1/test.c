#include <stdio.h>
#include <stdlib.h> // exit() 함수를 위해

#define MAX_COLS 32768

void convertEndian(unsigned int *);
void encodeMipsInst(unsigned int);
void opRType(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);


int makeSigned(int, int);
//R-Type  | op(6) | rs(5) | rt(5) | rd(5) | shamt(5) | funct(6) |
//I-Type  | op(6) | rs(5) | rt(5) |     cont or address(16)     |
//J-Type  | op(6) |                address(26)                  |
//32개 0000 0000 0000 0000 0000 0000 0000 0000

int main(int argc, char *argv[])
{
  FILE *in;
  char s[MAX_COLS]; // 행이 1줄씩 임시로 저장될 버퍼
  unsigned int input[1];

  if (argc == 1)
  { //인풋이 0
    fputs("\nHelp: program_name <file_name>\n", stderr);
    exit(1);
  }

  in = fopen(argv[1], "rb");

  unsigned int i = 0;
  while (1)
  {

    fread(input, sizeof(int), 1, in); //8bit씩 읽음
    if (feof(in))
      break;
    printf("inst %d: ", i);
    i++;
    convertEndian(input);

    printf("%08x ", input[0]);
    encodeMipsInst(input[0]);
  }

  return 0;
}

void convertEndian(unsigned int *input)
{
  unsigned int Little_Endian, Big_Endian = 0;
  unsigned int temp[4] = {0};

  Little_Endian = input[0];

  //각 파트마다 마스킹해서 바꿈
  //masking

  for (int i = 0; i < 4; i++)
  {
    temp[i] = (Little_Endian >> (8 * i)) & 0b11111111;
  }

  for (int i = 0; i < 4; i++)
  {
    Big_Endian += temp[i] << (24 - 8 * i);
  }
  input[0] = Big_Endian;
}

//R-Type  | op(6) | rs(5) | rt(5) | rd(5) | shamt(5) | funct(6) |
//I-Type  | op(6) | rs(5) | rt(5) |     cont or address(16)     |
//J-Type  | op(6) |                address(26)                  |
//32개 0b0000 0000 0000 0000 0000 0000 0000 0000
//32개 R 0b000000 00000 00000 00000 00000 000000
//32개 I 0b000000 00000 00000 0000000000000000
//32개 J 0b000000 00000000000000000000000000
void encodeMipsInst(unsigned int Bin_Inst)
{
  int op, rs, rt, rd, shamt, funct, const_addr_16, address_26;
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
    opRType(op, rs, rt, rd, shamt, funct);
  }
  else //not R_type
  {
    rs = (Bin_Inst & 0b00000011111000000000000000000000) >> 21;
    rt = (Bin_Inst & 0b00000000000111110000000000000000) >> 16;
    const_addr_16 = (Bin_Inst & 0b00000000000000001111111111111111);
    address_26 = (Bin_Inst & 0b00000011111111111111111111111111);

    //load store
    if (op == 0b100000)
    {//finalcheck
      printf("lb $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }
    else if (op == 0b100001)
    {//finalcheck
      printf("lh $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }
    else if (op == 0b100011)
    {//finalcheck
      printf("lw $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }
    else if (op == 0b100100)
    {//finalcheck
      printf("lbu $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16),rs); //update17
    }//여기 띄어쓰기
    else if (op == 0b100101)
    {//finalcheck
      printf("lhu $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16),rs); //update17
    }//여기 띄어쓰기
    else if (op == 0b101000)
    {//finalcheck
      printf("sb $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }
    else if (op == 0b101001)
    {//finalcheck
      printf("sh $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }
    else if (op == 0b101011)
    {//finalcheck
      printf("sw $%d, %d($%d)\n", rt, makeSigned(const_addr_16, 16), rs);
    }

    //Itype
    else if (op == 0b001000)
    {//finalcheck
      printf("addi $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16));
    }
    else if (op == 0b001001)
    {//finalcheck
      printf("addiu $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16)); //update17
    }
    else if (op == 0b001010)
    {//finalcheck
      printf("slti $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16)); //update17
    }
    else if (op == 0b001011)
    {//finalcheck
      printf("sltiu $%d, $%d, %d\n", rt, rs, makeSigned(const_addr_16, 16)); //update17
    }
    else if (op == 0b001100)
    {//finalcheck
      printf("andi $%d, $%d, %d\n", rt, rs, const_addr_16);
    }
    else if (op == 0b001101)
    {//finalcheck
      printf("ori $%d, $%d, %d\n", rt, rs, const_addr_16);
    }
    else if (op == 0b001110)
    {//finalcheck
      printf("xori $%d, $%d, %d\n", rt, rs, const_addr_16);
    }
    else if (op == 0b001111)
    {//finalcheck
      printf("lui $%d, %d\n", rt, makeSigned(const_addr_16, 16)); //잻 악수탠투
    }

    //제로 익스텐드 imm만 주의
    else if (op == 0b000010)
    {//finalcheck
      printf("j $%d, $%d\n", rt, address_26);
    }
    else if (op == 0b000011)
    {//finalcheck
      printf("jal %d\n", address_26);
    }
    else if (op == 0b000100)
    {//finalcheck
      printf("beq $%d, $%d, %d\n", rs, rt, makeSigned(const_addr_16, 16));
    }
    else if (op == 0b000101)
    {//finalcheck
      printf("bne $%d, $%d, %d\n", rs, rt, makeSigned(const_addr_16, 16));
    }

    else
    {
      printf("unkwon instruction\n");
    }
  }
}

//R-Type  | op(6) | rs(5) | rt(5) | rd(5) | shamt(5) | funct(6) |
//I-Type  | op(6) | rs(5) | rt(5) |     const or address(16)     |
//J-Type  | op(6) |                address(26)                  |
void opRType(unsigned int op, unsigned int rs, unsigned int rt, unsigned int rd, unsigned int shamt, unsigned int funct)
{
  if (funct == 0b000000)
  {//finalcheck
    printf("sll $%d, $%d, %d\n", rd, rt, shamt);
  } 
  else if (funct == 0b000010)
  {//finalcheck
    printf("srl $%d, $%d, %d\n", rd, rt, shamt);
  } 
  else if (funct == 0b000011)
  {//finalcheck
    printf("sra $%d, $%d, %d\n", rd, rt, shamt);
  } 
  else if (funct == 0b000100)
  {//finalcheck
    printf("sllv $%d, $%d, $%d\n", rd, rt, rs);
  } 
  else if (funct == 0b000110)
  {//finalcheck
    printf("srlv $%d, $%d, $%d\n", rd, rt, rs);
  } 
  else if (funct == 0b000111)
  {//finalcheck
    printf("srav $%d, $%d, $%d\n", rd, rt, rs);
  } 
  else if (funct == 0b001000)
  {//finalcheck
    printf("jr $%d\n", rs);
  } 
  else if (funct == 0b001001)
  {//finalcheck
    printf("jalr $%d, $%d\n", rd, rs);
  } 
  else if (funct == 0b001100)
  {//finalcheck
    printf("syscall\n");
  } 
    
  else if (funct == 0b010000)
  {//finalcheck
    printf("mfhi $%d\n", rd);
  } 
  else if (funct == 0b010001)
  {//finalcheck
    printf("mthi $%d\n", rs);
  } 
  else if (funct == 0b010010)
  {//finalcheck
    printf("mflo $%d\n", rd);
  } 
  else if (funct == 0b010011)
  {//finalcheck
    printf("mtlo $%d\n", rs);
  } 
  else if (funct == 0b011000)
  {//finalcheck
    printf("mult $%d, $%d\n", rs, rt);
  } 
  else if (funct == 0b011001)
  {//finalcheck
    printf("multu $%d, $%d\n", rs, rt);
  } 
  else if (funct == 0b011010)
  {//finalcheck
    printf("div $%d, $%d\n", rs, rt);
  }
  else if (funct == 0b011011)
  {//finalcheck
    printf("divu $%d, $%d\n", rs, rt);
  } 
  else if (funct == 0b100000)
  {//finalcheck
    printf("add $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100001)
  {//finalcheck
    printf("addu $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100010)
  {//finalcheck
    printf("sub $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100011)
  {//finalcheck
    printf("subu $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100100)
  {//finalcheck
    printf("and $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100101)
  {//finalcheck
    printf("or $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100110)
  {//finalcheck
    printf("xor $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b100111)
  {//finalcheck
    printf("nor $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b101010)
  {//finalcheck
    printf("slt $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else if (funct == 0b101011)
  {//finalcheck
    printf("sltu $%d, $%d, $%d\n", rd, rs, rt);
  } 
  else
  {
    printf("unknown instruction\n");
  } //오류 생성}

  //TODO :: 북마크에 있는 op코드표 참고해서 최종 점검 잘 하기!
}


int makeSigned(int us, int digit) //16비트짜리
{
  int ret;
  //양수이면 그대로 return

  if ((us & 0b1000000000000000) == 0)
  {
    ret = us & 0b00000000000000001111111111111111;
    return ret;
  }
  else
  {
    us = us & 0b00000000000000001111111111111111;
    us = ~us;
    us++;
    ret = (us & 0b00000000000000001111111111111111) * -1;
    return ret;
  }
}