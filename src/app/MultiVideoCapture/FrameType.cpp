#include "FrameType.hpp"


FrameType::FrameType() {
	release();
}


FrameType::~FrameType() {
	release();
}


FrameType FrameType::clone() const {
	FrameType obj;
	obj.mFrame = this->mFrame.clone();
	obj.mTimestamp = this->mTimestamp;

	return obj;
}


cv::Mat& FrameType::mat() {
	return mFrame;
}


void FrameType::copyTo(FrameType& obj) {
	this->mFrame.copyTo(obj.mFrame);
	obj.mTimestamp = this->mTimestamp;
}


bool FrameType::empty() const {
	return mFrame.empty();
}


bool FrameType::setFrame(const cv::Mat& frame) {
	return setFrame(frame, std::chrono::system_clock::now());
}


bool FrameType::setFrame(const cv::Mat& frame, std::chrono::system_clock::time_point timestamp) {
	mFrame = frame.clone();
	mTimestamp = timestamp;
	return true;
}


void FrameType::setTimestamp(std::chrono::system_clock::time_point timestamp) {
	mTimestamp = timestamp;
}


cv::Mat FrameType::getFrame() const {
	return mFrame.clone();
}


std::chrono::system_clock::time_point FrameType::timestamp() const {
	return mTimestamp;
}


void FrameType::release() {
	mFrame.release();
	mTimestamp = std::chrono::system_clock::time_point();
}
