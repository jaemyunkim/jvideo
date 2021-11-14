#include "VideoCaptureType.hpp"

#include <mutex>
std::mutex gMtxPrintMsg;


VideoCaptureType::VideoCaptureType() {
	this->release();
	mResolution = { 640, 480 };
	mFps = 30.f;
}


VideoCaptureType::~VideoCaptureType() {
	this->release();
}


bool VideoCaptureType::open(int index) {
	return this->open(index, -1);
}


bool VideoCaptureType::open(int index, int apiPreference) {
	// check camera status
	if (mStatus == CAM_STATUS_OPENED) {
		std::lock_guard<std::mutex> lock(gMtxPrintMsg);
		std::cout << "camera " << index << " is already opened" << std::endl;
		return false;
	}
	else if (mStatus == CAM_STATUS_OPENING) {
		std::lock_guard<std::mutex> lock(gMtxPrintMsg);
		std::cout << "camera " << index << " is opening" << std::endl;
		return false;
	}

	// try to open the camera
	mStatus = CAM_STATUS_OPENING;
	bool cam_status = false;
	if (apiPreference == -1)
		cam_status = cv::VideoCapture::open(index);
	else
		cam_status = cv::VideoCapture::open(index, apiPreference);

	if (cam_status == true && cv::VideoCapture::grab() == true) {
		mCamId = index;

		// disable autofocus
		if (cv::VideoCapture::get(cv::CAP_PROP_AUTOFOCUS) != 0) {
			cv::VideoCapture::set(cv::CAP_PROP_AUTOFOCUS, 0);
		}
		set(mResolution, mFps);

		mStatus = CAM_STATUS_OPENED;
	}
	else {
		release();
		std::lock_guard<std::mutex> lock(gMtxPrintMsg);
		std::cout << "can't open the camera " << index << std::endl;
	}

	return cam_status;
}


bool VideoCaptureType::isOpened() const {
	if (mStatus == CAM_STATUS_OPENED)
		return true;
	else
		return false;
}


CamStatus VideoCaptureType::status() const {
	return mStatus;
}


void VideoCaptureType::release() {
	mStatus = CAM_STATUS_CLOSED;
	mCamId = -1;

	cv::VideoCapture::release();
}


bool VideoCaptureType::retrieve(FrameType& frame, int flag) {
	bool status = cv::VideoCapture::retrieve(frame.mat(), flag);
	frame.setTimestamp(std::chrono::system_clock::now());

	return status;
}


bool VideoCaptureType::read(FrameType& image) {
	if (cv::VideoCapture::grab()) {
		this->retrieve(image);
	}
	else {
		release();
		image.release();
	}

	return !image.empty();
}


VideoCaptureType& VideoCaptureType::operator >> (FrameType& image) {
	read(image);

	return *this;
}


bool VideoCaptureType::set(cv::Size resolution, float fps) {
	mResolution = resolution;
	mFps = fps;

	// get old settings
	cv::Size oldSize((int)cv::VideoCapture::get(cv::CAP_PROP_FRAME_WIDTH), (int)cv::VideoCapture::get(cv::CAP_PROP_FRAME_HEIGHT));
	double oldFps = cv::VideoCapture::get(cv::CAP_PROP_FPS);
	double oldAutofocus = cv::VideoCapture::get(cv::CAP_PROP_AUTOFOCUS);

	// set resolution and fps
	bool statusSize= true, statusFps = true;
	if (resolution != oldSize) {
		statusSize = 
			cv::VideoCapture::set(cv::CAP_PROP_FRAME_WIDTH, resolution.width) &&
			cv::VideoCapture::set(cv::CAP_PROP_FRAME_HEIGHT, resolution.height);
	}
	if (fps != oldFps) {
		statusFps = cv::VideoCapture::set(cv::CAP_PROP_FPS, fps);
	}

	if (statusSize && statusFps) {
		return true;
	}
	else {
		// rollback resolution
		cv::VideoCapture::set(cv::CAP_PROP_FRAME_WIDTH, oldSize.width);
		cv::VideoCapture::set(cv::CAP_PROP_FRAME_HEIGHT, oldSize.height);

		// rollback fps
		cv::VideoCapture::set(cv::CAP_PROP_FPS, oldFps);

		return false;
	}
}
