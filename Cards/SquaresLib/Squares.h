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
extern "C" __declspec(dllexport) void GetFPS();

//Parameters
int				threshSize = 0;
CvRect			boundboxlast;
bool			displayMode = 0;
int				accuracy = 50;
int				count = 4;
int				templateFrame = 15;
int				markerSide = 100;
//CvSize			testSize = cvSize(300,300);
//Eval Parameters
int				templateSide = markerSide-(2*templateFrame);
CvSize			markerSize = cvSize(markerSide,markerSide);
CvSize			templateSize = cvSize(templateSide,templateSide);


CvCapture		*capture;
CvCapture		*vidCapture;
IplImage		*frame;
IplImage		*gray;
IplImage		*thresh;
IplImage		*threshCont;
IplImage		*Result = cvCreateImage( templateSize, 8,1);
IplImage		*imgProc = cvCreateImage(markerSize, 8, 3 );
IplImage		*imgProcGray = cvCreateImage( templateSize,8,1);
IplImage		*imgProcThresh = cvCreateImage( templateSize,8,1);
IplImage		*imgProcFinal = cvCreateImage( templateSize,8,1);

IplImage		*imgMatchResult;
CvMemStorage	*storage;
IplImage		*imgResult = cvCreateImage(cvSize(1, 1),  IPL_DEPTH_32F, 1);

IplImage		*vidFrame;
IplImage		*vidFrame2;
IplImage		*imgDisplay;
IplImage		*imgTemplate;
IplImage		*imgTemplate0;
IplImage		*imgTemplate1;
IplImage		*imgTemplate2;
IplImage		*imgTemplate3;


double			maxVal;
int				markerRotation;
int				returnMarkerRotation;
CvPoint			finalSquare[4];
CvSeq			*contours;
CvSeq			*contoursResult;
CvSeq			*squares;


// Externals //
extern "C" __declspec(dllexport) bool Initialize();
extern "C" __declspec(dllexport) void Detect();
extern "C" __declspec(dllexport) IplImage* GetFrame();
extern "C" __declspec(dllexport) IplImage* GetVideoFrame();
extern "C" __declspec(dllexport) bool Finalize();

// Internals//
void Threshold(IplImage * input);
void LoadTemplates();
void Rotate(IplImage * input, float angle, IplImage * out, CvSize SizeRotated);
void FindContours(IplImage * in, int approx);
void DetectSquares( IplImage* input, CvSeq* squares );
double MatchMarkers( IplImage* input);
void VisualizeSquares(IplImage* input,int angle, IplImage* display);
void Visualize(IplImage* input, IplImage* copyinput2, CvPoint pt[4]);
void Crop(CvPoint2D32f* corners,CvRect* boundbox);
