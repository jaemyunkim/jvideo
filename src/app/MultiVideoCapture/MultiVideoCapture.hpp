#ifndef MULTI_VIDEO_CAPTURE_H_
#define MULTI_VIDEO_CAPTURE_H_


#ifndef __cplusplus
#  error MultiVideoCapture.hpp header must be compiled as C++
#endif

#ifndef MULTIVIDEOCAPTURE_EXPORTS
#  ifdef DLL_EXPORTS
#    if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
#      define MULTIVIDEOCAPTURE_EXPORTS __declspec(dllexport)
#    elif defined __GNUC__ && __GNUC__ >= 4 || defined(__APPLE__)
#      define MULTIVIDEOCAPTURE_EXPORTS __attribute__ ((visibility ("default")))
#      define FRAMETYPE_TEMPLATE
#    endif
#  else
#    define MULTIVIDEOCAPTURE_EXPORTS __declspec(dllimport)
#  endif	// !DLL_EXPORTS
#endif	// !MULTIVIDEOCAPTURE_EXPORTS


#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "FrameType.hpp"


class MULTIVIDEOCAPTURE_EXPORTS MultiVideoCapture {
public:
	MultiVideoCapture();
	virtual ~MultiVideoCapture();

	virtual void open(std::vector<int> cameraIds);
	virtual void open(std::vector<int> cameraIds, int apiPerference);
	virtual void release();
    
	virtual bool isOpened(int cameraNum) const;
	virtual bool isAnyOpened() const;
	virtual bool isAllOpened() const;

	virtual bool read(std::vector<FrameType>& images);
	virtual MultiVideoCapture& operator >> (std::vector<FrameType>& images);

	virtual bool set(cv::Size resolution = { 640, 480 }, float fps = 30.f);

protected:
	virtual void resize(size_t size);

protected:
    int mApiPreference;
    cv::Size mResolution;
    float mFps;
};


#endif // ~MULTI_VIDEO_CAPTURE_H_
