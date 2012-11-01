#include <Windows.h>
#include <iostream>
#include <vld.h>
#include "..\\muspl\\muspl.h"

#pragma comment(lib, "..\\Debug\\muspl.lib")

int main()
{
	MigPlayer player;

	if (false == player.OpenMusic(TEXT("Hello.mp3"), true))
		std::cout << "play music failed" << std::endl;

	player.Play();

	while (1)
	{
		if (::GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;

		if (::GetAsyncKeyState(VK_UP) & 0x8000)
		{
			long vol = player.GetVolume();
			vol += 50;
			if (true ==player.SetVolume(vol))
				std::cout << "+++" << std::endl;
		}

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			long vol = player.GetVolume();
			vol -= 50;
			if (true == player.SetVolume(vol))
				std::cout << "---" << std::endl;
		}

		if (::GetAsyncKeyState('L') & 0x8000)
		{
			if (true == player.Play())
				std::cout << "Play" << std::endl;
		}

		if (::GetAsyncKeyState('P') & 0x8000)
		{
			if (true == player.Pause())
				std::cout << "Pause" << std::endl;
		}

		if (::GetAsyncKeyState('S') & 0x8000)
		{
			if (true == player.Stop())
				std::cout << "Stop" << std::endl;
		}

		if (::GetAsyncKeyState('M'))
		{
			bool mute = player.IsMuted();
			mute = !mute;

			if (true == player.Mute(mute))
				std::cout << (mute ? "Mute" : "Not Mute") << std::endl;
		}

		Sleep(33);
	}
		
	return 0;
}