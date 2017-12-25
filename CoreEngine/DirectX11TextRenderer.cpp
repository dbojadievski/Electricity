#include "DirectX11TextRenderer.h"
#include <assert.h>

void
DirectX11TextRenderer::SetText(const wchar_t * pText)
{
	HRESULT errCode = S_OK;

	//if (this->m_pTextLayout)
		//this->m_pTextLayout->Release();
	size_t len = wcslen(pText);
	errCode = this->m_pDWriteFactory->CreateTextLayout(pText, len, this->m_pTextFormatFPS, 640, 480, &m_pTextLayout);
	
	assert(errCode == S_OK);
}

DirectX11TextRenderer::~DirectX11TextRenderer()
{
	/*NOTE(Dino): We can't release the dxgi device and buffer, as this class doesn't own them. They belong to the main renderer. */
	this->m_pD2Factory->Release();
	this->m_pDevice->Release();
	this->m_pDeviceContext->Release();

	this->m_pTargetBitmap->Release();
}

DirectX11TextRenderer::DirectX11TextRenderer(IDXGIDevice * pDxgiDevice, IDXGISurface * pDxgiSurface)
{
	assert(pDxgiDevice);
	assert(pDxgiSurface);

	this->m_pDxgiDevice = pDxgiDevice;
	IDXGIAdapter * pAdapter;
	auto errCode = this->m_pDxgiDevice->GetAdapter( &pAdapter);
	this->m_pDxgiBuffer = pDxgiSurface;
	DXGI_SURFACE_DESC Desc;
	errCode = this->m_pDxgiBuffer->GetDesc(&Desc);
}

void 
DirectX11TextRenderer::Init()
{
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **) &this->m_pDWriteFactory);
	D2D1_FACTORY_OPTIONS options;
#ifndef NDEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
	options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif

	HRESULT errCode = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &this->m_pD2Factory);
	assert(errCode == S_OK);

	errCode = this->m_pD2Factory->CreateDevice(this->m_pDxgiDevice, &this->m_pDevice);
	assert(errCode == S_OK);

	errCode = this->m_pDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &this->m_pDeviceContext);
	assert(errCode == S_OK);

	CreateBitmapRenderTarget();
	InitializeTextFormats();
}

HRESULT 
DirectX11TextRenderer::CreateBitmapRenderTarget()
{
	HRESULT errCode = S_OK;

	// specify the desired bitmap properties
	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	bp.dpiX = 96.0f;
	bp.dpiY = 96.0f;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = nullptr;

	errCode = this->m_pDeviceContext->CreateBitmapFromDxgiSurface(this->m_pDxgiBuffer, NULL, &this->m_pTargetBitmap);
	assert(errCode == S_OK);
	if (errCode == S_OK)
		this->m_pDeviceContext->SetTarget(this->m_pTargetBitmap);

	return errCode;
}


void
DirectX11TextRenderer::InitializeTextFormats()
{
	HRESULT errCode = S_OK;


	/*Initializing basic text colour brushes for convenience. */
	{
		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_pBlackBrush);
		assert(errCode == S_OK);
	
		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DimGray), &this->m_pGreyBrush);
		assert(errCode == S_OK);

	
		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &this->m_pRedBrush);
		assert(errCode == S_OK);

		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &this->m_pGreenBrush);
		assert(errCode == S_OK);
	
		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &this->m_pBlueBrush);
		assert(errCode == S_OK);

		errCode = this->m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &this->m_pYellowBrush);
		assert(errCode == S_OK);
	}

	errCode = this->m_pDWriteFactory->CreateTextFormat(L"Lucida Console", NULL, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-GB", &m_pTextFormatFPS);
	assert(errCode == S_OK);
	this->m_pTextFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	this->m_pTextFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	assert(errCode == S_OK);
}

void
DirectX11TextRenderer::Render()
{
	this->m_pDeviceContext->BeginDraw();
	this->m_pDeviceContext->DrawTextLayout(D2D1::Point2F(2.0f, 5.0f), this->m_pTextLayout, this->m_pYellowBrush);
	this->m_pDeviceContext->EndDraw();
} 