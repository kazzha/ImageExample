#pragma once

#include <wincodec.h>
#include "D2DFramework.h"

class ImageExample :
    public D2DFramework
{
    Microsoft::WRL::ComPtr<IWICImagingFactory> mspWICFactory;
    Microsoft::WRL::ComPtr<ID2D1Bitmap> mspBitmap;

private:
    HRESULT LoadBMP(LPCWSTR filename, ID2D1Bitmap** ppBitmap);
    HRESULT LoadWIC(LPCWSTR filename, ID2D1Bitmap** ppBitmap);

protected:
    virtual HRESULT CreateDeviceResources() override;

public:
    virtual HRESULT Initialize(HINSTANCE hInstance, LPCWSTR title = L"ImageExample",
        UINT w = 1024, UINT h = 768);
    void Render() override;
    void Release() override;
};

