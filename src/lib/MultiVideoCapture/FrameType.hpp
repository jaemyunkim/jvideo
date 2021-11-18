#ifndef FRAME_TYPE_H_
#define FRAME_TYPE_H_


#ifndef __cplusplus
#  error Frametype.hpp header must be compiled as C++
#endif


#ifndef FRAMETYPE_EXPORTS
#  ifdef DLL_EXPORTS
#    if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
#      define FRAMETYPE_EXPORTS __declspec(dllexport)
#    elif defined __GNUC__ && __GNUC__ >= 4 || defined(__APPLE__)
#      define FRAMETYPE_EXPORTS __attribute__ ((visibility ("default")))
#    endif
#    define FRAMETYPE_TEMPLATE
#  else
#    if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
#      define FRAMETYPE_EXPORTS __declspec(dllimport)
#    elif defined __GNUC__ && __GNUC__ >= 4 || defined(__APPLE__)
#      define FRAMETYPE_EXPORTS
#    endif
#    define FRAMETYPE_TEMPLATE extern
#  endif	// !DLL_EXPORTS
#endif	// !FRAMETYPE_EXPORTS


#include <chrono>
#include "opencv2/opencv.hpp"


FRAMETYPE_TEMPLATE template class FRAMETYPE_EXPORTS std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>;
FRAMETYPE_TEMPLATE template class FRAMETYPE_EXPORTS std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>;


class FRAMETYPE_EXPORTS FrameType {
public:
	FrameType();
	virtual ~FrameType();

	virtual FrameType clone() const;
	virtual void copyTo(FrameType& obj);
	virtual bool empty() const;

	virtual bool setFrame(const cv::Mat& frame);
	virtual bool setFrame(const cv::Mat& frame, std::chrono::system_clock::time_point timestamp);
	virtual void setTimestamp(std::chrono::system_clock::time_point timestamp);
	virtual cv::Mat frame() const;
	virtual cv::Mat& mat();
	virtual std::chrono::system_clock::time_point timestamp() const;

	virtual void release();

protected:
	cv::Mat mFrame;
	std::chrono::system_clock::time_point mTimestamp;
};


#endif // !FRAME_TYPE_H_
