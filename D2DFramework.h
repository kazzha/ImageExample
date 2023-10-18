#pragma once
#include <d2d1.h>
#include <wrl/client.h>
#include <exception>
#include <stdio.h>


// COM Exception
class com_exception : public std::exception {
private:
	HRESULT result;

public:
	com_exception(HRESULT hr) : result(hr) {}
	virtual const char* what() const override
	{
		static char str[64]{}; // static : 프로그램 끝날 때까지 한번만 만들고 다시 만들지 않음
		sprintf_s(str, "Failed with HRESUL : %08X\n", result); // %X: 16진수, 08: 8글자 X: 대문자 x: 소문자
		return str;
	}
};

inline void ThrowIfFailed(HRESULT hr) // inline : 이 함수가 2줄밖에 안되니 굳이 호출스택을 사용하지않게
{
	if (FAILED(hr))
	{
		throw com_exception(hr);
    }
}

class D2DFramework
{
private:
	const LPCWSTR gClassName{ L"MyWindowClass" };

protected:
	HWND mHwnd{};

protected:
	Microsoft::WRL::ComPtr<ID2D1Factory> mspD2DFactory{};
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> mspRenderTarget{};

protected:
	HRESULT InitWindow(HINSTANCE hInstance, LPCWSTR title= L"D2DFramework", UINT w=1024, UINT h=768);
	virtual HRESULT InitD2D(HWND hwnd);
	virtual HRESULT CreateDeviceResources();

public:
	virtual HRESULT Initialize(HINSTANCE hInstance, LPCWSTR title = L"D2DFramework", UINT w = 1024, UINT h = 768);
	virtual void Release();
	virtual int GameLoop();
	virtual void Render();

	void ShowError(LPCWSTR msg, LPCWSTR title = L"error");

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};



