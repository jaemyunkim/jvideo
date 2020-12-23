#ifndef	FRAME_RECORD_H_
#define	FRAME_RECORD_H_


#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <chrono>

// use opencv
#include "opencv2/opencv.hpp"

// boost filesystem
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;


/**
* @class	FrameRecord
* @brief	Frame record class.
* @author	Jaemyun Kim.
* @date		17 Nov. 2018
*/
class FrameRecord
{
public:
	/*
	*	@brief	Default constructor.
	*/
	FrameRecord();
	
	/**
	*	@brief	Destructor.
	*/
	virtual ~FrameRecord();

	/**
	*	@brief	Set a filename of video file.
	*	@param[in]	fName	File name.
	*/
	void setFilename(fs::path fName)
	{
		mFName = fName;
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
	*	@brief	Set frame per second.
	*/
	void setFps(double fps)
	{
		mFps = fps;
	}

	/**
	*	@brief	Set resolution.
	*/
	void setResolution(cv::Size resolution)
	{
		mRecordResolution = resolution;
	}

	/**
	*	@brief	Set record information.
	*/
	void setRecordInfo(double fps, cv::Size resolution)
	{
		setFps(fps);
		setResolution(resolution);
	}

	/**
	*	@brief	Get status that is ready
	*/
	bool isReady()
	{
		return mIsReady;
	}

	/**
	*	@brief	Open file.
	*/
	void openFile();

	/**
	*	@brief	Write file.
	*/
	void writeFile();

	/**
	*	@brief	Close file.
	*/
	void closeFile();

	/**
	*	@brief	Start video record.
	*	@param[in]	frame	Frame buffer for video writing.
	*/
	virtual void start(const cv::Mat &frame);

	/**
	*	@brief	Stop video record.
	*/
	virtual void stop();

protected:
	/**
	*	@brief	Run video record.
	*	@param[in]	frame	Frame buffer for video writing.
	*/
	virtual void run(const cv::Mat &frame);

protected:
	std::thread mThread;	///< processing thread to record frame.
	std::mutex mFrameMutex;	///< Mutex for recording frame.

protected:
	bool mIsRecord;	///< Status of repeat that records the frame into a video file.
	bool mIsReady;	///< Status of frame recording.
	cv::Mat mFrame;	///< Image buffer having frame to be written into a video file..
	cv::VideoWriter mOutputCap;	///< Video recorder.
	long mFrameCount;	///< frame counter that counts a number of frames.

	double mFps;	///< Frame per seconds.
	cv::Size mRecordResolution;	///< Resolution of the frame.

	fs::path mFName;	///< File name for a video file.
};


#endif // !FRAME_GRAB_H_
