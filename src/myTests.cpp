#include <gtest/gtest.h>
#include "../include/emu4380.h"

TEST(FetchTest, FetchNormalOperation){
	int size = 131072;
	prog_mem = new unsigned char[size];
	reg_file[RegNames::PC] = 10;

	EXPECT_TRUE(fetch());

	delete[] prog_mem;
}

TEST(FetchTest, FetchAtMemoryBoundary){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 8;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchBeyondMemoryBoundary){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchWithInvalidPC){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size + 10;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}


TEST(FetchTest, FetchAtMemoryLimit){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = 10;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchOverMemoryLimit){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}


TEST(DecodeTest, ValidOperation){
	cntrl_regs[CntrlRegNames::OPERATION] = 7; 
  cntrl_regs[CntrlRegNames::OPERAND_1] = 1; 
  cntrl_regs[CntrlRegNames::OPERAND_2] = 2;

	EXPECT_TRUE(decode());
}

TEST(DecodeTest, InvalidOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 99;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, InvalidOperand){
	cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_1] = 99;
  cntrl_regs[CntrlRegNames::OPERAND_2] = 2;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, ImmediateAssignment){
	cntrl_regs[CntrlRegNames::OPERATION] = 8;
  cntrl_regs[CntrlRegNames::OPERAND_1] = 1;
  cntrl_regs[CntrlRegNames::IMMEDIATE] = 12345;
  
	EXPECT_TRUE(decode());
	EXPECT_EQ(data_regs[1], 12345);
}

TEST(DecodeTest, OperandEdgeValues){
  cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_1] = 0;
  cntrl_regs[CntrlRegNames::OPERAND_2] = num_gen_regs - 1;

  EXPECT_TRUE(decode());
}

TEST(DecodeTest, DecodeWithoutFetching){
  cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_2] = 1;

	decode();
	EXPECT_TRUE(decode());
}

TEST(DecodeTest, UninitialzedCntrlRegisters){
	for(int i = 0; i < num_cntrl_regs; i++){
		cntrl_regs[i] = 0xFFFFFFFF;
	}

  EXPECT_FALSE(decode());
}

TEST(ExecuteTest, AddOperation){
	cntrl_regs[CntrlRegNames::OPERATION] = 18;
	data_regs[0] = 1;
	data_regs[1] = 2;
	data_regs[2] = 3;
	reg_file[2] = 5;
	reg_file[3] = 10;

	EXPECT_TRUE(execute());
	EXPECT_EQ(reg_file[1], 15);
}

TEST(ExecuteTest, SubOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 20;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 5);
}

TEST(ExecuteTest, MulOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 22;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 50);
}

TEST(ExecuteTest, DivOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 24;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 2);
}

TEST(ExecuteTest, StoreMemoryLimit){
	int size = 131072;
	cntrl_regs[CntrlRegNames::OPERATION] = 10;
	data_regs[0] = 1;
	data_regs[1] = size + 1;
	reg_file[1] = 12345;

	EXPECT_FALSE(execute());
}

TEST(ExecuteTest, LoadOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 11;
  data_regs[0] = 1;
  data_regs[1] = 100;
  auto* address = reinterpret_cast<unsigned int*>(prog_mem + 100);
	*address = 54321;

  EXPECT_TRUE(execute());
	EXPECT_EQ(reg_file[1], 54321);
}

TEST(ExecuteTest, DivByZero){
  cntrl_regs[CntrlRegNames::OPERATION] = 24;
  data_regs[0] = 1;
  data_regs[1] = 2;
	data_regs[2] = 3;
	reg_file[2] = 50;
	reg_file[3] = 0;	

  EXPECT_FALSE(execute());
}

TEST(FetchFunction, PopulatesControlRegistersCorrectly) {
  int size = 131072;
  prog_mem = new unsigned char[size];

  prog_mem[PC] = 0x01;
  prog_mem[PC + 1] = 0x02;
  prog_mem[PC + 2] = 0x03;
  prog_mem[PC + 3] = 0x04;
  // Set an immediate value (for simplicity, using 0x05060708)
  prog_mem[PC + 4] = 0x08;
  prog_mem[PC + 5] = 0x07;
  prog_mem[PC + 6] = 0x06;
  prog_mem[PC + 7] = 0x05;

  reg_file[RegNames::PC] = 0; // Starting PC

  bool success = fetch();

  ASSERT_TRUE(success);
  EXPECT_EQ(cntrl_regs[CntrlRegNames::OPERATION], 0x01); //FAILING
  EXPECT_EQ(cntrl_regs[CntrlRegNames::OPERAND_1], 0x02); //FAILING
  EXPECT_EQ(cntrl_regs[CntrlRegNames::OPERAND_2], 0x03); //FAILING
  EXPECT_EQ(cntrl_regs[CntrlRegNames::OPERAND_3], 0x04); //FAILING
  unsigned int expectedImmediate = 0x08070605; // Adjust based on your system's endianness
  //EXPECT_EQ(cntrl_regs[CntrlRegNames::IMMEDIATE], expectedImmediate);

  delete[] prog_mem;
}


int main(int argc, char **argv){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
