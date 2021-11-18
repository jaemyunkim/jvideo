#include "FrameQueue.h"


// Default contructor
FrameQueue::FrameQueue(size_t size) {
	clear();
	resize(size);
}


// Destructor
FrameQueue::~FrameQueue() {
	clear();
}


// Clear member variables
void FrameQueue::clear() {
	mFrames.clear();
	mFrameCount = 0;
}


// Check the queue is full
bool FrameQueue::full() {
	return mFrames.size() == mCapicity;
}


// Check the queue is empty
bool FrameQueue::empty() {
	return mFrames.empty();
}


// Get the number of elements in the queue
size_t FrameQueue::size() {
	return mFrames.size();
}


// Get frame counter
int FrameQueue::getFrameCount() {
	return mFrameCount;
}


// Resize the queue
void FrameQueue::resize(size_t newsize) {
	mFrames.resize(newsize);
	mCapicity = newsize;
}


// Insert a new frame into last location in the queue
void FrameQueue::enqueue(const cv::Mat &frame)
{
	if (mFrames.size() >= mCapicity)
		mFrames.pop_front();

	mFrames.push_back(frame);
	mFrameCount++;
}


// Get the last frame in the queue
cv::Mat FrameQueue::getLastFrame() {
	try {
		if (mFrames.empty())
			throw "No frame in frame queue";
		return mFrames.back();
	}
	catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return mFrames.back().clone();
}


// Get all frames in the queue
const std::list<cv::Mat> &FrameQueue::getAllFrames() {
	return mFrames;
}
