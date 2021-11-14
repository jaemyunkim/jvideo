#include "MultiVideoCapture.hpp"
#include "VideoCaptureType.hpp"

#include <thread>
#include <atomic>
#include <mutex>
std::mutex gMtxCamRead;

#include "ThreadPool.hpp"
ThreadPool::ThreadPool* pThread_pool = NULL;
std::atomic_bool camOpenCondition;


std::vector<VideoCaptureType> gVidCaps;	// to hide in the MultiVideoCapture class


void openCameras(std::atomic_bool& condition, std::vector<VideoCaptureType>& vidCaps, std::vector<int> camIds, int apiPreference) {
	const int nbDevs = (int)camIds.size();
	bool (VideoCaptureType::*openfunc)(int, int) = &VideoCaptureType::open;

	// check the camera status whether open or not
	int waitFor = 2000;
	while (condition) {
		std::vector<std::future<bool> > futures;
		for (int i = 0; i < nbDevs; i++) {
			if (vidCaps[i].status() != CAM_STATUS_OPENED) {
				int id = camIds[i];
				futures.emplace_back(pThread_pool->EnqueueJob(openfunc, &vidCaps[i], id, apiPreference));
			}
		}

		// wait until the previous jobs are done.
		for (int i = 0; i < futures.size(); i++)
			futures[i].wait();
	}
}


MultiVideoCapture::MultiVideoCapture() {
	camOpenCondition.store(true);
	mApiPreference = -1;
	mResolution = { 640, 480 };
	mFps = 30.f;
}


MultiVideoCapture::~MultiVideoCapture() {
	release();
}


void MultiVideoCapture::open(std::vector<int> cameraIds) {
	this->open(cameraIds, -1);
}


void MultiVideoCapture::open(std::vector<int> cameraIds, int apiPreference) {
	this->resize(cameraIds.size());

	pThread_pool = new ThreadPool::ThreadPool(gVidCaps.size() * 2 + 2);

	mApiPreference = apiPreference;
	camOpenCondition.store(true);

	pThread_pool->EnqueueJob(openCameras, std::ref(camOpenCondition), std::ref(gVidCaps), cameraIds, mApiPreference);

	while (!isAnyOpened()) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "any cam is opened!" << std::endl;
}


void MultiVideoCapture::release() {
	// stop thread flag
	camOpenCondition.store(false);
	mApiPreference = -1;

	const int nbDevs = (int)gVidCaps.size();
	void (VideoCaptureType::*releasefunc)() = &VideoCaptureType::release;
	std::vector<std::future<void> > futures;

	for (int i = 0; i < nbDevs; i++) {
		if (gVidCaps[i].status() != CAM_STATUS_CLOSED) {
			futures.emplace_back(pThread_pool->EnqueueJob(releasefunc, &gVidCaps[i]));
		}
	}

	// wait until the previous jobs are done.
	for (int i = 0; i < futures.size(); i++)
		futures[i].wait();

	if (pThread_pool) {
		//pThread_pool->~ThreadPool();
		//pThread_pool->destroy();
		delete[] pThread_pool;
		pThread_pool = NULL;
	}
}


bool MultiVideoCapture::isOpened(int cameraNum) const {
	if (gVidCaps[cameraNum].isOpened() == CAM_STATUS_OPENED)
		return true;
	else
		return false;
}


bool MultiVideoCapture::isAnyOpened() const {
	for (int i = 0; i < (int)gVidCaps.size(); i++) {
		if (gVidCaps[i].status() == CAM_STATUS_OPENED) {
			return true;
		}
	}

	return false;
}


bool MultiVideoCapture::isAllOpened() const {
	for (int i = 0; i < (int)gVidCaps.size(); i++) {
		if (gVidCaps[i].status() != CAM_STATUS_OPENED) {
			return false;
		}
	}

	return true;
}


bool MultiVideoCapture::read(std::vector<FrameType>& images) {
	std::lock_guard<std::mutex> lock(gMtxCamRead);

	const int nbDevs = (int)gVidCaps.size();
	if (nbDevs != images.size())
		images.resize(nbDevs);

	std::vector<std::future<bool> > futures;
	bool (VideoCaptureType::*readfunc)(FrameType&) = &VideoCaptureType::read;

	for (int i = 0; i < nbDevs; i++) {
		if (gVidCaps[i].status() == CAM_STATUS_OPENED) {
			futures.emplace_back(pThread_pool->EnqueueJob(readfunc, &gVidCaps[i], std::ref(images[i])));
		}
	}

	bool status = false;
	for (int i = 0; i < nbDevs; i++) {
		if (!images[i].empty()) {
			status = true;
		}
		else {
			images[i].mat() = cv::Mat::zeros(mResolution, CV_8UC3);
		}
	}

	return status;
}


MultiVideoCapture& MultiVideoCapture::operator >> (std::vector<FrameType>& images) {
	read(images);

	return *this;
}


bool MultiVideoCapture::set(cv::Size resolution, float fps) {
	mResolution = resolution;
	mFps = fps;

	const int nbDevs = (int)gVidCaps.size();
	std::vector<bool> status(nbDevs);

	for (int i = 0; i < nbDevs; i++) {
		status[i] = gVidCaps[i].set(resolution, fps);
	}

	return true;
}


void MultiVideoCapture::resize(size_t size) {
	if (gVidCaps.size() != size) {
		release();
		gVidCaps.resize(size);
	}
}
