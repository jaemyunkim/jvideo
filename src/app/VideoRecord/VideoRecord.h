#ifndef	VIDEO_RECORD_H_
#define	VIDEO_RECORD_H_


#include <iostream>
#include <regex>

#include "FrameGrab.h"
#include "FrameRecord.h"
#include "getopt.h"

// use opencv
#include "opencv2/opencv.hpp"

// boost filesystem
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;


/*
*	@class	VideoRecord
*	@brief	Video recorder class.
*	@authro	Jaemyun Kim.
*	@date	31 Oct. 2018
*/
class VideoRecord
{
public:
	/*
	*	@brief	Default constructor.
	*/
	VideoRecord();
	
	/**
	*	@brief	Destructor.
	*/
	virtual ~VideoRecord();

	/**
	*	@brief	Usage checking.
	*/
	void usage(int argc, char **argv);

	/**
	*	@brief	Usage message.
	*/
	void usageMessage(std::string name, std::ostream &out);

	/**
	*	@brief	Set a source name.
	*	@param[in]	srcName	Source name.
	*/
	void setSource(fs::path srcName)
	{
		mFg.setSource(srcName);
	}

	/**
	*	@brief	Set a source name.
	*	@param[in]	id	Device id.
	*/
	void setSource(int id)
	{
		mFg.setSource(id);
	}

	/**
	*	@brief	Set a filename of video file.
	*	@param[in]	fName	File name.
	*/
	void setFilename(fs::path fName)
	{
		mFr.setFilename(fName);
	}

	/**
	*	@brief	Set a Record video resolution.
	*	@param[in]	size	Desired video resolution.
	*/
	void setRecordResolution(std::string resolution);

	/**
	*	@brief	Run video record.
	*/
	virtual void run();// override;

protected:
	FrameGrab mFg;	///< Frame brabber.
	FrameRecord mFr;	///< Frame recorder.

	int mFrameCount;	///< frame counter that counts a number of frames.
	double mFps;	///< Frame per seconds.
	cv::Size mRecordResolution;	///< Resolution of the frame.
};


#endif // !VIDEO_RECORD_H_
