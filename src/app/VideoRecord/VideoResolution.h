#ifndef	VIDEO_RESOLUTION_H_
#define	VIDEO_RESOLUTION_H_


#include <vector>

// use opencv
#include "opencv2/opencv.hpp"


namespace VideoResolution
{
	std::vector<cv::Size> resultion43 = { {160, 120}, {320, 240}, {640, 480}, {800, 600}, {1024, 768}, {1152, 864}, {1400, 1050}, {1600, 1200}, {2048, 1536}, {3200, 2400} };
	std::vector<cv::Size> resultion169 = { {640, 360}, {960, 540}, {1280, 720}, {1600, 900}, {1920, 1080}, {2560, 1440}, {3840, 2160} };


	int calcGCD(int a, int b)
	{
		while (b != 0) {
			int r = a % b;
			a = b;
			b = r;
		}

		return a;
	}


	cv::Size calcRatio(int a, int b)
	{
		int gcd = calcGCD(a, b);
		cv::Size ratio = { a / gcd, b / gcd };
		return ratio;
	}


	cv::Size calcRatio(cv::Size resolution)
	{
		int gcd = calcGCD(resolution.width, resolution.height);
		cv::Size ratio = { resolution.width / gcd, resolution.height / gcd };
		return ratio;
	}
}



#endif // !VIDEO_RESOLUTION_H_
