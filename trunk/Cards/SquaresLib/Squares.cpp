// This is the main DLL file.

#include "cv.h"
#include "Squares.h"
#include "math.h"

bool Initialize()
{
	capture = cvCaptureFromCAM( 0 );
	if(!capture)
		return false;

	cvNamedWindow( "Detection", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Threshold", CV_WINDOW_AUTOSIZE );
	show = true;

	//optional camera input resize
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480 );

	storage = cvCreateMemStorage(0);

	LoadTemplates();
	return true;
}

IplImage* GetFrame()
{
	frame = cvQueryFrame( capture );
 
    if( !frame ) 
		return NULL;

	return frame;
}


void Threshold(IplImage *input)
{
	if(!thresh)
		thresh = cvCreateImage( cvSize(input->width, input->height), 8, 1 );

	gray = cvCreateImage( cvSize(input->width, input->height), 8, 1 );
	cvCvtColor(input,gray,CV_BGR2GRAY);
	cvThreshold(gray, thresh, 100, 255, CV_THRESH_BINARY);
}

void Detect()
{
	Threshold(GetFrame());
	FindContours(thresh, 5);
	DrawSquares(frame,squares);
	//ExtractMarkers();
}


void ShowFrame()
{
	CvPoint * rect = finalSquare;
	int count = 4;
	CvSeqReader reader;
	cvStartReadSeq(squares, &reader);
	if(show == true)
	{
		IplImage *frameCpy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, 3);
		cvCopy(frame, frameCpy);
		cvPolyLine( frameCpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
	
		cvShowImage("Detection", frame);
		cvShowImage("Threshold", thresh);
		
	}
}


void FindContours(IplImage * input, int approx)
{
	cvReleaseMemStorage(&storage);
	storage = cvCreateMemStorage(0);
	squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
	contours = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
	contoursResult = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

	threshCont = cvCreateImage( cvSize(input->width, input->height), 8, 1 );
	cvCopy(input, threshCont);

	//cvSetImageROI( threshCont, cvRect( 0, 0, input->width,input->height ));
	cvFindContours( threshCont, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

	while( contours )
    {
		contoursResult = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, approx, 0 );
        if( contoursResult->total == 4 && cvContourArea(contoursResult,CV_WHOLE_SEQ,0) > 1000 && cvCheckContourConvexity(contoursResult) )
        {
			for(int i = 0; i < 4; i++ )
				cvSeqPush( squares, (CvPoint*)cvGetSeqElem( contoursResult, i ));
        }
        contours = contours->h_next;
    }
   cvReleaseImage( &threshCont );
}


void DrawSquares( IplImage* input, CvSeq* squares )
{
	CvSeqReader reader;
	cvStartReadSeq( squares, &reader, 0 );
	//system("cls");
	// read 4 sequence elements at a time (all vertices of a square)
	for(int i = 0; i < squares->total; i += 4 )
	{
		CvPoint corners[4], *rectangle = corners;

		// read 4 vertices
		CV_READ_SEQ_ELEM( corners[0], reader );
		CV_READ_SEQ_ELEM( corners[1], reader );
		CV_READ_SEQ_ELEM( corners[2], reader );
		CV_READ_SEQ_ELEM( corners[3], reader );
		//printf("Square:\n corner1: x:%d, y:%d\n corner2: x:%d, y:%d\n corner3: x:%d, y:%d\n corner4: x:%d, y:%d\n\n", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y);

        // draw the square as a closed polyline (for debug purposes only)
        cvPolyLine( input, &rectangle, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
    }
   cvShowImage( "Detection", input );
}



void LoadTemplates()
{
	imgTemplate1 = cvLoadImage("\Images\marker.jpg", 0);
	imgTemplate1 = cvCreateImage( markerSize,8, 1 );
	imgTemplate2  = cvCreateImage( markerSize,8, 1 );
	imgTemplate3  = cvCreateImage( markerSize,8, 1 );
	Rotate(imgTemplate1, 90, imgTemplate1,markerSize);
	Rotate(imgTemplate1, 180, imgTemplate2,markerSize);
	Rotate(imgTemplate1, 270, imgTemplate3,markerSize);
}

void Rotate(IplImage * input, float angle, IplImage * out, CvSize sizeRotated)
{
	float matrix[6];
	CvMat mapMatrix = cvMat(2, 3, CV_32F, matrix);
	float angleRadians = angle  * ((float)CV_PI / 180.0f);
	matrix[0] = (float)( cos(angleRadians) );
	matrix[1] = (float)( sin(angleRadians) );
	matrix[3] = -matrix[1];
	matrix[4] = matrix[0];
	matrix[2] = sizeRotated.width*0.5f;  
	matrix[5] = sizeRotated.height*0.5f;
	
	cvGetQuadrangleSubPix( input, out, &mapMatrix);
}



bool Finalize()
{
	if(!capture)
		return true;

	cvReleaseCapture( &capture );
	cvClearMemStorage( storage );
	if(show)
	{
		cvDestroyWindow("Detection");
		cvDestroyWindow("Threshold");
	}
	return true;
}