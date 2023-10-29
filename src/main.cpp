#include "global.hpp"
#include "platform.hpp"
#include "application.hpp"
#include <iostream>

#ifdef EMU_PLATFORM_WINDOWS
#include <Windows.h>
#endif // Windows

int emu_main(int argc, char* argv[])
{
    Application app;
    return app.run(argc, argv);
}

#ifdef EMU_PLATFORM_WINDOWS
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    EMU_UNUSED(hInstance);
    EMU_UNUSED(hPrevInstance);
    EMU_UNUSED(lpCmdLine);
    EMU_UNUSED(nShowCmd);

    return emu_main(__argc, __argv);
}
#else // Linux, macOS
int main(int argc, char* argv[])
{
    return emu_main(argc, argv);
}
#endif // Windows
