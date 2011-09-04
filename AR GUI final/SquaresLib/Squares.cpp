// This is the main DLL file.

#include "cv.h"
#include "Squares.h"
#include "math.h"

// For UI
// Get image parameters
extern "C" __declspec(dllexport) char* GetImg()
{
	if(imgframe == NULL)
		imgframe = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
	cvCopy(frame, imgframe, NULL);
	return imgframe->imageData;
}

extern "C" __declspec(dllexport) int Width()
{
	return frame->width;
}

extern "C" __declspec(dllexport) int Height()
{
	return frame->height;
}

extern "C" __declspec(dllexport) int Stride()
{
	return frame->widthStep;
}

// Set new marker
extern "C" __declspec(dllexport) void SetMark(char* path)
{
	imgTemplate = cvLoadImage(path, 0);
	imgTemplate0 = cvCreateImage( templateSize,8, 1 );
	cvSetImageROI(imgTemplate, cvRect(templateFrame, templateFrame, templateSide, templateSide));
	cvCopy(imgTemplate, imgTemplate0, NULL);
	cvResetImageROI(imgTemplate);
	cvReleaseImage(&imgTemplate);
	// Create rotated marker templates
	imgTemplate1 = cvCreateImage( templateSize,8, 1 );
	imgTemplate2 = cvCreateImage( templateSize,8, 1 );
	imgTemplate3 = cvCreateImage( templateSize,8, 1 );
	Rotate(imgTemplate0, 90, imgTemplate1,templateSize);
	Rotate(imgTemplate0, 180, imgTemplate2,templateSize);
	Rotate(imgTemplate0, 270, imgTemplate3,templateSize);
}

// Set new displayed image
extern "C" __declspec(dllexport) void SetImg(char* path)
{
	imgDisplay = cvLoadImage(path,1);
}

// Set new displayed video
extern "C" __declspec(dllexport) void SetVid(char* path)
{
	vidCapture = cvCreateFileCapture(path);
}

extern "C" __declspec(dllexport) void TogMode()
{
	displayMode = !displayMode;
}

// Video control
extern "C" __declspec(dllexport) void TogVideoPause()
{
	pause = !pause;
}

extern "C" __declspec(dllexport) void RewVideo()
{
	cvSetCaptureProperty(vidCapture, CV_CAP_PROP_POS_FRAMES, 0);
	if(pause==1)
		pause = !pause;
}



// Initializing funtion
int Initialize()
{	
	// Load marker templates and displayed image
	if(!LoadTemplates())
		return 2;
	time(&start);
	// Start camera capture
	capture = cvCaptureFromCAM( 0 );
	if(!capture)
		return 1;

	// Set displayed video
	vidCapture = cvCreateFileCapture("..\\Images\\movie.mov");
	if(!vidCapture)
		return 1;
	show = true;

	// Optional camera input resize
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480 );

	storage = cvCreateMemStorage(0);
	
	return 0;
}

// Get single camera frame
IplImage* GetFrame()
{
	frame = cvQueryFrame( capture );
	if( !frame ) 
		return NULL;

	return frame;
}

// Get single video frame
IplImage* GetVideoFrame()
{
	vidFrame = cvQueryFrame(vidCapture);
	if (!vidFrame)
		return vidFrame2;
	vidFrame2 = vidFrame;
	return vidFrame;
}


//  Main program function
void Main()
{
	CalcFPS();
	Threshold(GetFrame());
	FindContours(thresh, 7);
	DetectSquares(frame,squares);
	if(displayMode==0)
		VisualizeSquares(frame,returnMarkerRotation,imgDisplay);
	else
	{
		if(pause==0)
		VisualizeSquares(frame,returnMarkerRotation,GetVideoFrame());
		else
		VisualizeSquares(frame,returnMarkerRotation,vidFrame);
	}
}

// Binarize the image
void Threshold(IplImage *input)
{	
	if(!thresh)
		thresh = cvCreateImage( cvSize(input->width, input->height), 8, 1 );
	if(!gray)
		gray = cvCreateImage( cvSize(input->width, input->height), 8, 1 );
	//Convert to grayscale
	cvCvtColor(input,gray,CV_BGR2GRAY);

	// for simple threshold
	// cvThreshold(gray, thresh, 51 ,255,CV_THRESH_BINARY);

	// Define threshSize for custom adaptive threshold based on the previously found marker
	double ratio = (threshSize/(double)input->height);
	threshSize =5+((int)( ratio*15)*2);
	if(threshSize<15)
		threshSize=15;

	// Binarize the image
	cvAdaptiveThreshold(gray, thresh, 255, CV_THRESH_BINARY, CV_ADAPTIVE_THRESH_GAUSSIAN_C, threshSize, 5);
	
	// for additional morphology filter
	// cvMorphologyEx(thresh,thresh,temp,NULL,CV_MOP_OPEN,1);

	cvNot(thresh,thresh);
}

// Find and save the frame contours
void FindContours(IplImage * input, int approx)
{
	cvReleaseMemStorage(&storage);
	storage = cvCreateMemStorage(0);
	squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
	contours = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
	contoursResult = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

	threshCont = cvCreateImage( cvSize(input->width, input->height), 8, 1 );

	cvCopy(input, threshCont);
	
	// Find the frame contours
	cvSetImageROI( threshCont, cvRect( 0, 0, input->width,input->height ));
	cvFindContours( threshCont, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	cvResetImageROI( threshCont);

	// Approximate any square with area bigger > 1000 px and save the 4 side contours as squares sequence
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

// Detect the correct square
void DetectSquares( IplImage* input, CvSeq* squares )
{
	CvSeqReader reader;
	cvStartReadSeq( squares, &reader, 0 );
	CvMat* warp_matrix = cvCreateMat(3,3,CV_32FC1);
	CvPoint2D32f squareCorners[4],procCorners[4];
	double tmp=0;
	CvRect boundbox;
	maxVal=0;

	// Read 4 sequence elements at a time (all vertices of a square)
	for(int i = 0; i < squares->total; i += 4)
	{
		CvPoint corners[4], *rectangle = corners;

		// Read 4 vertices
		CV_READ_SEQ_ELEM( corners[0], reader );
		CV_READ_SEQ_ELEM( corners[1], reader );
		CV_READ_SEQ_ELEM( corners[2], reader );
		CV_READ_SEQ_ELEM( corners[3], reader );
		

		// Found square corners
		squareCorners[0].x= corners[0].x;
		squareCorners[0].y= corners[0].y;
		squareCorners[1].x= corners[1].x;
		squareCorners[1].y= corners[1].y;

		squareCorners[2].x= corners[2].x;
		squareCorners[2].y= corners[2].y;
		squareCorners[3].x= corners[3].x;
		squareCorners[3].y= corners[3].y;

		// Processed image corners (same as the template ones)
		procCorners[0].x= 0;
		procCorners[0].y= 0;
		procCorners[1].x= markerSide;
		procCorners[1].y= 0;

		procCorners[2].x= markerSide;
		procCorners[2].y= markerSide;
		procCorners[3].x= 0;
		procCorners[3].y= markerSide;

		// Square transformation upon raw frame using warp matrix	
		Crop(squareCorners,&boundbox);
		cvGetPerspectiveTransform(squareCorners,procCorners,warp_matrix);
		cvSetImageROI(frame,boundbox);
		cvWarpPerspective( frame, imgProc, warp_matrix);
		cvResetImageROI(frame);

		// Adaptive threshold upon processed image to get the best binarized marker image
		cvSetImageROI(imgProc, cvRect(templateFrame, templateFrame, templateSide, templateSide));
		cvCvtColor(imgProc,imgProcGray,CV_BGR2GRAY);
		cvAdaptiveThreshold(imgProcGray, imgProcThresh, 255, CV_THRESH_BINARY, CV_ADAPTIVE_THRESH_GAUSSIAN_C, 151, 1);
		cvNot(imgProcThresh,imgProcThresh);
		cvCopy(imgProcThresh, Result, NULL);

		// Template matching- evaluating the best fit and final bounding box 
		tmp = MatchMarkers(Result);
		cvResetImageROI(imgProc);
		if(maxVal < tmp )
		{
			maxVal = tmp;
			returnMarkerRotation = markerRotation;
			finalSquare[0] = corners[0];
			finalSquare[1] = corners[1];
			finalSquare[2] = corners[2];
			finalSquare[3] = corners[3];
			boundboxlast=boundbox;
		}
	}
	// Updating threshSize value for best Adaptive Threshold results
	if(boundbox.height>boundbox.height)
		threshSize = boundboxlast.height;
	else
		threshSize = boundboxlast.width;
	cvReleaseMat(&warp_matrix);
}

// Visualize found square, determine recognition ratio and rotation angle
void VisualizeSquares(IplImage* input,int angle, IplImage* display)
{
	IplImage* copyDisplay;
	if(returnMarkerRotation==0 || returnMarkerRotation==2)
		copyDisplay = cvCreateImage(  cvSize(display->width,display->height),8, 3 );
	else
		copyDisplay = cvCreateImage(  cvSize(display->height,display->width),8, 3 );
	maxVal=maxVal*100;
	if(maxVal>accuracy)
	{
		float angle = returnMarkerRotation * 90;	
		Rotate(display,angle,copyDisplay, cvGetSize(display));
		//printf("maxVal = %.2f%%, Marker found: Side =  %d\n", maxVal, returnMarkerRotation);
		Visualize(frame,copyDisplay,finalSquare);
	}
	cvReleaseImage(&copyDisplay);

}

// Match the marker with templates
double MatchMarkers(IplImage *input)
{
	double MaxVal[4];
	double maxVal=0, minVal=0;
	// Template matching
	cvZero(imgResult);
	cvMatchTemplate(input, imgTemplate0, imgResult, CV_TM_CCOEFF_NORMED);
	CvPoint minLoc, maxLoc;
	// Finding max match value
	cvMinMaxLoc(imgResult, &minVal, &MaxVal[0], &minLoc, &maxLoc);

	cvZero(imgResult);
	cvMatchTemplate(input, imgTemplate1, imgResult,CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(imgResult, &minVal, &MaxVal[1], &minLoc, &maxLoc);

	cvZero(imgResult);
	cvMatchTemplate(input, imgTemplate2, imgResult, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(imgResult, &minVal, &MaxVal[2], &minLoc, &maxLoc);

	cvZero(imgResult);
	cvMatchTemplate(input, imgTemplate3, imgResult, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(imgResult, &minVal, &MaxVal[3], &minLoc, &maxLoc);

	// determine the max similarity ratio and rotation angle of the found squares
	for(int i=0; i<4; i++)
	{
		if(maxVal<MaxVal[i])
		{
			maxVal = MaxVal[i];
			markerRotation = i;
		}
	}

	return maxVal;
}

// Image/ Video superinmposing
void Visualize(IplImage* input, IplImage* imgDisplay2, CvPoint corners[4])
{
	CvMat* warp_matrix = cvCreateMat(3,3,CV_32FC1);
	IplImage* cpy_img = cvCreateImage( cvGetSize(input), 8, 3 );
	IplImage* neg_img = cvCreateImage( cvGetSize(input), 8, 3 );
	CvPoint2D32f destCorners[4];
	CvPoint2D32f imgCorners[4];
	CvSize displaySize = cvGetSize(imgDisplay2);
	IplImage* blank  = cvCreateImage( displaySize, 8, 3);
	cvZero(blank);
	cvNot(blank,blank);

	// Displayed image corners
	imgCorners[0].x=  0;
	imgCorners[0].y=  0;
	imgCorners[1].x=  displaySize.width;
	imgCorners[1].y=  0;

	imgCorners[2].x=  displaySize.width;
	imgCorners[2].y=  displaySize.height;
	imgCorners[3].x=  0;
	imgCorners[3].y=  displaySize.height;

	// Found finall square corners
	destCorners[0].x= corners[0].x;
	destCorners[0].y= corners[0].y;
	destCorners[1].x= corners[1].x;
	destCorners[1].y= corners[1].y;

	destCorners[2].x= corners[2].x;
	destCorners[2].y= corners[2].y;
	destCorners[3].x= corners[3].x;
	destCorners[3].y= corners[3].y;

	//Superimposing the image onto the frame
	cvGetPerspectiveTransform(imgCorners,destCorners,warp_matrix);
	cvZero(neg_img);
	cvZero(cpy_img);

	cvWarpPerspective( imgDisplay2, neg_img, warp_matrix);
	cvWarpPerspective( blank, cpy_img, warp_matrix);
	cvNot(cpy_img,cpy_img);

	cvAnd(cpy_img,input,cpy_img);
	cvOr(cpy_img,neg_img,input);

	cvReleaseImage(&cpy_img);
	cvReleaseImage(&neg_img);
	cvReleaseMat(&warp_matrix);
	cvReleaseImage(&blank);
}

// FPS oriented functions
double GetFPS()
{
	return FPS;
}

void CalcFPS()
{
	  double FPScounter;
	  time(&end);
      ++counter;
      sec = difftime (end, start); 
      FPScounter = counter / sec;
	  if(sec > 1)
	  {
		  counter = 0;
		  time(&start);
		  FPS=FPScounter;
	  }
}

// Crop and find the boundingbox
void Crop(CvPoint2D32f* squareCorners, CvRect* boundbox)
{
		float cornersx[4],cornersy[4];
		int minx=5000,maxx=0,miny=5000,maxy=0;

		for (int j=0;j<4;j++)
		{
			cornersx[j] = squareCorners[j].x;
			cornersy[j] = squareCorners[j].y;
		}

		for (int j=0;j<4;j++)
		{
			if(minx>cornersx[j])
				minx=cornersx[j];
			if(maxx<cornersx[j])
				maxx=cornersx[j];
			if(miny>cornersy[j])
				miny=cornersy[j];
			if(maxy<cornersy[j])
				maxy=cornersy[j];
		}
		int rectwidth = maxx-minx;
		int rectheight = maxy-miny;
		*boundbox=cvRect(minx,miny,rectwidth,rectheight);

		for(int j=0;j<4;j++)
		{
			squareCorners[j].x -= minx;
			squareCorners[j].y -= miny;
		}
}

// Load the marker template and displayed image
bool LoadTemplates()
{
	imgDisplay = cvLoadImage("..\\Images\\image.jpg",1);
	imgTemplate = cvLoadImage("..\\Images\\marker.jpg", 0);
	if(imgDisplay == NULL || imgTemplate == NULL)
		return false;
	imgTemplate0 = cvCreateImage( templateSize,8, 1 );
	cvSetImageROI(imgTemplate, cvRect(templateFrame, templateFrame, templateSide, templateSide));
	cvCopy(imgTemplate, imgTemplate0, NULL);
	cvResetImageROI(imgTemplate);
	cvReleaseImage(&imgTemplate);
	// Create rotated marker templates
	imgTemplate1 = cvCreateImage( templateSize,8, 1 );
	imgTemplate2  = cvCreateImage( templateSize,8, 1 );
	imgTemplate3  = cvCreateImage( templateSize,8, 1 );
	Rotate(imgTemplate0, 90, imgTemplate1,templateSize);
	Rotate(imgTemplate0, 180, imgTemplate2,templateSize);
	Rotate(imgTemplate0, 270, imgTemplate3,templateSize);
	return true;
}

// Rotate the image
void Rotate(IplImage * input, float angle, IplImage * output, CvSize sizeRotated)
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

	cvGetQuadrangleSubPix( input, output, &mapMatrix);
}


bool Finalize()
{
	if(!capture)
		return true;

	cvReleaseCapture( &capture );
	cvClearMemStorage( storage );
	if(show)
	{
		cvDestroyWindow("Display");
		cvDestroyWindow("Threshold");
		cvDestroyWindow("Processing");
	}
	return true;
}