// TestUI.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Squares.h"
#include "cv.h"
#include <time.h>


int main(int argc, _TCHAR* argv[])
{
	int key = 0;
	Initialize();

	while(key!='q') 
	{
	//FOR FPS ONLY
		GetFPS();
	
		Detect();
		key = cvWaitKey(1);
    }
	Finalize();
	return 0;
}
