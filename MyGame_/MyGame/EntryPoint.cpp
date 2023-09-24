/*
	WinMain() is the entry point of a Windows program
	(instead of the familiar main() in a console program)
*/

// Includes
//=========

#include "cMyGame.h"
#include <Windows.h>
// Entry Point
//============//write somethong broooo. I was wondering where this comment came from

int WINAPI WinMain( HINSTANCE i_thisInstanceOfTheApplication, HINSTANCE, char* i_commandLineArguments, int i_initialWindowDisplayState )
{
	OutputDebugStringW(L"\n");
	OutputDebugStringW(L"*******************Game Starting***********************");
	OutputDebugStringW(L"\n");
#if defined( EAE6320_PLATFORM_D3D )
	OutputDebugStringW(L"*******************Platform:Direct3D***********************\n");
#elif defined( EAE6320_PLATFORM_GL )
	OutputDebugStringW(L"*******************Platform:OpenGL***********************\n");
#endif
	return eae6320::Application::Run<eae6320::cMyGame>( i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState );
}
