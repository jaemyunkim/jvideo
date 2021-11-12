#include "MultiVideoCapture.hpp"

#include <thread>
#include <atomic>
#include <mutex>
std::mutex gMtxCamRead;

#include "ThreadPool.hpp"
ThreadPool::ThreadPool thread_pool(8);
std::atomic_bool camOpenCondition;


void openCameras(std::atomic_bool& condition, std::vector<VideoCaptureType>& vidCaps, std::vector<int> camIds, int apiPreference) {
    const int nbDevs = (int)camIds.size();
    bool (VideoCaptureType::*openfunc)(int, int) = &VideoCaptureType::open;

    // check the camera status whether open or not
    int waitFor = 2000;
    while(condition) {
        std::vector<std::future<bool> > futures;
        for (int i = 0; i < nbDevs; i++) {
            if (vidCaps[i].status() != CAM_STATUS_OPENED) {
                int id = camIds[i];
				futures.emplace_back(thread_pool.EnqueueJob(openfunc, &vidCaps[i], id, apiPreference));
            }
        }

        // wait until the previous jobs are done.
        for (int i = 0; i < futures.size(); i++)
            futures[i].wait();

        // keep trying to open each camera in every [waitFor] sec.
		std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
    }
}


MultiVideoCapture::MultiVideoCapture() {
    camOpenCondition.store(true);
    mApiPreference = -1;
}


MultiVideoCapture::~MultiVideoCapture() {
    release();
    thread_pool.~ThreadPool();
}


void MultiVideoCapture::open(std::vector<int> cameraIds) {
    this->open(cameraIds, -1);
}


void MultiVideoCapture::open(std::vector<int> cameraIds, int apiPreference) {
    this->resize(cameraIds.size());

    mApiPreference = apiPreference;
    camOpenCondition.store(true);

    thread_pool.EnqueueJob(openCameras, std::ref(camOpenCondition), std::ref(mVidCaps), cameraIds, mApiPreference);

    while(!isAnyOpened()) {
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "any cam is opened!" << std::endl;
}


void MultiVideoCapture::release() {
    // stop thread flag
    camOpenCondition.store(false);
    mApiPreference = -1;

    const int nbDevs = (int)mVidCaps.size();
    void (VideoCaptureType::*releasefunc)() = &VideoCaptureType::release;
	std::vector<std::future<void> > futures;

    for(int i = 0; i < nbDevs; i++) {
        if (mVidCaps[i].status() != CAM_STATUS_CLOSED) {
			futures.emplace_back(thread_pool.EnqueueJob(releasefunc, &mVidCaps[i]));
		}
	}

	// wait until the previous jobs are done.
	for (int i = 0; i < futures.size(); i++)
		futures[i].wait();
}


bool MultiVideoCapture::isOpened(int cameraNum) const {
    if (mVidCaps[cameraNum].isOpened() == CAM_STATUS_OPENED)
        return true;
    else
        return false;
}


bool MultiVideoCapture::isAnyOpened() const {
    for (int i = 0; i < (int)mVidCaps.size(); i++) {
        if (mVidCaps[i].status() == CAM_STATUS_OPENED) {
            return true;
        }
    }

    return false;
}


bool MultiVideoCapture::isAllOpened() const {
    for (int i = 0; i < (int)mVidCaps.size(); i++) {
        if (mVidCaps[i].status() != CAM_STATUS_OPENED) {
            return false;
        }
    }

    return true;
}


bool MultiVideoCapture::read(std::vector<FrameType>& images) {
	std::lock_guard<std::mutex> lock(gMtxCamRead);

    const int nbDevs = (int)mVidCaps.size();
    if (nbDevs != images.size())
        images.resize(nbDevs);

    std::vector<std::future<bool> > futures;
    bool (VideoCaptureType::*readfunc)(FrameType&) = &VideoCaptureType::read;

    for (int i = 0; i < nbDevs; i++) {
        if (mVidCaps[i].status() == CAM_STATUS_OPENED) {
            futures.emplace_back(thread_pool.EnqueueJob(readfunc, &mVidCaps[i], std::ref(images[i])));
        }
    }

    bool status = false;
    for (int i = 0; i < nbDevs; i++) {
        if (!images[i].empty()) {
            status = true;
            break;
        }
    }

    return status;
}


MultiVideoCapture& MultiVideoCapture::operator >> (std::vector<FrameType>& images) {
    read(images);

    return *this;
}


bool MultiVideoCapture::set(cv::Size resolution, float fps) {
    for (int i = 0; i < (int)mVidCaps.size(); i++) {
        mVidCaps[i].set(resolution, fps);
    }

    mResolution = resolution;
    mFps = fps;

    return true;
}


void MultiVideoCapture::resize(size_t size) {
    if (mVidCaps.size() != size) {
        release();
        mVidCaps.resize(size);
    }
}
