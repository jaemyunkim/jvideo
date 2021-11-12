#ifndef FRAME_TYPE_H_
#define FRAME_TYPE_H_


#include "opencv2/opencv.hpp"


class FrameType {
public:
	FrameType();
	~FrameType();

	FrameType clone() const;
	cv::Mat& mat();
	void copyTo(FrameType& obj);
	bool empty() const;

	bool setFrame(const cv::Mat& frame);
	bool setFrame(const cv::Mat& frame, std::chrono::system_clock::time_point timestamp);
	void setTimestamp(std::chrono::system_clock::time_point timestamp);
	cv::Mat getFrame() const;
	std::chrono::system_clock::time_point timestamp() const;

	void release();

protected:
	cv::Mat mFrame;
	std::chrono::system_clock::time_point mTimestamp;
};


#endif // !FRAME_TYPE_H_
