// SquareDetection.h

#include "cv.h"
#include <highgui.h>
#include <stdio.h>
#include <ctype.h>


CvCapture		*capture;
IplImage		*frame;
IplImage		*thresh;
CvMemStorage	*storage;

IplImage		*imgTemplate0;
IplImage		*imgTemplate1;
IplImage		*imgTemplate2;
IplImage		*imgTemplate3;

int				markerSize = 100;
double			maxVal;
int				markerRotation;

// Externals //
extern "C" __declspec(dllexport) bool Initialize();


// Internals//
void LoadTemplates();
void Rotate(IplImage * rot, float angle, IplImage * out, CvSize SizeRotated);