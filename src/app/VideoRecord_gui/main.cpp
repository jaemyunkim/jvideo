#include <QApplication>
#include "VideoRecord_gui.h"


int main(int argc, char* argv[])
{
#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
	//_CrtSetBreakAlloc(477062); // 메모리 생성시 브레이크 걸리도록 추가
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	QApplication app(argc, argv);

	QWidget *widget = new QWidget;
	VideoRecord_gui ui(widget);

	widget->show();
	return app.exec();
}