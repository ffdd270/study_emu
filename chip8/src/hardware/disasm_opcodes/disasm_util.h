//
// Created by nhy20 on 2020-05-30.
//

#ifndef CHIP8_DISASM_UTIL_H
#define CHIP8_DISASM_UTIL_H

#ifdef _CHIP8_DISASM_BUILD

#include <string>

inline std::string hex_to_string( int opCode )
{
	std::string string = "";

	do
	{
		int number = opCode % 16;
		std::string add_string = "";

		switch (number)
		{
			case 0xA:
				add_string = "A";
				break;
			case 0xB:
				add_string = "B";
				break;
			case 0xC:
				add_string = "C";
				break;
			case 0xD:
				add_string = "D";
				break;
			case 0xE:
				add_string = "E";
				break;
			case 0xF:
				add_string = "F";
				break;
			default:
				add_string = std::to_string(number);
		}

		string.append( add_string );

		opCode /= 16;
	} while ( opCode != 0 );

	return std::string( string.rbegin(), string.rend());
}

inline std::string createComment_Registers( const std::string & func_name, BYTE register_index_x, BYTE register_index_y )
{
	return func_name + " V" + hex_to_string( register_index_x ) + " V" + hex_to_string( register_index_y );
}

inline std::string createComment_RegByte( const std::string & func_name, BYTE register_index_x, BYTE byte )
{
	return func_name + " V" + hex_to_string( register_index_x ) + " 0x" + hex_to_string( byte );
}


inline std::string opCodeToDisASMString( WORD opCode, const std::string & comment )
{
	return  "0x" + hex_to_string(opCode) + " # " + comment;
}

inline WORD GetOpCodeSecondValue( WORD opCode )
{
	return (opCode & 0x0F00)  >> 8;
}

inline WORD GetOpCodeThirdValue( WORD opCode )
{
	return (opCode & 0x00F0)  >> 4;
}

inline WORD GetOpCodeForthValue( WORD opCode )
{
	return (opCode & 0xF000)  >> 12;
}

#endif //_CHIP8_DISASM_BUILD

#endif //CHIP8_DISASM_UTIL_H
