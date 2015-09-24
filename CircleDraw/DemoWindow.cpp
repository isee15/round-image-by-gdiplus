#define _WIN32_WINNT 0x0501
#define WINVER 0x0501

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <gdiplus.h>
#define _UNICODE
#define UNICODE
#include <tchar.h>

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Ole32.lib")

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.Lib")
#pragma comment(lib, "gdi32.Lib")

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")


LPCTSTR clsName = L"isee15";
LPCTSTR wndName = L"Circle";

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ApplyAlphaMask(Bitmap &bmp, Bitmap &alphaMaskImage);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    /*  GDI+ startup token */
    ULONG_PTR gdiplusStartupToken;
    Gdiplus::GdiplusStartupInput gdiInput;
    Gdiplus::GdiplusStartup(&gdiplusStartupToken, &gdiInput, NULL);

    MSG Msg;
    HWND hWnd;
    WNDCLASSEX wndClsEx;

    // Create the application window
    wndClsEx.cbSize = sizeof(WNDCLASSEX);
    wndClsEx.style = CS_HREDRAW | CS_VREDRAW;
    wndClsEx.lpfnWndProc = WndProcedure;
    wndClsEx.cbClsExtra = 0;
    wndClsEx.cbWndExtra = 0;
    wndClsEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClsEx.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndClsEx.lpszMenuName = NULL;
    wndClsEx.lpszClassName = clsName;
    wndClsEx.hInstance = hInstance;
    wndClsEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Register the application
    RegisterClassEx(&wndClsEx);

    // Create the window object
    hWnd = CreateWindow(clsName,
                        wndName,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    // Find out if the window was created
    if (!hWnd) // If the window was not created,
        return 0; // stop the application

    // Display the window to the user
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    // Decode and treat the messages
    // as long as the application is running
    while (GetMessage(&Msg, NULL, 0, 0)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    Gdiplus::GdiplusShutdown(gdiplusStartupToken);
    return Msg.wParam;
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,
                              WPARAM wParam, LPARAM lParam) {
    // this is the main message handler of the system
    PAINTSTRUCT ps;          // used in WM_PAINT
    HDC hdc;         // handle to a device context
    switch (Msg) {
        // If the user wants to close the application
        case WM_DESTROY:
            // then close it
            PostQuitMessage(WM_QUIT);
            break;
        case WM_PAINT: {
            // simply validate the window
            hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);

            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;

            int saveDcId = SaveDC(hdc);

            HDC hMemDc = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
            SelectObject(hMemDc, hBitmap);

            Graphics g(hMemDc);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            SolidBrush brush(Color::White);
            g.FillRectangle(&brush, 0, 0, w, h);

            Image img(L"bk.jpg");
            //1
            {
                GraphicsPath path;
                path.AddEllipse(100.0, 100, 100, 100);
                g.SetClip(&path, CombineModeReplace);
                g.DrawImage(&img, 100, 100, 100, 100);
            }
            {
                GraphicsPath path;
                path.AddEllipse(50.0, 150, 100, 100);
                g.SetClip(&path, CombineModeReplace);
                g.DrawImage(&img, 50, 150, 100, 100);
            }
            {
                GraphicsPath path;
                path.AddEllipse(150.0, 150, 100, 100);
                g.SetClip(&path, CombineModeReplace);
                g.DrawImage(&img, 150, 150, 100, 100);
            }
            //2
            {
                RectF arc((REAL) 600, (REAL) 100, 100, 100);

                GraphicsPath path;
                path.StartFigure();
                path.AddArc(arc, 180, 90);
                path.AddArc(arc, 270, 90);
                path.AddArc(arc, 0, 90);
                path.AddArc(arc, 90, 90);
                path.CloseFigure();
                g.SetClip(&path, CombineModeReplace);

                g.DrawImage(&img, 600, 100, 100, 100);
            }
            //3
            {
                GraphicsPath path;
                path.AddRectangle(Rect(0, 0, w, h));
                g.SetClip(&path, CombineModeReplace);
                TextureBrush brush(&img);
                Matrix matrix(100.0f / 121, 0, 0, 100.0f / 121, 0, 0);
                brush.SetTransform(&matrix);
                //brush.SetWrapMode(WrapModeClamp);
                g.FillEllipse(&brush, 350, 100, 100, 100);
                g.FillEllipse(&brush, 300, 150, 100, 100);
                g.FillEllipse(&brush, 400, 150, 100, 100);
            }
            //4
            {
                Bitmap memImg(img.GetWidth(), img.GetHeight(), PixelFormat32bppARGB);
                Graphics *mg = Graphics::FromImage(&memImg);
                mg->SetSmoothingMode(SmoothingModeAntiAlias);
                TextureBrush brush(&img);
                mg->FillEllipse(&brush, 0, 0, img.GetWidth(), img.GetHeight());
                delete mg;
				CLSID clsid;
				GetEncoderClsid(L"image/png", &clsid);
				memImg.Save(L"process1.png", &clsid);

                g.DrawImage(&memImg, 100, 300, 100, 100);
                g.DrawImage(&memImg, 50, 350, 100, 100);
                g.DrawImage(&memImg, 150, 350, 100, 100);
            }

            //5
            {
                Bitmap memImg(100, 100, PixelFormat32bppARGB);
                Graphics *mg = Graphics::FromImage(&memImg);
                mg->DrawImage(&img, 0, 0, 100, 100);
                delete mg;
                Bitmap mask(L"mask.bmp");
                ApplyAlphaMask(memImg, mask);
                g.DrawImage(&memImg, 350, 300, 100, 100);
                g.DrawImage(&memImg, 300, 350, 100, 100);
                g.DrawImage(&memImg, 400, 350, 100, 100);
				CLSID clsid;
				GetEncoderClsid(L"image/png", &clsid);
				memImg.Save(L"process2.png", &clsid);
            }


            BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hMemDc, 0, 0, SRCCOPY);
            RestoreDC(hdc, saveDcId);
            DeleteObject(hBitmap);
            DeleteObject(hMemDc);

            EndPaint(hWnd, &ps);


        }
            break;
        case WM_ERASEBKGND:
            break;
        default:
            // Process the left-over messages
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    // If something was not done, let it go
    return 0;
}
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void ApplyAlphaMask(Bitmap &bmp, Bitmap &alphaMaskImage) {
    int width = bmp.GetWidth();
    int height = bmp.GetHeight();

    BitmapData dataAlphaMask;
    alphaMaskImage.LockBits(&(Rect(0, 0, width, height)), ImageLockModeRead, PixelFormat32bppARGB, &dataAlphaMask);
    __try
    {
        BitmapData data;
        bmp.LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &data);
        __try
        {

            byte *pData0Mask = (byte *) dataAlphaMask.Scan0;
            byte *pData0 = (byte *) data.Scan0;

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    byte *pData = pData0 + (y * data.Stride) + (x * 4);
                    byte *pDataMask = pData0Mask + (y * dataAlphaMask.Stride) + (x * 4);

                    byte maskBlue = pDataMask[0];
                    byte maskGreen = pDataMask[1];
                    byte maskRed = pDataMask[2];
                    byte maskAlpha = pDataMask[3];

                    //the closer the color is to black the more opaque it will be.
                    byte alpha = (byte) (255 - (maskRed + maskBlue + maskGreen)*(maskAlpha) / (3*255.0f));

                    //respect the original alpha value
                    byte originalAlpha = pData[3];
                    pData[3] = (byte) (((float) (alpha * originalAlpha)) / 255.0f);

                }
            }

        }
        __finally
        {
            bmp.UnlockBits(&data);
        }
    }
    __finally
    {
        alphaMaskImage.UnlockBits(&dataAlphaMask);
    }
}