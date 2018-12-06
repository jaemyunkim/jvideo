#include "VideoRecord.h"

// OpenCVDeviceEnumerator
#ifdef WIN32
#include "DeviceEnumerator.h"
#include <map>
#endif // !WIN32


// Default constructor.
VideoRecord::VideoRecord()
{
	mFrameCount = 0;
	mFps = 0.f;
	mRecordResolution = cv::Size(0, 0);
}


// Destructor.
VideoRecord::~VideoRecord()
{
	mFg.stop();
	mFr.stop();
}


// Usage checking.
void VideoRecord::usage(int argc, char **argv)
{
	fs::path exec = argv[0];
	if (argc <= 1)
	{
		usageMessage(exec.filename().string(), std::cout);
		exit(0);
	}

	int c, id;
	char *endptr;

	while ((c = getopt(argc, argv, "h:i:o:r:")) != -1)
	{
		switch (c)
		{
		case 'h':
			// display usage.
			usageMessage(exec.filename().string(), std::cout);
			exit(0);
			break;
		case 'i':
			// set a source device.
			if (optarg[0] == '-')
			{
				exit(1);
			}
			id = strtol(optarg, &endptr, 10);
			if (strlen(endptr) > 0)
				setSource(optarg);
			else
				setSource(id);
			break;
		case 'o':
			// set a file name to record.
			if (optarg[0] == '-')
			{
				usageMessage(argv[0], std::cout);
				exit(1);
			}
			setFilename(optarg);
			break;
		case 'r':
			// set an output resolution.
			if (optarg[0] == '-')
			{
				usageMessage(exec.filename().string(), std::cout);
				exit(1);
			}
			setRecordResolution(optarg);
			break;
		case ':':
			// first parameter is ':'.
			exit(1);
			break;
		case '?':
			// otehr errors.
			std::cerr << "invalid option: " << c << std::endl;
			usageMessage(exec.filename().string(), std::cout);
			exit(1);
			break;
		default:
			break;
		}
	}
}


// Usage message.
void VideoRecord::usageMessage(std::string name, std::ostream &out)
{
	out << "Usage: " << name << " [OPTION]... -i SOURCE" << std::endl;
	out << "  or:  " << name << " [OPTION]... -i SOURCE -o DEST" << std::endl;
	out << "Get frames from SOURCE and display them, and record to DEST." << std::endl;
	out << std::endl << "Available options:" << std::endl;
	out << "  -i\tsource (device id, source path or url)" << std::endl;
	out << "    \t  your device:\tID\tvideo device" << std::endl;
	
#ifdef WIN32
	DeviceEnumerator de;
	std::map<int, Device> devices = de.getVideoDevicesMap();;

	for (auto const &device : devices)
	{
		out << "    \t\t\t" << device.first << "\t" << device.second.deviceName << std::endl;
	}
#endif // !WIN32
	
	out << "  -o\toutput video file name [destination path]/[file name]" << std::endl;
	out << "    \t  example:\t/home/user/video_test/test.mp4" << std::endl;
	out << "  -r\trecord resolution [width]x[height]" << std::endl;
	out << "    \t  example:\t320x240" << std::endl;
	out << "  -h\tdisplay this help and exit" << std::endl;
}


// Set a Record video resolution.
void VideoRecord::setRecordResolution(std::string resolution)
{
	typedef std::regex_iterator<std::string::const_iterator> rx_iterator;
	typedef rx_iterator::value_type rx_iterated;

	//std::regex re("(\\d+)");
	std::regex rx("([0-9]*)x([0-9]*)");
	std::regex_iterator<std::string::const_iterator> rit(resolution.begin(), resolution.end(), rx);
	std::vector<int> result;
	//rx_iterator rend;
	
	//std::transform(rit, rend, std::back_inserter(result)); ,
	//	[](const rx_iterated& it) { return std::stoi(it[1]); });

	std::smatch m;
	if (regex_match(resolution, m, rx))
	{
		for (auto &sm : m)
			result.push_back(std::stoi(sm));
	}

	cv::Size size(result[1], result[2]);
	mRecordResolution = size;
}


// Run video record.
void VideoRecord::run()
{
	try
	{
		cv::Mat frame;
		mFg.start(frame);

		while (!mFg.isReady())
		{
			// wait for ready to record.
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		mFps = mFg.getFps();

		if (mRecordResolution == cv::Size(0, 0))
			mRecordResolution = mFg.getResolution();

		mFr.setRecordInfo(mFps, mRecordResolution);
		mFr.start(frame);

		int frameCount = 0;
		long lastFrameCount = mFg.getFrameCount();

		// Loop to read from input and write to output
		while (true)
		{
			if (frame.empty())
				std::cout << "no input frame." << std::endl;

			// display image and refresh fps when a new frame comes.
			if (lastFrameCount != mFg.getFrameCount())
			{
				cv::imshow("input video", frame);

				// increase frame counter.
				frameCount++;

				// refresh the last frame counter.
				lastFrameCount = mFg.getFrameCount();
			}

			char ch = cv::waitKey(1);
			if (ch == 'q')
				break;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		mFr.stop();
		mFg.stop();
	}
	catch (std::exception &e)
	{
		mFr.stop();
		mFg.stop();

		std::cerr << "!!! VideoRecord: " << e.what() << std::endl;
		return;
	}
}