#pragma once

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <dwrite_2.h>
#include "CoreTypes.h"


class DirectX11TextRenderer
{
private:
	IDWriteFactory2 * m_pDWriteFactory;
	ID2D1Factory2  * m_pD2Factory;
	ID2D1Device1 * m_pDevice;
	ID2D1DeviceContext1 * m_pDeviceContext;
	ID2D1Bitmap1 * m_pTargetBitmap; /*This is the target render surface. */
	/*NOTE(Dino): Inject these via constructor.*/
	IDXGIDevice * m_pDxgiDevice;
	IDXGISurface * m_pDxgiBuffer;
	
	IDWriteTextLayout * m_pTextLayout;

	HRESULT CreateBitmapRenderTarget();

	/*Basic colour brushes, pre-created for convenience. */
	ID2D1SolidColorBrush * m_pBlackBrush;
	ID2D1SolidColorBrush * m_pGreyBrush;

	ID2D1SolidColorBrush * m_pRedBrush;
	ID2D1SolidColorBrush * m_pGreenBrush;
	ID2D1SolidColorBrush * m_pBlueBrush;

	ID2D1SolidColorBrush * m_pYellowBrush;

	IDWriteTextFormat * m_pTextFormatFPS;


public:
	void SetText(const wchar_t * pText);
	void Init();
	void InitializeTextFormats();
	void Render();

	DirectX11TextRenderer(IDXGIDevice * pDxgiDevice, IDXGISurface * pDxgiSurface);
	~DirectX11TextRenderer();
};