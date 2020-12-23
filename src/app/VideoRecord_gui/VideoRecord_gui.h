#ifndef	VIDEO_RECORD_GUI_H_
#define	VIDEO_RECORD_GUI_H_


#include "ui_VideoRecord_gui.h"
#include <QtConcurrent>
#include <QDebug>

#include <iostream>

// use opencv
#include "opencv2/opencv.hpp"

// boost filesystem
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

// OpenCVDeviceEnumerator
#ifdef WIN32
#include "DeviceEnumerator.h"
#endif // !WIN32

#include "FrameGrab.h"
#include "FrameRecord.h"


/**
* @class	VideoRecord_gui
* @brief	Video recorder class.
* @author	Jaemyun Kim.
* @date		31 Oct. 2018
*/
class VideoRecord_gui : public QWidget, private Ui::jVdRec
{
	Q_OBJECT
public:
	/*
	*	@brief	Default constructor.
	*/
	VideoRecord_gui(QWidget *parent = 0);
	
	/**
	*	@brief	Destructor.
	*/
	virtual ~VideoRecord_gui();

	/**
	*	@brief	Display current frame.
	*/
	void displayImage();

	/**
	*	@brief	Refresh fps.
	*/
	void refreshFps();

	/**
	*	@brief	Refresh resolution for record.
	*/
	void refreshRecordResolution();

	/**
	*	@brief	Start frame grab.
	*/
	virtual void start();

	/**
	*	@brief	Stop frame grab.
	*/
	virtual void stop();

private slots:
	void handleComobBoxChangedSource();
	void handleComboBoxChangedDisplayResolution(int index);
	void handleComboBoxChangedRecordResolution(int index);
	void handleButtonSource();
	void handleButtonRecord();
	void handleButtonFileDialog();
	void startSource();
	void stopSource();
	void startRecord();
	void stopRecord();
	void worker();

signals:
	void signalStartSource();
	void signalStopSource();
	void signalStartRecord();
	void signalStopRecord();

protected:
	//Ui::jVdRec ui;

	QFutureWatcher<void> mWatcher;
	QFuture<void> mFuture;

	bool mIsWork;
	bool mGrabWork;
	bool mBtnSource;
	bool mBtnRecord;
	
	fs::path mSrcName;
	fs::path mFName;
	cv::Mat mFrame;
	long mFrameCount;

	double mFps;
	cv::Size mRecordResolution;

	FrameGrab mGrab;
	FrameRecord mRec;

#ifdef WIN32
	std::map<int, Device> mDevices;	///< List connected devices.
#endif // !WIN32
};



#endif // !VIDEO_RECORD_GUI_H_
