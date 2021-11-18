#ifndef	FRAME_GRAB_H_
#define	FRAME_GRAB_H_


#include <list>

// use opencv
#include "opencv2/opencv.hpp"


/**
* @class	FrameQueue
* @brief	Frame queue class
* @author	Jaemyun Kim
* @date		23 Dec. 2020
*/
class FrameQueue {
public:
	/**
	* @brief	Default contructor
	* @param[in]	size Size of the queue
	*/
	FrameQueue(size_t size = 10);

	/**
	* @brief	Destructor
	*/
	~FrameQueue();

	/**
	* @brief	Clear member variables
	*/
	void clear();

	/**
	* @brief	Check the queue is full
	*/
	bool full();

	/**
	* @brief	Check the queue is empty
	*/
	bool empty();

	/**
	* @brief	Get the number of elements in the queue
	*/
	size_t size();

	/**
	* @brief	Get frame count
	* @return	Frame count
	*/
	int getFrameCount();

	/**
	* @biref	Resize the queue
	* @param[in]	newsize	Size of the queue
	*/
	void resize(size_t newsize);

	/**
	* @brief	Insert a new frame into last location in the queue
	* @param[in]	frame	A new frame to be inserted
	*/
	void enqueue(const cv::Mat &frame);
	
	/**
	* @brief	Get the last frame in the queue
	* @return	Last frame
	*/
	cv::Mat getLastFrame();

	/**
	* @brief	Get all frames in the queue
	* @return	A list of the frame queue
	*/
	const std::list<cv::Mat>& getAllFrames();

protected:
	std::list<cv::Mat> mFrames;	///< List as the frame queue
	size_t mCapicity;	///< Maximum size of the queue
	int mFrameCount;	///< Frame counter
};


#endif //!FRAME_GRAB_H_