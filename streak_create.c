#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

void copyRow(IplImage* main, IplImage* sub, int start_height, int end_height, int line_height);

int main(int argc, char *argv[])
{
    CvCapture* capture = cvCreateFileCapture("/Users/Will/Desktop/MVI_5415.MOV");

    int s = 2000;
    IplImage *framebuff = cvQueryFrame(capture);
    IplImage *img[s];

    cvNamedWindow("Window1", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Window1",100,0);
    
    cvNamedWindow("Window2", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Window2",100,500);

    int i,j;
    int num_frames;
    
    int vid_width  = 1280;
    int vid_height = 720;
    cvSetImageROI(framebuff, cvRect(0,0,vid_width,vid_height));


    for(i=0; 1; i++) {
        framebuff = cvQueryFrame(capture);
        if(!framebuff) { 
            num_frames = i-1; 
            fprintf(stderr, "\n\nFinished loading %d frames of video\n\n", num_frames); break; 
        }
        if(i>s-5) {
            fprintf(stderr, "\n!!!!!!!!!!!!!!!!\n!!!!!!!!!!!!!!!!\n");
            fprintf(stderr, "Video over %d frames long - please update size of img[] array\n", s); 
            return 0;
        }

        img[i] = cvCreateImage(cvSize(vid_width,vid_height),8,3);

        cvSetImageROI(framebuff, cvRect(0,0,vid_width,vid_height));
        cvSetImageROI(img[i],    cvRect(0,0,vid_width,vid_height));
        if(i%100==0 && i!=0) {
            fprintf(stderr,"\nCopied up to image %d into memory", i);
            fprintf(stderr, " : width: %d, %d - height: %d, %d" 
                   , framebuff->width, img[i]->width, framebuff->height, img[i]->height);
        }
        cvCopy(framebuff, img[i], NULL);
    }

    double frames_per_sec = 25;
    CvSize video_track_size = cvSize(vid_width,num_frames);
    CvVideoWriter* writerTracker = cvCreateVideoWriter(
        "/Users/Will/Desktop/splitVideo5.avi",
        CV_FOURCC('D','I','V','X'),
        frames_per_sec,
        video_track_size,
        1
    );


    IplImage *mainimg = cvCreateImage(cvSize(vid_width,num_frames),8,3);


    for(j=0; j<vid_height; j++) {
        for(i=0; i<num_frames; i++) {
            copyRow(mainimg, img[i], j, i, 1);
            cvSetImageROI(mainimg,   cvRect(0,0,vid_width,num_frames));
        }
        if(j%100==0 && j!=0) {
            fprintf(stderr, "\nWritten up to frame %d...", j);
        }
        //cvShowImage("Window1", mainimg);
        //cvWaitKey(0);
        cvWriteFrame(writerTracker, mainimg);
    }

    fprintf(stderr, "\n\nComplete!\n\n");
    cvReleaseVideoWriter(&writerTracker);
}


void copyRow(IplImage* main, IplImage* sub, int start_height, int end_height, int line_height)
{
    int width = main->width;

    cvSetImageROI(main, cvRect(0,end_height,  width,line_height));
    cvSetImageROI(sub,  cvRect(0,start_height,width,line_height));

    cvCopy(sub, main, NULL);

    cvResetImageROI(main);
    cvResetImageROI(sub );
}
