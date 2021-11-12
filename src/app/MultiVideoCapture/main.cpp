#include <iostream>
#include <thread>

#include "opencv2/opencv.hpp"
#include "MultiVideoCapture.hpp"


int main() {
	std::vector<int> camIds = { 0, 1 };
	std::vector<FrameType> images(camIds.size());
	cv::Size resolution = { 1280, 720 };
	//cv::Size resolution = { 640, 360 };
	float fps = 30.f;

	MultiVideoCapture mvc;
	mvc.open(camIds, CV_CAP_DSHOW);
	mvc.set(resolution, fps);

	std::chrono::system_clock::time_point wait_until;
	std::chrono::milliseconds duration(long(1000.f / fps));
	char c = ' ';
	while (c != 17) {	// 17 == ctrl + q
		wait_until = std::chrono::system_clock::now() + duration;

		mvc >> images;

		for (int i = 0; i < camIds.size(); i++) {
			if (!images[i].empty())
				cv::imshow("cam " + std::to_string(i), images[i].mat());
		}

		c = cv::waitKey(1);

		std::this_thread::sleep_until(wait_until);
	}

	mvc.release();

	return 1;
}