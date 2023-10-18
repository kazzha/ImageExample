#include  <fstream>
#include  <vector>
#include "ImageExample.h"

#pragma comment (lib, "WindowsCodecs.lib")

HRESULT ImageExample::LoadBMP(LPCWSTR filename, ID2D1Bitmap** ppBitmap)
{
    std::ifstream file;
    file.open(filename, std::ios::binary);

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    file.read(reinterpret_cast<char*>(&bfh), sizeof(BITMAPFILEHEADER));
    file.read(reinterpret_cast<char*>(&bih), sizeof(BITMAPINFOHEADER));

    if (bfh.bfType != 0x4D42) // 리틀 인디언 방식
    {
        return E_FAIL;
    }
    if (bih.biBitCount != 32)
    {
        return E_FAIL;
    }

    std::vector<char> pixels(bih.biSizeImage); //  w * h * bpp(사이즈)
    file.seekg( bfh.bfOffBits);

    int pitch = bih.biWidth * (bih.biBitCount / 8);
    // file.read(&pixels[0], bih.biSizeImage);
    /*
    for (int y = bih.biHeight - 1; y >= 0; y--) // 파일에서 순서대로 읽어서 거꾸로 저장하는것
    {
        file.read(&pixels[y * pitch], pitch);
    }
    */
    int index{};
    for (int y = bih.biHeight - 1; y >= 0; y--) 
    {
        index = y * pitch;
        for (int x = 0; x < bih.biWidth; x++)
        {
            char r{}, g{}, b{}, a{};
            file.read(&b, 1);
            file.read(&g, 1);
            file.read(&r, 1);
            file.read(&a, 1);

            if (r == 30 && g == -57/* 199*/ && b == -6 /*250*/)
            {
                r = g = b = a = 0;
            }

            pixels[index++] = b;
            pixels[index++] = g;
            pixels[index++] = r;
            pixels[index++] = a;
        }
    }
   
    file.close();

    HRESULT hr = mspRenderTarget->CreateBitmap(
        D2D1::SizeU(bih.biWidth, bih.biHeight),
        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        ppBitmap
        ); // 엔디안 문제 때문에 B G R A로 바꿔서 함- WINDOW 운영체제가 쓰는 포멧은 이게 표준
    ThrowIfFailed(hr);

    
    (*ppBitmap)->CopyFromMemory(nullptr, &pixels[0], pitch);

    return S_OK;
}

HRESULT ImageExample::LoadWIC(LPCWSTR filename, ID2D1Bitmap** ppBitmap)
{
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr;

    // 1. 디코더 만들기- 파일포멧을 해석
    hr = mspWICFactory->CreateDecoderFromFilename(
        filename,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        decoder.GetAddressOf()
        );
    ThrowIfFailed(hr);

    // 2. 해석한 다음 그림 1장을 가져옴
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    decoder->GetFrame(0, frame.GetAddressOf());
    ThrowIfFailed(hr);

    // 3. 그림을 B G R A 순서로 바꿈
    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    hr = mspWICFactory->CreateFormatConverter(converter.GetAddressOf()); 
    // 인터페이스 만들때 별 정보가 없음 - 직접 초기화 해야해서
    ThrowIfFailed(hr);

    hr = converter->Initialize(
         frame.Get(),
         GUID_WICPixelFormat32bppPBGRA,
         WICBitmapDitherTypeNone,
         nullptr,
         0,
         WICBitmapPaletteTypeCustom
        );

    ThrowIfFailed(hr);

    // 4. 컨버팅 된 정보를 가지고 그림파일을 만듦  
    // WIC 장점: 알아서 뒤집힌 것도 돌려줌
    hr = mspRenderTarget->CreateBitmapFromWicBitmap(
        converter.Get(),
        ppBitmap
    );

    ThrowIfFailed(hr);

    return S_OK;
}

HRESULT ImageExample::CreateDeviceResources()
{
    D2DFramework::CreateDeviceResources();
/*
HRESULT hr = LoadBMP(L"Images/32.bmp", mspBitmap.ReleaseAndGetAddressOf());
    ThrowIfFailed(hr);*/

    HRESULT hr = LoadWIC(L"Images/32.bmp", mspBitmap.ReleaseAndGetAddressOf());
    ThrowIfFailed(hr);

    return S_OK;
}

HRESULT ImageExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT w, UINT h)
{
    HRESULT hr = CoInitialize(nullptr);  // Com 방식을 사용하려면 이 초기화를 꼭 해야함
    ThrowIfFailed(hr);
    hr = ::CoCreateInstance(   // Com : 이걸로 인터페이스 만들어 냄
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(mspWICFactory.GetAddressOf())
    );
    ThrowIfFailed(hr);

    D2DFramework::Initialize(hInstance, title, w, h);
    return S_OK;
}

void ImageExample::Render()
{
    mspRenderTarget->BeginDraw();
    mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f, 1.0f));

    mspRenderTarget->DrawBitmap(mspBitmap.Get());


    HRESULT hr = mspRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        CreateDeviceResources();
    }
}

void ImageExample::Release()
{
    D2DFramework::Release();

    mspBitmap.Reset();
    mspWICFactory.Reset();

    CoUninitialize(); // Com 사용시 해제
}
