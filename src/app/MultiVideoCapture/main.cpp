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
	std::chrono::system_clock::time_point capture_times[2];
	std::chrono::system_clock::time_point cam_times[2];
	char c = ' ';
	while (c != 17) {	// 17 == ctrl + q
		wait_until = std::chrono::system_clock::now() + duration;

		capture_times[0] = std::chrono::system_clock::now();
		mvc >> images;
		capture_times[1] = std::chrono::system_clock::now();

		for (int i = 0; i < camIds.size(); i++) {
			cam_times[i] = images[i].timestamp();
			if (!images[i].empty())
				cv::imshow("cam " + std::to_string(i), images[i].mat());
		}

		std::chrono::duration<double> capture_sec = capture_times[1] - capture_times[0];
		std::chrono::duration<double> diff_sec = std::chrono::abs(cam_times[0] - cam_times[1]);
		std::cout << "capture time: " << std::setw(10) << std::left << capture_sec.count() << " sec";
		std::cout << "\ttime difference: " << std::setw(10) << std::left << diff_sec.count() << " sec" << std::endl;

		c = cv::waitKey(1);

		std::this_thread::sleep_until(wait_until);
	}

	mvc.release();

	return 1;
}