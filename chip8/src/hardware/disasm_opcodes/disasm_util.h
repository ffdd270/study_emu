//
// Created by nhy20 on 2020-05-30.
//

#ifndef CHIP8_DISASM_UTIL_H
#define CHIP8_DISASM_UTIL_H
#include <string>

inline std::string hex_to_string( int opCode )
{
	std::string string = "";

	while ( opCode != 0 )
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
	}

	return "0x" + std::string( string.rbegin(), string.rend());
}

inline std::string createComment_Registers( const std::string & func_name, BYTE register_index_x, BYTE register_index_y )
{
	return func_name + " V" + hex_to_string( register_index_x ) + " V" + hex_to_string( register_index_y );
}

inline std::string createComment_RegByte( const std::string & func_name, BYTE register_index_x, BYTE byte )
{
	return func_name + " V" + hex_to_string( register_index_x ) + " " + hex_to_string( byte );
}


inline std::string opCodeToDisASMString( WORD opCode, const std::string & comment )
{
	return hex_to_string(opCode) + " # " + comment;
}


#endif //CHIP8_DISASM_UTIL_H
