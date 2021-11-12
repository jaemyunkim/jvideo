#ifndef MULTI_VIDEO_CAPTURE_H_
#define MULTI_VIDEO_CAPTURE_H_


#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "FrameType.hpp"
#include "VideoCaptureType.hpp"


class MultiVideoCapture {
public:
    MultiVideoCapture();
    ~MultiVideoCapture();

    void open(std::vector<int> cameraIds);
    void open(std::vector<int> cameraIds, int apiPerference);
    void release();
    
    bool isOpened(int cameraNum) const;
    bool isAnyOpened() const;
    bool isAllOpened() const;

    bool read(std::vector<FrameType>& images);
    virtual MultiVideoCapture& operator >> (std::vector<FrameType>& images);

    virtual bool set(cv::Size resolution = {640, 480}, float fps = 30.f);

protected:
    void resize(size_t size);

protected:
    std::vector<VideoCaptureType> mVidCaps;
    int mApiPreference;
    cv::Size mResolution;
    float mFps;
};


#endif // ~MULTI_VIDEO_CAPTURE_H_
