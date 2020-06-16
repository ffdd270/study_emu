//
// Created by nhy20 on 2020-06-07.
//

#include "GameboyCPU.h"

void GameboyCPU::nextStep()
{


	BYTE opCode = 0x06;


	switch ( opCode )
	{

		case 0x06:
			load8Bit( mBC.hi, 8 );
			break;
	}





}



// 여기서부터는 명령어 셋
void GameboyCPU::load8Bit(BYTE & to_value, BYTE value)
{

	to_value = value;

}
