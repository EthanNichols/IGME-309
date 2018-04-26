// Include standard headers
#include "Main.h"

int main(void)
{
	using namespace Simplex;
	Application* pApp = new Application();
	srand(time(NULL));
	pApp->Init("", RES_C_1280x720_16x9_HD, false, false);
	pApp->Run();
	SafeDelete(pApp);
	return 0;
}