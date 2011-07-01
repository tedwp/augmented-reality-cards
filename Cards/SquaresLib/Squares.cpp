// This is the main DLL file.

#include "cv.h"
#include "Squares.h"
#include "math.h"

bool Initialize()
{
	capture = cvCaptureFromCAM( 0 );
	if(!capture)
		return false;

	//optional camera input resize
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480 );

	storage = cvCreateMemStorage(0);

	LoadTemplates();
	return true;
}


void LoadTemplates()
{
	imgTemplate1 = cvLoadImage("marker.jpg", 0);
	CvSize sizeRotated;
	sizeRotated.width = markerSize;
	sizeRotated.height = markerSize;
	imgTemplate1 = cvCreateImage( sizeRotated,8, 1 );
	imgTemplate2  = cvCreateImage( sizeRotated,8, 1 );
	imgTemplate3  = cvCreateImage( sizeRotated,8, 1 );
	Rotate(imgTemplate1, 90, imgTemplate1,sizeRotated);
	Rotate(imgTemplate1, 180, imgTemplate2,sizeRotated);
	Rotate(imgTemplate1, 270, imgTemplate3,sizeRotated);
}

void Rotate(IplImage * rot, float angle, IplImage * out, CvSize sizeRotated)
{
	float m[6];
	CvMat M = cvMat(2, 3, CV_32F, m);
	float angleRadians = angle  * ((float)CV_PI / 180.0f);
	m[0] = (float)( cos(angleRadians) );
	m[1] = (float)( sin(angleRadians) );
	m[3] = -m[1];
	m[4] = m[0];
	m[2] = sizeRotated.width*0.5f;  
	m[5] = sizeRotated.height*0.5f;
	
	cvGetQuadrangleSubPix( rot, out, &M);
}
