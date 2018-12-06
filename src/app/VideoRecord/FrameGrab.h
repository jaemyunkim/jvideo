#ifndef	FRAME_GRAB_H_
#define	FRAME_GRAB_H_


#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// use opencv
#include "opencv2/opencv.hpp"

// boost filesystem
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;


/*
*	@class	FrameGrab
*	@brief	Frame grab class.
*	@authro	Jaemyun Kim.
*	@date	05 Nov. 2018
*/
class FrameGrab
{
public:
	/*
	*	@brief	Default constructor.
	*/
	FrameGrab();
	
	/**
	*	@brief	Destructor.
	*/
	virtual ~FrameGrab();

	/**
	*	@brief	Set a source name.
	*	@param[in]	srcName	Source name.
	*/
	void setSource(fs::path srcName)
	{
		mSourceMode = 0;
		mDevId = -1;
		mSrcName = srcName;
	}

	/**
	*	@brief	Set a source name.
	*	@param[in]	id	Device id.
	*/
	void setSource(int id)
	{
		mSourceMode = 1;
		mSrcName.empty();
		mDevId = id;
	}

	/**
	*	@brief	Get status that is ready
	*/
	bool isReady()
	{
		return mIsReady;
	}

	/**
	*	@brief	Set an image frame buffer.
	*	@return	Image frame buffer
	*/
	const cv::Mat getFrame()
	{
		return mFrame;
	}

	/**
	*	@brief	Get frame count.
	*	@return	Frame count.
	*/
	const long getFrameCount()
	{
		return mFrameCount;
	}

	/**
	*	@brief	Get a fps for video.
	*/
	const double getFps()
	{
		return mFps;
	}

	/**
	*	@brief	Get a frame resultion.
	*/
	const cv::Size getResolution()
	{
		return mResolution;
	}

	/**
	*	@brief	Open source.
	*/
	void openSource();

	/**
	*	@brief	Read frame.
	*/
	void readFrame();

	/**
	*	@brief	Close source.
	*/
	void closeSource();

	/**
	*	@brief	Start frame grab.
	*	@param[in]	frame	Frame buffer to be shared.
	*/
	virtual void start(cv::Mat &frame);

	/**
	*	@brief	Stop frame grab.
	*/
	virtual void stop();

protected:
	/**
	*	@brief	Run video record.
	*	@param[in]	frame	Frame buffer to be shared.
	*/
	virtual void run(cv::Mat &frame);

protected:
	std::thread mThread;	///< processing thread to grab frame.
	std::mutex mFrameMutex;	///< Mutex for reading frame.

protected:
	int mSourceMode;	///< Mode of source. 0: url, 1: device id.
	fs::path mSrcName;	///< Source url.
	int mDevId;	///< Source that is a device id.

	bool mIsGrab;	///< Status of repeat that frame grab from the source.
	bool mIsReady;	///< Status of frame grabbing.
	cv::Mat mFrame;	///< Image buffer having frame from the source.
	cv::VideoCapture mInputCap;	///< Video frame grabber.
	long mFrameCount;	///< frame counter that counts a number of frames.

	double mFps;	///< Frame per seconds.
	cv::Size mResolution;	///< Resolution of the frame.
};


#endif // !FRAME_GRAB_H_
