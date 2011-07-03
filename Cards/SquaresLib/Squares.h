// SquareDetection.h

#include "cv.h"
#include <highgui.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

//FOR FPS ONLY
time_t start, end;
double FPS;
int counter = 0;
double sec;
extern "C" __declspec(dllexport) double GetFPS();

CvCapture		*capture;
IplImage		*frame;
IplImage		*gray;
IplImage		*thresh;
IplImage		*threshCont;
IplImage		*imgProc;
IplImage		*imgMatchResult;
CvMemStorage	*storage;
IplImage* imgResult = cvCreateImage(cvSize(1, 1),  IPL_DEPTH_32F, 1);

IplImage		*imgDisplay;
IplImage		*imgTemplate0;
IplImage		*imgTemplate1;
IplImage		*imgTemplate2;
IplImage		*imgTemplate3;

int				count = 4;
int				markerSide = 100;
CvSize			markerSize = cvSize(markerSide,markerSide);
double			maxVal;
int				markerRotation;
CvPoint			finalSquare[4];
CvSeq			*contours;
CvSeq			*contoursResult;
CvSeq			*squares;
bool			show;

// Externals //
extern "C" __declspec(dllexport) bool Initialize();
extern "C" __declspec(dllexport) void Detect();
extern "C" __declspec(dllexport) IplImage* GetFrame();
extern "C" __declspec(dllexport) void ShowFrame();
extern "C" __declspec(dllexport) bool Finalize();


// Internals//
void Threshold(IplImage * input);
void LoadTemplates();
void Rotate(IplImage * input, float angle, IplImage * out, CvSize SizeRotated);
void FindContours(IplImage * in, int approx);
void DrawSquares( IplImage* input, CvSeq* squares );
double MatchMarkers( IplImage* input);
void visualize(IplImage* input, IplImage* imgDisplay2, CvPoint pt[4]);