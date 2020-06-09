//
// Created by nhy20 on 2020-05-09.
//

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include "../typedef.h"
#include <vector>
#include <string>
class Chip8
{
public:
	Chip8();

	WORD getNextOpCode();


	BYTE getScreenData(int x, int y) { return mScreenData[y][x]; }
	WORD getProgramCounter( ) const { return mProgramCounter; }
	BYTE getRegisterValue( WORD index ) const { return mRegisters[index]; }
	WORD getAddressIndex() const { return mAddressIndex; }
	BYTE getRandomValue() const { return mRandomValue; }
	BYTE getMemoryValue(BYTE index){ return mGameMemory[index]; }
	BYTE getDelayTimer() { return mDelayTimer; }
	BYTE getSoundTimer() { return mSoundTimer; }

	BYTE waitInput();
	void addInput(BYTE input_code);
	void upInput(BYTE input_code);

	static BYTE * getFontSet();
	static int getFontSetLength() { return 80; }


	void setMemoryValue(BYTE index, BYTE value) { mGameMemory[index] = value; }
	void setAddressIndex(BYTE address_index) { mAddressIndex = address_index; }
	void setInjectionCounter(WORD counter) { mInjectionCounter = counter; }
	void setRegisterValue( BYTE index, BYTE value ) { mRegisters[index] = value; }

	std::vector<WORD>  getCallStack() const {return mStack;}

	void nextStep();
	void nextStep0xF(WORD opCode);
	void nextStep0x8(WORD opCode);

	void injectionCode( WORD code );
	void reset();
	void loadRom();

#ifdef _CHIP8_DISASM_BUILD
	void pushDisASMString(const std::string & code);
	bool isEOF() { return this->mIsEOF; }
	void createDisASMFile();
#endif
	/*
	 * OP Code는 4자리다. 0xFFFF 형식으로 되어 있기도 하고..
	 * 그래서 최대수의 AND 연산으로 각 자리수를 구해줄 수 있다. 이를태면..
	 * 0xABCD & 0xF000 = A. 아렇게.
	 */
	static inline WORD DecodeOpCodeFirst( WORD opcode ) { return opcode & 0xF000; }
	static inline WORD DecodeOpCodeSecond( WORD opcode )  { return opcode & 0x0F00; }
	static inline WORD DecodeOpCodeThird( WORD opcode ) { return opcode & 0x00F0; }
	static inline WORD DecodeOpCodeForth( WORD opcode ) { return opcode & 0x000F; }

	~Chip8();

private:
	void generateRandomValue();

private:
	//명령어를 실제로 수행하는 함수들.
	// !주의! 값이 저장되는  모든 연산의 '피연산자'는 좌항입니다!
	// EX )  V1 레지스터의 값을 V2에 넣는다면 -> LD V2 V1

	// [jump]
	// JP NNN. 실제로 사용되진 않음.
	void opCode0NNN(WORD opCode);

	// [code]
	// Clear the display.
	void opCode00E0(WORD opCode);

	// [code]
	// Return from a subroutine.
	//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
	void opCode00EE(WORD opCode);

	// [code]
	//JP(jump) addr ( 점프 addr. ) addr = NNN.
	void opCode1NNN(WORD opCode);

	// [code]
	//call addr. ( 호출 addr ). addr = NNN. stack에 현재 주소 push.
	void opCode2NNN(WORD opCode);

	// [if]
	//SE (Skip if Equal) Vx, Byte( 같으면_건너띄기 Vx Byte ).Vx =  X. Byte = KK.
	void opCode3XKK(WORD opCode);

	// [if]
	//SNE (Skip if not Equal) Vx, Byte (같지_않으면_건너띄기 Vx Byte )  Vx = X. Byte = KK,
	void opCode4XKK(WORD opCode);

	// [if]
	//SE (Skip if Equal ) Vx, Vy ( 같으면_건너띄기  Vx Vy ). Vx =  X, Vy = Y
	void opCode5XY0(WORD opCode);

	// [load]
	//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
	//LD는 Load의 약자다 =ㅁ=.
	void opCode6XKK(WORD opCode);

	// [math]
	//ADD Vx Byte. ( 더하기 Vx Byte ) Vx = X, Byte = NN
	void opCode7XKK(WORD opCode);

	// [load]
	//LD(LoaD) Vx, Vy. ( 로드 Vx Vy )
	void opCode8XY0(WORD opCode);

	// [bit]
	//OR Vx, Vy .  Vx = Vx | Vy. 비트 OR 연산자.
	void opCode8XY1(WORD opCode);

	// [bit]
	//AND Vx Vy .  Vx = Vx & Vy. 비트 AND 연산자 .
	void opCode8XY2(WORD opCode);

	// [bit]
	//XOR Vx Vy . Vx = Vx XOR Vy. 비트 XOR 연산자.
	void opCode8XY3(WORD opCode);

	// [math]
	//ADD Vx Vy.   Vx = Vx + Vy. Vf = Carry.
	//Vx와 Vy를 더하고. 결과가 8비트(255)가 넘는다면 Vf를 1로. 넘지 않으면 Vf 0으로 정합니다. Vx에는 나머지 8비트만 저장됩니다.
	void opCode8XY4(WORD opCode);


	// [math]
	//SUB (빼기)  Vx Vy. Vx = Vx - Vy . Vf = NOT borrow.
	//Vf를 Vx가 Vy보다 크면 1. 크지 않으면 0을 저장. Vx에 Vy를 뺀다.  Vx엔 결과가 저장된다.
	//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
	void opCode8XY5(WORD opCode);

	// [bit]
	//SHR (SHfit Right) Vx {, Vy } Vx = Vx SHR 1 (혹은 y.).
	//Vx의 최하위 비트가 1이면 VF는 1로 설정하고, 그렇지 않으면 0으로 설정. 그 다음, Vx는 2만큼 나눈다. ( 1비트씩 우로 밀렸으니까 )
	void opCode8XY6(WORD opCode);

	// [math]
	//SUBN (SUB Not ( 반대로 빼기 ) ) Vx, Vy. Vx = Vy - Vx. VF = NOT borrow.
	//Vy에 Vx를 빼고, Vf를 Vy가 Vx보다 크면 1. 크지 않으면 0을 저장. Vx엔 결과가 저장된다.
	void opCode8XY7(WORD opCode);

	// [bit]
	//SHL(SHift Left) Vx, {Vy}. Vx = Vx SHL 1.
	//Vx의 최하의 비트가 1이면 VF를 1로 설정. 아니라면 0. 그 후, Vx를 2만큼 곱해진다. ( 1비트씩 좌로 밀렸으니까 )
	void opCode8XYE(WORD opCode);

	// [if]
	//SNE ( Skip Not Equal ) Vx, Vy.
	//Vx와 Vy가 같지 않으면 다음 구문을 생략한다.
	void opCode9XY0(WORD opCode);

	// [code]
	//LD (LoaD) I, addr. register I(mAddressIndex) = NNN.
 	//레지스터 I의 값을 NNN으로 설정한다.
	void opCodeANNN(WORD opCode);

	// [code]
	//JP(JumP) V0, addr. PC = NNN + V0.
	//Program Count를 NNN + V0로 점프한다.
	void opCodeBNNN(WORD opCode);

	// [code]
	//RND(random) Vx, Byte(kk). Vx = random byte(255) AND kk.
 	//0에서 255까지의 값을 생성한 후, kk값으로 AND 연산을 한 결과를 Vx에 저장한다. AND에 관한 정보는 8xy2를 참조.
	void opCodeCXKK(WORD opCode);

	// [code]
	// DRW Vx, Vy, nibble. 메모리 레지스터 (mAddressIndex) I 에서 부터 N 바이트에 읽은 스프라이트를 (x = Vx, y = Vy) 에 표시하고. Vf = Collision 설정.
 	// 주소 I부터 N바이트에 있는 스프라이트를 부른 다음, ( Vx, Vy ) 좌표에 그린다. 그리는 방식은 기존 화면에 XOR.
 	// 만약 이로 인해 픽셀이 지워지면 Vf를 1로 설정. 아니면 0으로 설정.
 	// 스프라이트가 디스플레이의 외부로 위치하도록 좌표를 설정하면, 반대편 화면 ( 오른쪽에 배치하면 왼쪽에서 튀어나옴 ) 에서 나온다.
 	// XOR 명령은 8xy3 명령을 참조.
	void opCodeDXYN(WORD opCode);

	// [keyboard]
	// SKP( SKip if Pressed ) Vx.
	// 키보드 확인 후 키가 눌러 져있다면, PC 2 증가.
	void opCodeEX9E(WORD opCode);

	// [keyboard]
	// SKNP ( SKip if Not Pressed ) Vx.
	// 키보드 확인 후 키가 눌러져있지 않다면, PC 2 증가.
	void opCodeEXA1(WORD opCode);

	// [load]
	// LD Vx, DT. Delay Vx = DT.
	// Vx를 DT로 Set.
	void opCodeFX07(WORD opCode);

	// [load]
	// LD Vx, Key. Set Vx = Key.
	// 키 입력까지 대기 후, Vx에 키 값을 저장. 키를 누를 떄 까지 모든 행동을 중지.
	void opCodeFX0A(WORD opCode);

	// [load]
	// LD Vx, K. Delay Timer = Vx.
	// DT(Delay Timer)를 Vx로.
	void opCodeFX15(WORD opCode);

	// [load]
	// LD ST, Vx. Sound Timer = Vx.
	// ST(Sound Timer)를 Vx로.
	void opCodeFX18(WORD opCode);

	// [math]
	// ADD I, Vx. I = I + Vx.
	// register I의 값에 Vx를 더한다.
	void opCodeFX1E(WORD opCode);

	// [code]
	// Set F, V. I = Vx 값에 해당하는 숫자의 스프라이트 주소로.
	// 레지스터 I를 Vx값에 해당하는 16진수 스프라이트 위치로 설정.
	void opCodeFX29(WORD opCode);

	// [load]
	// LD B, Vx. I = BCD( Vx의 100자리 수. ) I + 1 = BCD( Vx의 10자리 수.) I + 2 = BCD( Vx의 1자리 수 ),
	// 메모리 위치 I, I + 1 및 I + 2에 Vx의 백~일의 자리수의 BCD 표현을 저장. ( BCD : 4비트로 10자리 수를 표현하는 기법. )
	void opCodeFX33(WORD opCode);

	// [load]
	// LD [I], Vx. Memory Address [I] to length of ( V0 to Vx ) = V0 to Vx.
	// V0부터 Vx에 해당하는 값들을 메모리 주소 I에 저장 ( 그럼 메모리 끝점은 Vx 만큼 이동하겠죠? )
	void opCodeFX55(WORD opCode);

	// [load]
	// LD Vx, [I]. V0 to Vx = Memory Address [I] to length of ( V0 to Vx ).
	// 메모리 주소 I부터 I + x에 해당하는 메모리 주소까지 불러온 다음, V0부터 Vx에 저장합니다.
	void opCodeFX65(WORD opCode);

private:
	void CPUReset();

	// CHIP 8 내부 메모리 & CPU등 설정.

/* CHIP 8 기기는 아래로 이루어져있다.
 *
    * Memory of 0xFFF bytes ( 0xFFF 바이트짜리 메모리. )
    * 16 8-Bit data registers, called V0 to VF. VF doubles as the carry flag ( 16개의 8비트 데이터 레지스터, V0~VF라고 불린다. )
    * 16 Bit address register I which is used to access memory ( 메모리에 접근할 때 사용하는 16비트 주소 레지스터. )
    * 16 Bit program counter and a stack ( 16 비트 프로그램 카운터 ( 코드 실행 위치를 기억하는 장치 )와 스택. )
 *
 */
private:
	BYTE mGameMemory[0xFFF]; // 메모리 : 0xFFF 바이트.
	BYTE mRegisters[16]; // Vx 16개의 레지스터들.
	BYTE mScreenData[32][64]; // y ,x  좌표계.
	BYTE mRandomValue ;

	BYTE mDelayTimer, mSoundTimer;

	WORD mAddressIndex; // 레지스터 I. 16비트 주소를 담는 변수.
	WORD mProgramCounter; // 16비트 프로그램 카운터.
	WORD mInjectionCounter; // 디버깅용 16비트 코드 인젝션 카운터.

	// 함수 호출시 리턴 위치를 알기 위해 위치를 기록해 놓는 콜 스택.
	std::vector<WORD> mStack;

#ifdef _CHIP8_DISASM_BUILD
	std::vector<std::string> codes;
	bool mIsEOF = false;
#endif

	//입력 스택. 1이면 입력. 0이면 미입력.
	BYTE mKeys[16];
};


#endif //CHIP8_CHIP8_H
