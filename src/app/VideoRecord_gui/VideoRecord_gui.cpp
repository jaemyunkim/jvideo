#include "VideoRecord_gui.h"

#include <chrono>
#include <qmessagebox>
#include <QFileDialog>

#include "VideoResolution.h"


// Default constructor.
VideoRecord_gui::VideoRecord_gui(QWidget *parent)
    : QWidget(parent)
{
	//ui.setupUi(this);
	this->setupUi(this);

	// set condition variables.
	mIsWork = false;
	mGrabWork = false;
	mBtnSource = false;
	mBtnRecord = false;

	// put the connected video devices on combobox of Source. /////////////////////////////////
	comboBox_Source->addItem(QString());
#ifdef WIN32
	DeviceEnumerator de;
	mDevices = de.getVideoDevicesMap();

	// Print information about the devices.
	for (auto const &device : mDevices)
	{
		comboBox_Source->addItem(QString::fromStdString(device.second.deviceName));

		//std::cout << "== VIDEO DEVICE (id:" << device.first << ") ==" << std::endl;
		//std::cout << "Name: " << device.second.deviceName << std::endl;
		//std::cout << "Path: " << device.second.devicePath << std::endl;
	}
#endif // !WIN32

	// background color initialization on display. /////////////////////////////////
	int bg_color = 2;
	switch (bg_color)
	{
	case 1:	// method 1.
		label_Display->setStyleSheet("QLabel { background-color : white; color : white; border : 1px solid black; }");
		break;
	case 2:	// method 2.
		QPalette sample_palette;
		sample_palette.setColor(QPalette::Window, Qt::white);
		sample_palette.setColor(QPalette::WindowText, Qt::black);

		label_Display->setAutoFillBackground(true);
		label_Display->setPalette(sample_palette);
		//label_Display->setText("What ever text");
		label_Display->setText("");
		break;
	}

	qDebug() << "  ** display size: " << label_Display->size();

	// qt connections /////////////////////////////////
	connect(this, SIGNAL(signalStartSource()), this, SLOT(startSource()));
	connect(this, SIGNAL(signalStopSource()), this, SLOT(stopSource()));
	connect(this, SIGNAL(signalStartRecord()), this, SLOT(startRecord()));
	connect(this, SIGNAL(signalStopRecord()), this, SLOT(stopRecord()));
}


// Destructor.
VideoRecord_gui::~VideoRecord_gui()
{
	//stop();
	//mGrab.closeSource();

	//closeSource();
	//stopSource();

	mGrab.stop();
	mRec.stop();

	//mWatcher.waitForFinished();
	//mWatcher.cancel();
}


// display image.
void VideoRecord_gui::displayImage()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	// convert BGR in opencv to RGB for qt display.
	cv::Mat img;
#ifdef CV_VERSION_MAJOR >= 4
	cv::cvtColor(mFrame, img, cv::COLOR_BGR2RGB);
#else // !CV_VERSION_MAJOR >= 4
	cv::cvtColor(mFrame, img, CV_BGR2RGB);
#endif // !CV_VERSION_MAJOR >= 4

	// resize image to fitting display size.
	QSize size = label_Display->size();
	if(size.width() != img.cols || size.height() != img.rows)
		cv::resize(img, img, cv::Size(size.width(), size.height()));
	//cv::resize(img, img, cv::Size(), 0.75, 0.75);

	// display frame.
	label_Display->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, (int)img.step, QImage::Format_RGB888)));

	////assert(!fs::exists(mSrcInfo));
	//if(!fs::exists(mSrcName))
	//{
	//	QString msg = QString("Can not find image file: \"") + QString::fromStdWString(mSrcName.wstring()) + QString("\"");
	//	QMessageBox::warning(this, QString("VideoRecord_gui"), msg);
	//	return;
	//}

	//mFrame = cv::imread(mSrcName.string(), cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
	//if (mFrame.empty())
	//{
	//	QString msg = QString("Can not read image file: \"") + QString::fromStdWString(mSrcName.wstring()) + QString("\"");
	//	QMessageBox::warning(this, QString("VideoRecord_gui"), msg);
	//	return;
	//}
	//cv::imshow("test image", mFrame);

	//// convert BGR in opencv to RGB for qt display.
	//cv::Mat img;
	//cv::cvtColor(mFrame, img, CV_BGR2RGB);

	//mpScene->addPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
	//graphicsView->setScene(mpScene);
	//graphicsView->show();
}


// Refresh fps.
void VideoRecord_gui::refreshFps()
{
	// get fps.
	mFps = mGrab.getFps();

	// display fps.
	QString fps = QString::number(mFps, 'f', 2) + " fps";
	
	// refresh fps display.
	label_Fps->setText(fps);
}


// Refresh resolution for record.
void VideoRecord_gui::refreshRecordResolution()
{
	try
	{
		// clear record resolution list.
		comboBox_RecordResolution->clear();

		// set record resolution list based on the resultion of source.
		cv::Size resolution = mGrab.getResolution();
		cv::Size res = VideoResolution::calcRatio(resolution);
		std::vector<cv::Size> candResolution;

		// search current retio.
		if (res == cv::Size(4, 3))
			candResolution = VideoResolution::resultion43;
		else if (res == cv::Size(16, 9))
			candResolution = VideoResolution::resultion169;
		else
		{
			std::string msg("Unsupported resolution.");
			throw std::runtime_error(msg);
		}

		for (auto const &resol : candResolution)
		{
			if (resol.width > resolution.width)
				break;

			QString item = QString::number(resol.width) + " x " + QString::number(resol.height);
			comboBox_RecordResolution->addItem(item);
		}

		comboBox_RecordResolution->setCurrentIndex(comboBox_RecordResolution->count() - 1);
	}
	catch (std::exception &e)
	{
		// clear record resolution list.
		comboBox_RecordResolution->clear();

		std::cerr << "!!! VideoRecord_gui: " << e.what() << std::endl;
		std::string msg("Cannot refresh resolution for record.");
		throw std::runtime_error(msg);
	}
}


// Start frame grab.
void VideoRecord_gui::start()
{
	if(!mIsWork)
	{
		mIsWork = true;
	}
}


// Stop frame grab.
void VideoRecord_gui::stop()
{
	if(mIsWork)
	{
		mIsWork = false;
	}
}



void VideoRecord_gui::handleComobBoxChangedSource()
{
	if(comboBox_Source->currentIndex() == 0)
	{
		QString text = comboBox_Source->currentText();
		comboBox_Source->setItemText(0, text);
	}
}



void VideoRecord_gui::handleComboBoxChangedDisplayResolution(int index)
{
	QString resolution = comboBox_DisplayResolution->currentText();
	QRegExp rx("[ x ]");	// match a comma or a space
	QStringList list = resolution.split(rx, QString::SkipEmptyParts);
	QSize newDisplaySize(list[0].toInt(), list[1].toInt());
	QSize displaySize = label_Display->size();

	label_Display->resize(newDisplaySize);

	QSize widgetSize = this->size();
	QSize newWidgetSize = widgetSize - displaySize + newDisplaySize;

	this->resize(newWidgetSize);
	//this->update();
	//this->adjustSize();
}


void VideoRecord_gui::handleComboBoxChangedRecordResolution(int index)
{
	QString resolution = comboBox_RecordResolution->currentText();
	QRegExp rx("[ x ]");	// match a comma or a space
	QStringList list = resolution.split(rx, QString::SkipEmptyParts);
	mRecordResolution = cv::Size(list[0].toInt(), list[1].toInt());
}


void VideoRecord_gui::handleButtonSource()
{
	if(!mBtnSource)
	{
		pushButton_Source->setText("close");
		mBtnSource = true;
		emit signalStartSource();
	}
	else
	{
		pushButton_Source->setText("open");
		mBtnSource = false;
		emit signalStopRecord();
		emit signalStopSource();
	}
}


void VideoRecord_gui::handleButtonRecord()
{
	if (!mBtnRecord)
	{
		if (mBtnSource)
		{
			pushButton_Record->setText("stop");
			comboBox_RecordResolution->setEnabled(false);
			mBtnRecord = true;
			emit signalStartRecord();
		}
	}
	else
	{
		pushButton_Record->setText("record");
		comboBox_RecordResolution->setEnabled(true);
		mBtnRecord = false;
		emit signalStopRecord();
	}
}


void VideoRecord_gui::handleButtonFileDialog()
{
	const QString DEFAULT_DIR_KEY("default_dir");
	const QString caption = "Save Video";
	const QString filter = "All files (*.*) ;; Media files (*.avi *.mp4)";
	QFileInfo fi;
	QString dir;
	QString filename = comboBox_FileName->currentText();

	QSettings mySettings; // Will be using application informations
						  // for correct location of your settings

	if (filename.isEmpty())
		dir = mySettings.value(DEFAULT_DIR_KEY).toString();
	else
		dir = QFileInfo(filename).absolutePath();

	filename = QFileDialog::getSaveFileName(this, "Save Video", dir, filter);

	if (filename.isNull())
	{
		qDebug() << "  ** Can not use the file name: " << filename.toUtf8();
		return;
	}

	QDir CurrentDir;
	mySettings.setValue(DEFAULT_DIR_KEY,
		CurrentDir.absoluteFilePath(filename));

	qDebug() << "  ** Selected file name: " << filename.toUtf8();

	comboBox_FileName->setCurrentIndex(0);
	comboBox_FileName->setCurrentText(filename);
}


void VideoRecord_gui::startSource()
{
	try
	{
		QString srcInfo;
		int devId;

		qDebug() << "  ** Grabber: Starting";

		if (comboBox_Source->currentIndex() == 0)
		{
			// check current is whether a number or a string.
			// if it is a number then it is considered as a device id.
			// otherwise it is considered as url or path.
			srcInfo = comboBox_Source->currentText();
			bool isNumber;
			int id = srcInfo.toInt(&isNumber, 10);
			if (isNumber == true)
			{
				qDebug() << "  ** Source: do id based connection.";
				mGrab.setSource(id);
			}
			else
			{
				qDebug() << "  ** Source: do string based connection.";
				mSrcName = srcInfo.toStdWString();
				if (mSrcName.empty())
				{
					std::string msg("Empty source.");
					throw std::runtime_error(msg);
				}
				mGrab.setSource(mSrcName);
			}
		}
		else
		{
			devId = comboBox_Source->currentIndex() - 1;
			mGrab.setSource(devId);
		}

		////assert(!fs::exists(mSrcInfo));
		//if(!fs::exists(mSrcName))
		//{
		//	QString msg = QString("Can not find image file: \"") + QString::fromStdWString(mSrcName.wstring()) + QString("\"");
		//	QMessageBox::warning(this, QString("VideoRecord_gui"), msg);
		//	return;
		//}

		mGrab.start(mFrame);
		start();

		qDebug() << "  ** Grabber: Started";

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));

		mFuture = QtConcurrent::run(this, &VideoRecord_gui::worker);
		//connect(&mWatcher, SIGNAL(finished()), this, SLOT(worker()));
		//mWatcher.setFuture(mFuture);

		qDebug() << "  ** Worker: Connected";
	}
	catch (std::exception &e)
	{
		handleButtonSource();
		QMessageBox::warning(this, QString("VideoRecord_gui"), e.what());
	}
}


void VideoRecord_gui::stopSource()
{
	mGrab.stop();
	stop();

	qDebug() << "  ** Grabber: Stopping";

	while (mGrabWork)
	{
		//qDebug() << "w";
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	qDebug() << "  ** Grabber: Stopped";
}


void VideoRecord_gui::startRecord()
{
	QString fName = comboBox_FileName->currentText();
	mFName = fName.toStdWString();

	qDebug() << "  ** Recorder: Starting";

	mRec.setFilename(mFName);

	while (!mGrab.isReady())
	{
		// wait for ready to record.
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	mRec.setRecordInfo(mFps, mRecordResolution);
	mRec.start(mFrame);

	qDebug() << "  ** Recorder: Started";
}


void VideoRecord_gui::stopRecord()
{
	qDebug() << "  ** Recorder: Stopping";

	if (mBtnRecord)
		handleButtonRecord();

	mRec.stop();

	qDebug() << "  ** Recorder: Stopped";
}


void VideoRecord_gui::worker()
{
	try
	{
		qDebug() << "  ** Worker: Started";

		mGrabWork = true;

		while (!mGrab.isReady())
		{
			// wait for the grabber is ready.
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// list the possible video resolution for recording. /////////////////////////
		refreshRecordResolution();

		// reset frame count. /////////////////////////
		mFrameCount = 0;
		long lastFrameCount = mGrab.getFrameCount();

		while (mIsWork)
		{
			if (mFrame.empty())
			{
				std::string msg = "Can not read image file: \"" + mSrcName.string() + "\"";
				throw std::runtime_error(msg);
				//QString msg = QString("Can not read image file: \"") + QString::fromStdWString(mSrcName.wstring()) + QString("\"");
				//throw msg;
			}

			// display image and refresh fps when a new frame comes.
			if (lastFrameCount != mGrab.getFrameCount())
			{
				// refresh the last frame count.
				lastFrameCount = mGrab.getFrameCount();

				displayImage();
				refreshFps();

				// increase frame counter.
				mFrameCount++;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		mGrabWork = false;
		qDebug() << "  ** Worker: Finished";
	}
	catch(std::exception &e)
	{
		QMessageBox::warning(this, QString("VideoRecord_gui"), e.what());
		emit signalStopSource();
		emit signalStopRecord();
		return;
	}
}
