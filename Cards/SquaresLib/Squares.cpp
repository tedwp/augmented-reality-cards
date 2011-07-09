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
 cvNamedWindow( "Processing", CV_WINDOW_AUTOSIZE );
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
 if(!gray)
  gray = cvCreateImage( cvSize(input->width, input->height), 8, 1 );
 cvCvtColor(input,gray,CV_BGR2GRAY);
 cvThreshold(gray, thresh, 100, 255, CV_THRESH_BINARY);
}

void Detect()
{
 Threshold(GetFrame());
 FindContours(thresh, 5);
 DrawSquares(frame,squares);
}

double MatchMarkers(IplImage *input)
{
 double MaxVal[4];
 double maxVal=0, minVal=0;
 //template matching
 cvZero(imgResult);
 cvMatchTemplate(input, imgTemplate0, imgResult, CV_TM_CCORR_NORMED);
 CvPoint minLoc, maxLoc;
 //finding max match value
 cvMinMaxLoc(imgResult, &minVal, &MaxVal[0], &minLoc, &maxLoc);

 cvZero(imgResult);
 cvMatchTemplate(input, imgTemplate1, imgResult, CV_TM_CCORR_NORMED);
 //finding max match value
 cvMinMaxLoc(imgResult, &minVal, &MaxVal[1], &minLoc, &maxLoc);

 cvZero(imgResult);
 cvMatchTemplate(input, imgTemplate2, imgResult, CV_TM_CCORR_NORMED);
 //finding max match value
 cvMinMaxLoc(imgResult, &minVal, &MaxVal[2], &minLoc, &maxLoc);

 cvZero(imgResult);
 cvMatchTemplate(input, imgTemplate3, imgResult, CV_TM_CCORR_NORMED);
 //finding max match value
 cvMinMaxLoc(imgResult, &minVal, &MaxVal[3], &minLoc, &maxLoc);

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
  cvReleaseImage(&frameCpy);
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


 
 CvMat* warp_matrix = cvCreateMat(3,3,CV_32FC1);
 CvPoint2D32f squareCorners[4],procCorners[4];
 if(!imgProc)
  imgProc = cvCreateImage( markerSize, 8, 1 );
 double maxval;
 double returnmarkerRotation;
 double tmp;
 tmp=0;
 maxval=0;
 IplImage* Result = cvCreateImage( markerSize, 8,1);
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

  // found square corners
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

  // Square transforming to processed image using warp matrix
  cvGetPerspectiveTransform(squareCorners,procCorners,warp_matrix);
  cvZero(imgProc);
  cvWarpPerspective( thresh, imgProc, warp_matrix);

        // draw the square as a closed polyline (for debug purposes only)
       // cvPolyLine( input, &rectangle, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );

  tmp = MatchMarkers(imgProc);
  if(maxval < tmp )
  {
   maxval = tmp;
   returnmarkerRotation = markerRotation;
   cvCopyImage(imgProc, Result);
   finalSquare[0] = corners[0];
   finalSquare[1] = corners[1];
   finalSquare[2] = corners[2];
   finalSquare[3] = corners[3];
  }
 }
  IplImage* imgDisplay2 = cvCreateImage(  cvGetSize(imgDisplay),8, 3 );
  maxval=maxval*100;
  if(maxval>75)
  {
  float angle = returnmarkerRotation * 90;
  Rotate(imgDisplay,angle,imgDisplay2, cvGetSize(imgDisplay));
  //printf("MaxVal = %.2f%%, Marker found: Side =  %d\n", maxval, returnmarkerRotation);
  cvShowImage("Display", imgDisplay);
  visualize(frame,imgDisplay2,finalSquare);
  cvShowImage( "Processing", Result );
  }

  cvReleaseImage(&imgDisplay2);
  cvReleaseMat(&warp_matrix);
  cvReleaseImage(&Result);
 
   cvShowImage( "Processing", imgProc);
   cvShowImage( "Detection", input );
}


void visualize(IplImage* input, IplImage* imgDisplay2, CvPoint corners[4])
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

 imgCorners[0].x=  0;
 imgCorners[0].y=  0;
 imgCorners[1].x=  displaySize.width;
 imgCorners[1].y=  0;

 imgCorners[2].x=  displaySize.width;
 imgCorners[2].y=  displaySize.height;
 imgCorners[3].x=  0;
 imgCorners[3].y=  displaySize.height;


 destCorners[0].x= corners[0].x;
 destCorners[0].y= corners[0].y;
 destCorners[1].x= corners[1].x;
 destCorners[1].y= corners[1].y;
    
 destCorners[2].x= corners[2].x;
 destCorners[2].y= corners[2].y;
 destCorners[3].x= corners[3].x;
 destCorners[3].y= corners[3].y;

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


void LoadTemplates()
{
 imgDisplay = cvLoadImage("..\\Images\\image.jpg",1);
 imgTemplate0 = cvLoadImage("..\\Images\\marker.jpg", 0);
 imgTemplate1 = cvCreateImage( markerSize,8, 1 );
 imgTemplate2  = cvCreateImage( markerSize,8, 1 );
 imgTemplate3  = cvCreateImage( markerSize,8, 1 );
 Rotate(imgTemplate0, 90, imgTemplate1,markerSize);
 Rotate(imgTemplate0, 180, imgTemplate2,markerSize);
 Rotate(imgTemplate0, 270, imgTemplate3,markerSize);
}

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
  cvDestroyWindow("Detection");
  cvDestroyWindow("Threshold");
 }
 return true;
}