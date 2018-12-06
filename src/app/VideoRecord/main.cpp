#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif


// use opencv
#include "opencv2/opencv.hpp"

#include "VideoRecord.h"
#include "FrameGrab.h"


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


int main(int argc, char* argv[])
{
#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
	//_CrtSetBreakAlloc(477062); // 메모리 생성시 브레이크 걸리도록 추가
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	VideoRecord rec;
	rec.usage(argc, argv);
	rec.run();

	return 0;
}
