#include "FrameType.hpp"


inline
FrameType::FrameType() {
	release();
}


inline
FrameType::~FrameType() {
	release();
}


inline
FrameType FrameType::clone() const {
	FrameType obj;
	obj.mFrame = this->mFrame.clone();
	obj.mTimestamp = this->mTimestamp;

	return obj;
}


inline
void FrameType::copyTo(FrameType& obj) {
	this->mFrame.copyTo(obj.mFrame);
	obj.mTimestamp = this->mTimestamp;
}


inline
bool FrameType::empty() const {
	return mFrame.empty();
}


inline
bool FrameType::setFrame(const cv::Mat& frame) {
	return setFrame(frame, std::chrono::system_clock::now());
}


inline
bool FrameType::setFrame(const cv::Mat& frame, std::chrono::system_clock::time_point timestamp) {
	mFrame = frame.clone();
	mTimestamp = timestamp;
	return true;
}


inline
void FrameType::setTimestamp(std::chrono::system_clock::time_point timestamp) {
	mTimestamp = timestamp;
}


inline
cv::Mat FrameType::frame() const {
	return mFrame.clone();
}


inline
cv::Mat& FrameType::mat() {
	return mFrame;
}


inline
std::chrono::system_clock::time_point FrameType::timestamp() const {
	return mTimestamp;
}


inline
void FrameType::release() {
	mFrame.release();
	mTimestamp = std::chrono::system_clock::time_point();
}
