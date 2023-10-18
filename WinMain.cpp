#include <windows.h>
#include "ImageExample.h"


int WINAPI WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    // D2D Factory / single - 조금 느리지만 데드락이 없어 안전
    // &gpD2DFactory 반환할 값. 포인터 포인터 형식

    try{
    ImageExample myFramework;

    
    myFramework.Initialize(hInstance, L"Framework", 1024, 768);

    int ret= myFramework.GameLoop();

    myFramework.Release();

   
    /*MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        OnPaint(hwnd);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    */

    return ret;
    }
    catch(const com_exception& e)
    {
    OutputDebugStringA(e.what());
    MessageBoxA(nullptr, e.what(), "Error", MB_OK);
    }

    return 0;

}
