#include "MultiVideoCapture.hpp"
#include "VideoCaptureType.hpp"

#include <atomic>
std::atomic_bool camOpenCondition;
std::atomic_bool camSetCondition;

#include "ThreadPool.hpp"
ThreadPool::ThreadPool* pThread_pool = NULL;


std::vector<VideoCaptureType> gVidCaps;	// to hide in the MultiVideoCapture class


void openCameras(const std::atomic_bool& condition, std::vector<VideoCaptureType>& vidCaps, std::vector<int> camIds, int apiPreference) {
	const int nbDevs = (int)camIds.size();
	bool (VideoCaptureType::*openfunc)(int, int) = &VideoCaptureType::open;

	// check the camera status whether open or not
	int waitFor = 2000;
	while (condition) {
		std::vector<std::future<bool> > futures;
		for (int i = 0; i < nbDevs; i++) {
			if (vidCaps[i].status() == CAM_STATUS_CLOSED) {
				int id = camIds[i];
				futures.emplace_back(pThread_pool->EnqueueJob(openfunc, &vidCaps[i], id, apiPreference));
			}
		}

		// keep trying to open each camera in every [waitFor] sec.
		std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
	}
}


void setCamera(const std::atomic_bool& openCondition, std::atomic_bool& setCondition, VideoCaptureType& vidCap, cv::Size resolution, float fps) {
	bool (VideoCaptureType::*setfunc)(cv::Size, float) = &VideoCaptureType::set;

	int waitFor = 100;
	bool goSetting = true;
	setCondition.store(true);
	while (openCondition && setCondition) {
		if (vidCap.status() == CAM_STATUS_OPENED && goSetting == true) {
			pThread_pool->EnqueueJob(setfunc, &vidCap, resolution, fps);
			goSetting = false;
		}

		if (vidCap.status() == CAM_STATUS_CLOSED)
			goSetting = true;

		std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
	}
}


MultiVideoCapture::MultiVideoCapture() {
	mCameraIds.clear();
	mApiPreference = -1;
	mResolutions.clear();
	mFpses.clear();
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

	pThread_pool = new ThreadPool::ThreadPool(gVidCaps.size() * 2 + 4);

	mApiPreference = apiPreference;
	camOpenCondition.store(true);

	pThread_pool->EnqueueJob(openCameras, std::cref(camOpenCondition), std::ref(gVidCaps), cameraIds, mApiPreference);

	while (!isAnyOpened()) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "one of the cameras is open!" << std::endl;
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

	// wait until all jobs are done.
	for (int i = 0; i < futures.size(); i++) {
		futures[i].wait();
	}

	if (pThread_pool) {
		delete[] pThread_pool;
		pThread_pool = NULL;
	}
}


bool MultiVideoCapture::isOpened(int cameraNum) const {
	if (gVidCaps[cameraNum].isOpened() == true)
		return true;
	else
		return false;
}


bool MultiVideoCapture::isAnyOpened() const {
	for (int i = 0; i < (int)gVidCaps.size(); i++) {
		if (gVidCaps[i].isOpened() == true) {
			return true;
		}
	}

	return false;
}


bool MultiVideoCapture::isAllOpened() const {
	for (int i = 0; i < (int)gVidCaps.size(); i++) {
		if (gVidCaps[i].status() == false) {
			return false;
		}
	}

	return true;
}


bool MultiVideoCapture::read(std::vector<FrameType>& frames) {
	const int nbDevs = (int)gVidCaps.size();
	if (nbDevs != frames.size())
		frames.resize(nbDevs);

	std::vector<std::future<bool> > futures;
	bool (VideoCaptureType::*readfunc)(FrameType&) = &VideoCaptureType::read;

	for (int i = 0; i < nbDevs; i++) {
		if (gVidCaps[i].status() == CAM_STATUS_OPENED) {
			futures.emplace_back(pThread_pool->EnqueueJob(readfunc, &gVidCaps[i], std::ref(frames[i])));
		}
	}

	// wait until all jobs are done.
	bool status = false;
	for (int i = 0; i < futures.size(); i++) {
		futures[i].wait();
		status = status || futures[i].get();
	}

	return status;
}


MultiVideoCapture& MultiVideoCapture::operator >> (std::vector<FrameType>& frames) {
	read(frames);

	return *this;
}


bool MultiVideoCapture::set(cv::Size resolution, float fps) {
	const int nbDevs = (int)gVidCaps.size();
	bool status = true;
	for (int i = 0; i < nbDevs; i++) {
		bool status_set = this->set(mCameraIds[i], resolution, fps);
		status = status && status_set;
	}

	return status;
}



bool MultiVideoCapture::set(int cameraId, cv::Size resolution, float fps) {
	int id = std::find(mCameraIds.begin(), mCameraIds.end(), cameraId) - mCameraIds.begin();
	if (id >= mCameraIds.size())
		return false;

	if (mResolutions[id] == resolution && mFpses[id] == fps)
		return true;
	else
		camSetCondition.store(false);	// for terminating the thread that manages the previous setting.

	mResolutions[id] = resolution;
	mFpses[id] = fps;

	pThread_pool->EnqueueJob(setCamera, std::cref(camOpenCondition), std::ref(camSetCondition), std::ref(gVidCaps[id]), mResolutions[id], mFpses[id]);

	return true;
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
