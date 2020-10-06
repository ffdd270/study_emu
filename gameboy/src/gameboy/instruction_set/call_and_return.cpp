//
// Created by HaruGakkaP on 2020-10-06.
//

#include "../GameboyCPU.h"

void GameboyCPU::callWord(BYTE op_code)
{
	mGameMemory[ mSP.reg_16 - 1 ] = mPC.hi;
	mGameMemory[ mSP.reg_16 - 2 ] = mPC.lo;

	mPC.reg_16 = immediateValue16();
	mSP.reg_16 = mSP.reg_16 - 2;
}

