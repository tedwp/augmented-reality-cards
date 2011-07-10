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
	time(&start);

	while(key!='q') 
	{
	//FOR FPS ONLY
	  time(&end);
      ++counter;
      sec = difftime (end, start); 
	  if(sec == 5)
	  {
		  printf("FPS = %.2f\n", FPS);
		  counter= 0;
		  start = start+4;
	  }
      FPS = counter / sec;


	
		Detect();
		ShowFrame();
		key = cvWaitKey(1);
    }
	Finalize();
	return 0;
}
