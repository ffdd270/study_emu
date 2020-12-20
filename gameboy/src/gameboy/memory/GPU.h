//
// Created by HaruGakkaP on 2020-12-15.
//

#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include "memory/MemoryInterface.h"
#include <array>

namespace GPURegisterHelper
{
	//LCD Control Register
	[[nodiscard]] bool IsLCDDisplayEnable(BYTE value); // BIT 7
	[[nodiscard]] WORD GetSelectedWindowTileMap(BYTE value); // BIT 6
	[[nodiscard]] bool IsWindowDisplayEnable(BYTE value); // BIT 5
	[[nodiscard]] WORD GetSelectBGAndWindowTileData(BYTE value); // BIT 4
	[[nodiscard]] WORD GetSelectBGTileMapDisplay(BYTE value); // BIT 3
	[[nodiscard]] bool IsSpriteSize(BYTE value); // BIT 2
	[[nodiscard]] bool IsSpriteDisplayEnable(BYTE value); // BIT 1
	[[nodiscard]] bool CheckProperty(BYTE value); // BIT 0

	//LCD Status Register
	[[nodiscard]] bool IsEnableLYCoincidenceInterrupt(BYTE value);  // BIT 6
	[[nodiscard]] bool IsEnableMode2OAMInterrupt(BYTE value); // BIT 5
	[[nodiscard]] bool IsEnableMode1VBlankInterrupt(BYTE value); // BIT 4
	[[nodiscard]] bool IsEnableMode0HBlankInterrupt(BYTE value); // BIT 3
	[[nodiscard]] bool GetCoincidenceFlag(BYTE value); // BIT 2
	[[nodiscard]] BYTE GetModeFlag(BYTE value); // BIT 1-0
}


// Video RAM
// 0x8000~0x9fff
class GPU final : public MemoryInterface
{
public:
	GPU();

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;

	void NextStep(size_t clock);

	~GPU() final = default;

	//LCD Control Register
	[[nodiscard]] bool IsLCDDisplayEnable() const; // BIT 7
	[[nodiscard]] WORD GetSelectedWindowTileMap() const; // BIT 6
	[[nodiscard]] bool IsWindowDisplayEnable() const; // BIT 5
	[[nodiscard]] WORD GetSelectBGAndWindowTileData() const; // BIT 4
	[[nodiscard]] WORD GetSelectBGTileMapDisplay() const; // BIT 3
	[[nodiscard]] bool IsSpriteSize() const; // BIT 2
	[[nodiscard]] bool IsSpriteDisplayEnable() const; // BIT 1
	[[nodiscard]] bool CheckProperty() const; // BIT 0

	//LCD Status Register
	[[nodiscard]] bool IsEnableLYCoincidenceInterrupt() const;  // BIT 6
	[[nodiscard]] bool IsEnableMode2OAMInterrupt() const; // BIT 5
	[[nodiscard]] bool IsEnableMode1VBlankInterrupt() const; // BIT 4
	[[nodiscard]] bool IsEnableMode0HBlankInterrupt() const; // BIT 3
	//READ ONLY
	[[nodiscard]] bool GetCoincidenceFlag() const; // BIT 2
	[[nodiscard]] BYTE GetModeFlag() const; // BIT 1-0
private:
	void checkAddress(size_t mem_addr) const;

	void enableVBlank();
	// 마더보드는 이걸 접근 가능함.
	void disableVBlank();

	void setCoincidenceInterrupt( bool value );

	void setLCDMode( BYTE mode );
private:
	BYTE mLCDControlRegister;
	BYTE mLCDStatusRegister;

	// LCD가 스크롤 될 디스플레이 좌표.
	// 끝에 도달하면 자동으로 끝으로 맵핑을 시켜준다.
	BYTE mScrollY;
	BYTE mScrollX;

	// LYC -> LY와 비교해서, 같다면 LCDC의 2번 비트, STAT 인터럽트를 발생시킨다.
	BYTE mLYC;

	// LCD의 창 위치. 창은 일반 배경위에 표시할 수 있는 대체 배경.
	// WX는 0~166. WY는 0~143 범위 안에 설정되면 창이 표시됨. WX = 7, WY = 0는 왼쪽 상단에 있는 창을 찾은 다음 일반 배경을 완전히 덮음.
	// WX 0~6과 166은 하드웨어 버그로 인해신뢰할 수 없음. WX가 0이라면 SCX가 변경 될 때 창이 수평으로 더듬음.
	BYTE mWY, mWX;

	size_t mDots; // 점 찍는 중..
	size_t mScanLineY; // 스캔 라인..

	std::array<BYTE, 0x2000> mMemory;
};


#endif //GAMEBOY_GPU_H
