// TestUI.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Squares.h"
#include "cv.h"


int main(int argc, _TCHAR* argv[])
{
	int key = 0;

	Initialize();

	while(key!='q') 
	{

		key = cvWaitKey(1);
    }
	return 0;
}
