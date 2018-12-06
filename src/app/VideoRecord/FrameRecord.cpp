#include "FrameRecord.h"


// Default constructor.
FrameRecord::FrameRecord() : mThread()
{
	//mThread = std::thread(FrameGrab::run, 1000);
	mIsRecord = false;
	mIsReady = false;
	mFrameCount = 0;
	mFps = 30.f;
	mRecordResolution = cv::Size(320, 240);
}


// Destructor.
FrameRecord::~FrameRecord()
{
	stop();
}


// Open writer.
void FrameRecord::openFile()
{
	try
	{
		if (mFName.empty())
		{
			std::string msg("Output file name is not set.");
			throw std::runtime_error(msg);
		}

		// create directory if it doesn't exist.
		std::string path = mFName.parent_path().string();
		fs::create_directories(mFName.parent_path());

		// Set Encoding fourcc, fps, resoltuion.
		int fourcc = CV_FOURCC('X', 'V', 'I', 'D');

		// Setup output video
		mOutputCap.open(mFName.string(),// argv[2],
			fourcc, mFps, mRecordResolution);

		if (!mOutputCap.isOpened())
		{
			std::string msg("Output video could not be opened");
			throw std::runtime_error(msg);
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "!!! FrameRecord: " << e.what() << std::endl;
		std::string msg("Cannot open the file.");
		throw std::runtime_error(msg);
	}
}


// Write file.
void FrameRecord::writeFile()
{
	if (mFrame.size() != mRecordResolution)
	{
		cv::Mat img;
		cv::resize(mFrame, img, mRecordResolution);

		mOutputCap.write(img);
	}
	else
		mOutputCap.write(mFrame);

}


// Close file.
void FrameRecord::closeFile()
{
	if (mOutputCap.isOpened())
		mOutputCap.release();
}


// Start frame record.
void FrameRecord::start(const cv::Mat &frame)
{
	try
	{
		// open the frame record.
		openFile();

		// set the status.
		mIsRecord = true;

		// This will start the thread. Notice move semantics!
		mThread = std::thread(&FrameRecord::run, this, std::ref(frame));
	}
	catch (std::exception &e)
	{
		stop();
		std::cerr << "!!! FrameRecord: " << e.what() << std::endl;
		std::string msg("Stop.");
		//throw ex;
	}
}


// Stop frame record.
void FrameRecord::stop()
{
	// set the status.
	mIsRecord = false;

	// finish the thread for the frame record.
	if (mThread.joinable())
		mThread.join();

	// close the frame record.
	closeFile();
}

// Run video record.
void FrameRecord::run(const cv::Mat &frame)
{
	//assert(!mSrcName.empty());

	// keep taking the frames.
	try
	{
		if (mFName.empty())
		{
			std::string msg("Output file name is not set.");
			throw std::runtime_error(msg);
		}

		// reset frame count.
		mFrameCount = 0;

		// check times for the synchronizing the frame rate per seconds.
		float procTerm = 1.0f / (float)mFps;
		std::chrono::duration<float> totalProcTime, waitTime, procTime = std::chrono::duration<float>(procTerm);
		std::chrono::system_clock::time_point startTimestamp, endTimestamp, currentTimestamp, nextTimestamp;
		nextTimestamp = std::chrono::system_clock::now();
		currentTimestamp = nextTimestamp;
		waitTime = currentTimestamp - nextTimestamp;
		float initRecordTime = 0.f;
		
		int delayFound = 0;
		int totalDelay = 0;
		int totalframe = 0;
		
		// check total processing time.
		totalProcTime = std::chrono::duration<float>(0);
		
		// Loop to read from input.
		while (mIsRecord == true)
		{
			// wait for X microseconds until 1second/framerate time has passed after previous frame write
			//while (waitTime.count() < procTerm - procTime.count())
			//{
			//	//determine current elapsed time
			//	currentTimestamp = std::chrono::system_clock::now();
			//	waitTime = currentTimestamp - nextTimestamp;
			//}
			waitTime = std::chrono::duration<float>(procTerm) - procTime;
			if(waitTime > std::chrono::duration<float>(0))
				std::this_thread::sleep_for(waitTime);
			else
				waitTime = std::chrono::duration<float>(0);

			// determine time at start.
			startTimestamp = std::chrono::system_clock::now();
			
			// get the current frame.
			mFrameMutex.lock();
			frame.copyTo(mFrame);
			mFrameMutex.unlock();

			if (mFrame.empty())
			{
				std::string msg("Cannot receive the frame to FrameRecord.");
				throw std::runtime_error(msg);
			}

			// Write frame to video file.
			writeFile();

			// Increase frame counter.
			mFrameCount++;

			// determine time at end.
			endTimestamp = std::chrono::system_clock::now();
			procTime = endTimestamp - startTimestamp;

			if (mFrameCount == 1)
				initRecordTime = procTime.count();

			// print frame counter and processing time.
			std::cout << "  ** FrameRecord:frame: " << mFrameCount << "\tprocessing time: " << procTime.count() << " sec" << std::endl;
		
			// calculate total processing time.
			totalProcTime += procTime + waitTime;
		}

		// set the working status.
		mIsReady = false;

		float recordDelay = (totalProcTime.count()) - (procTerm * (mFrameCount - 1));
		float averageRecordDelay = recordDelay / mFrameCount;
		std::cout << std::fixed;
		std::cout << "  ** Recording delay: " << recordDelay << " sec" << std::endl;
		std::cout << "  ** Average recording delay: " << averageRecordDelay << " sec" << std::endl;
	}
	catch(std::exception &e)
	{
		stop();
		std::cerr << "!!! FrameRecord: " << e.what() << std::endl;
		return;
	}
}