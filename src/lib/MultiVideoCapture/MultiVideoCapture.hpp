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
#    if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
#    define MULTIVIDEOCAPTURE_EXPORTS __declspec(dllimport)
#    elif defined __GNUC__ && __GNUC__ >= 4 || defined(__APPLE__)
#      define MULTIVIDEOCAPTURE_EXPORTS
#    endif
#  endif	// !DLL_EXPORTS
#endif	// !FRAMETYPE_EXPORTS


#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "FrameType.hpp"


class MULTIVIDEOCAPTURE_EXPORTS MultiVideoCapture {
public:
	MultiVideoCapture();
	virtual ~MultiVideoCapture();

	virtual void open(std::vector<int> cameraIds);
	virtual void open(std::vector<int> cameraIds, int apiPreference);
	virtual void release();
    
	virtual bool isOpened(int cameraNum) const;
	virtual bool isAnyOpened() const;
	virtual bool isAllOpened() const;

	virtual bool read(std::vector<FrameType>& frames);
	virtual MultiVideoCapture& operator >> (std::vector<FrameType>& frames);

	virtual bool set(cv::Size resolution, float fps);

protected:
	virtual void resize(size_t size);
	virtual bool set(int cameraId, cv::Size resolution, float fps);

protected:
	std::vector<int> mCameraIds;
    int mApiPreference;

	std::vector<cv::Size> mResolutions;
	std::vector<float> mFpses;
};


#endif // ~MULTI_VIDEO_CAPTURE_H_
