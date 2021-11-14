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
		for (int i = 0; i < futures.size(); i++) {
			futures[i].wait();
		}

		// keep trying to open each camera in every [waitFor] sec.
		std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
	}
}


bool setCamera(VideoCaptureType& vidCaps, cv::Size resolution, float fps) {
	int waitFor = 100;
	// wait if the status of camera is CAM_STATUS_OPENING
	while (vidCaps.status() == CAM_STATUS_OPENING) {
		std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
	}

	bool status = false;
	if (vidCaps.status() == CAM_STATUS_OPENED) {
		status = vidCaps.set(resolution, fps);
	}
	else if (vidCaps.status() == CAM_STATUS_CLOSED) {
		// noting to do
	}

	return status;
}


MultiVideoCapture::MultiVideoCapture() {
	camOpenCondition.store(true);
	mCameraIds.clear();
	mApiPreference = -1;
	mResolutions.clear();
	mFpses.clear();
	//mResolution = { 640, 480 };
	//mFps = 30.f;
}


MultiVideoCapture::~MultiVideoCapture() {
	release();
}


void MultiVideoCapture::open(std::vector<int> cameraIds) {
	this->open(cameraIds, -1);
}


void MultiVideoCapture::open(std::vector<int> cameraIds, int apiPreference) {
	this->resize(cameraIds.size());
	mCameraIds = cameraIds;

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
			images[i].mat() = cv::Mat::zeros(mResolutions[i], CV_8UC3);
		}
	}

	return status;
}


MultiVideoCapture& MultiVideoCapture::operator >> (std::vector<FrameType>& images) {
	read(images);

	return *this;
}


bool MultiVideoCapture::set(cv::Size resolution, float fps) {
	const int nbDevs = (int)gVidCaps.size();
	std::vector<std::future<bool> > futures;
	for (int i = 0; i < nbDevs; i++) {
		futures.emplace_back(pThread_pool->EnqueueJob(setCamera, std::ref(gVidCaps[i]), resolution, fps));
	}

	for (int i = 0; i < futures.size(); i++) {
		futures[i].wait();
	}

	bool status = true;
	for (int i = 0; i < futures.size(); i++) {
		if (futures[i].get() == true) {
			mResolutions[i] = resolution;
			mFpses[i] = fps;
		}
		else
			status = false;
	}

	//const int nbDevs = (int)gVidCaps.size();
	//std::vector<bool> status(nbDevs);

	//for (int i = 0; i < nbDevs; i++) {
	//	status[i] = gVidCaps[i].set(resolution, fps);
	//}

	return status;
}


bool MultiVideoCapture::set(int cameraId, cv::Size resolution, float fps) {
	int id = std::find(mCameraIds.begin(), mCameraIds.end(), cameraId) - mCameraIds.begin();
	if (id >= mCameraIds.size())
		return false;

	std::future<bool> future;
	future = pThread_pool->EnqueueJob(setCamera, std::ref(gVidCaps[id]), resolution, fps);
	future.wait();

	bool status = future.get();
	if (status == true) {
		mResolutions[id] = resolution;
		mFpses[id] = fps;
	}

	return status;
}


void MultiVideoCapture::resize(size_t size) {
	if (gVidCaps.size() != size) {
		release();
		gVidCaps.resize(size);
		mCameraIds.resize(size, -1);
		mResolutions.resize(size);
		mFpses.resize(size);
		for (int i = 0; i < size; i++) {
			mResolutions[i] = { (int)gVidCaps[i].get(cv::CAP_PROP_FRAME_WIDTH), (int)gVidCaps[i].get(cv::CAP_PROP_FRAME_HEIGHT) };
			mFpses[i] = gVidCaps[i].get(cv::CAP_PROP_FPS);
		}
	}
}
