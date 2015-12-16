#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdlib>
#include <iostream>
#include <functional>
extern "C" {
#include <sys/times.h>
#include <unistd.h>
}

using namespace std;

const int exitKeyCode = 27;
const char winName[] = "Camera";
const size_t framesPerMeasure = 30;

void interp(IplImage *img, size_t x0, size_t y0, size_t w, size_t h);
inline char& getcoord(IplImage *img, int chan, size_t x, size_t y);
long calcUtime(std::function<void()> fn);

int main(int argc, char **argv) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " cam_number sampling_points_x sampling_points_y" << endl;
    return EXIT_FAILURE;
  }

  const size_t camNum = atoi(argv[1]), samplingPointsX = atoi(argv[2]), samplingPointsY = atoi(argv[3]);
  CvCapture *capture = cvCreateCameraCapture(camNum);
  if (!capture) {
    cerr << "Can't open camera" << endl;
    return EXIT_FAILURE;
  }
  
  size_t frameCounter = 0;
  const long ticksPerSecond = sysconf(_SC_CLK_TCK);
  struct tms fpsBegin, fpsEnd, tmpBegin, tmpEnd;
  long queryTime = 0, transformTime = 0, showTime = 0;
  do {
    if (!(frameCounter % framesPerMeasure)) 
      times(&fpsBegin);
    
    IplImage *frame;
    queryTime += calcUtime([&]() { frame = cvQueryFrame(capture); });
    if (!frame)
      break;
    IplImage *img = cvCloneImage(frame);

    const size_t 
      samplingDistX = img->width / samplingPointsX,
      samplingDistY = img->height / samplingPointsY;
    
    transformTime += calcUtime([&]() {
      for (size_t i = 0; i < samplingPointsY; ++i) 
        for (size_t j = 0; j < samplingPointsX; ++j) 
          interp(img, j * samplingDistX, i * samplingDistY, samplingDistX, samplingDistY);
    });

    showTime += calcUtime([&]() { cvShowImage(winName, img); });
    if (!((++frameCounter) % framesPerMeasure)) {
      times(&fpsEnd);
      cout << "FPS: " << (framesPerMeasure * ticksPerSecond) / (float)(fpsEnd.tms_utime - fpsBegin.tms_utime) << endl;
    }
    cvReleaseImage(&img);
  } while (cvWaitKey(33) != exitKeyCode);

  const float allTime = queryTime + transformTime + showTime;
  cout << "queryTime: " << queryTime * 100.0f / allTime << " %" << endl;
  cout << "transformTime: " << transformTime * 100.0f / allTime << " %" << endl;
  cout << "showTime: " << showTime * 100.0f / allTime << " %" << endl;

  cvReleaseCapture(&capture);
  cvDestroyWindow(winName);

  return EXIT_SUCCESS;
}

long calcUtime(std::function<void()> fn) {
  struct tms tBegin, tEnd;
  times(&tBegin);
  fn();
  times(&tEnd);
  return tEnd.tms_utime - tBegin.tms_utime;
}

inline char& getcoord(IplImage *img, int chan, size_t x, size_t y) {
  return img->imageData[y * img->widthStep + x * img->nChannels + chan];
}

void interp(IplImage *img, size_t x1, size_t y1, size_t w, size_t h) {
  const size_t x2 = x1 + w;
  const size_t y2 = y1 + h;

  for (int chan = 0; chan < img->nChannels; ++chan) {
    unsigned char 
      f11 = getcoord(img, chan, x1, y1),
      f12 = getcoord(img, chan, x1, y2),
      f21 = getcoord(img, chan, x2, y1),
      f22 = getcoord(img, chan, x2, y2);
    for (size_t y = y1; y < y2; ++y) 
      for (size_t x = x1; x < x2; ++x) 
        getcoord(img, chan, x, y) = (unsigned char)(
          ((int)f11 * (x2 - x) * (y2 - y) + 
           (int)f21 * (x - x1) * (y2 - y) +
           (int)f12 * (x2 - x) * (y - y1) +
           (int)f22 * (x - x1) * (y - y1)) / (int)((x2 - x1) * (y2 - y1)));
  }
}

