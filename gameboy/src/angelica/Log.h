//
// Created by nhy20 on 2020-06-01.
//

#ifndef GAMEBOY_LOG_H
#define GAMEBOY_LOG_H

#include <vector>
#include <string>
#include <iostream>

namespace Angelica
{
	class Log
	{


		void addErrorLog( const std::string & log )
		{
			std::cout << "[ERROR] : " << log << std::endl;
			logs.push_back( log );
		}

	private:
		std::vector<std::string> logs;
	};


}


#endif //GAMEBOY_LOG_H
