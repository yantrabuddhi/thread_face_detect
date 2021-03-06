//main.cpp
//mandeep singh bhatia

//test opencv in threaded way
//capture images in a thread
//process in another thread

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <cstdio>
#include <thread>
#include <mutex>

using namespace std;
using namespace cv;


/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

Mat frm;
mutex mtx;
int c=0;

void run_capture()
{
  VideoCapture capture;

  //-- 1. Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return; };
  if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return; };

  //-- 2. Read the video stream
  capture.open( -1 );//-1
  if( capture.isOpened() )
  {
      //capture.set(CV_CAP_PROP_FOURCC ,CV_FOURCC('M', 'J', 'P', 'G') );//MJPG
      capture.set(CV_CAP_PROP_FRAME_WIDTH,640);//640
      capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);//480

    for(;;)
    {
      mtx.lock();
      capture >> frm;
      mtx.unlock();

      //-- 3. Apply the classifier to the frame
      if( frm.empty() )
       { printf(" --(!) No captured frame -- Break!"); break; }

      c = waitKey(10);
      if( (char)c == 'c' ) { break; }

    }
  }

}


/**
 * @function detectAndDisplay
 */
void detect_and_display()
{
 Mat frame;
  for(;;){
   mtx.lock();
   frm.copyTo(frame);
   mtx.unlock();
   if( (char)c == 'c' ) { break; }
   if (frame.empty()) continue;
   std::vector<Rect> faces;
   Mat frame_gray;
	
   cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
   equalizeHist( frame_gray, frame_gray );
   //-- Detect faces
   face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

   for( size_t i = 0; i < faces.size(); i++ )
    {
      Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
      ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 2, 8, 0 );

      Mat faceROI = frame_gray( faces[i] );
      std::vector<Rect> eyes;

      //-- In each face, detect eyes
      eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

      for( size_t j = 0; j < eyes.size(); j++ )
       {
         Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
         int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
         circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 3, 8, 0 );
       }
    }
   //-- Show what you got
   imshow( window_name, frame );
  }
}

int main()
{
  thread cap(run_capture);
  thread dis(detect_and_display);
  cap.join();
  dis.join();
  return 0;
}
