#include <iostream>
#include <conio.h>
#include <atomic>
#include <thread>

#include "hardware/Chip8.h"

#include <chrono>

// simulation of Windows GetTickCount()
unsigned long long
GetTickCount()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

typedef unsigned long long long_time;


void ReadCh(std::atomic<bool>& run)
{
	std::string buffer;
	int input;

	while (run.load())
	{
		input = _getch();

		if (input == '0')
		{
			run.store(false);
		}
	}
}

void render( Chip8 & chip8 )
{
	char real_screen_array[ 32 * 64 + 32 ] = { ' ' }; //32번 개행 문자 포함.

	for ( int y = 0; y < 32; y++ )
	{
		for ( int x = 0; x < 64; x++ )
		{
			int index = ( y * 64 ) + x;

			if ( chip8.getScreenData(x, y) == 0 )
			{
				real_screen_array[index] = ' ';
			}
			else
			{
				real_screen_array[index] = 'O';
			}
		}

		real_screen_array[ y * 64 + 64 ] = '\n'; // 개행
	}


	printf("%s" , real_screen_array );
}

int main()
{
	std::atomic<bool> run(true);
	Chip8 chip8;

	chip8.reset();
	chip8.loadRom();

	std::thread cinThread(ReadCh, std::ref(run));

	while (run.load())
	{
		long_time tick = GetTickCount();

		chip8.nextStep();
		render(chip8);

		long_time tock = GetTickCount();

		while( tock - tick > 33 )
		{
			tick = GetTickCount();
		}
	}

	run.store(false);
	cinThread.join();


	std::cout << "Hello, World!" << std::endl;
	//Chip8 chip8;
	//std::cout << "chip8.getNextOpCode(); : " << chip8.getNextOpCode() << std::endl;



	return 0;
}
