// Include standard headers
#include "Main.h"

#include <Simplex\Simplex.h>

#include "Application.h"

int main(void)
{
	Application* pApp = new Application();
	pApp->Init("", Simplex::RES_C_1280x720_16x9_HD, false, false);
	pApp->Run();
	SafeDelete(pApp);
	return 0;
}