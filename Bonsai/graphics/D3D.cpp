#include "D3D.h"
namespace bonsai
{
	namespace graphics {
		Direct3D::Direct3D()
		{
			m_SwapChain = nullptr;
			m_Device = nullptr;
			m_DeviceContext = nullptr;
			m_RenderTargetView = nullptr;
			m_DepthStencilBuffer = nullptr;
			m_DepthStencilState = nullptr;
			m_DepthDisabledStencilState = nullptr;
			m_DepthStencilView = nullptr;
			m_RasterState = nullptr;
		}

		Direct3D::Direct3D(const Direct3D&)
		{
		}

		Direct3D::~Direct3D()
		{
		}

		bool Direct3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth,
			float screenNear)
		{
			HRESULT result;
			IDXGIFactory* factory;
			IDXGIAdapter* adapter;
			IDXGIOutput* adapterOutput;
			UINT numModes, i, numerator, denominator;
			ULONGLONG stringLength;
			DXGI_MODE_DESC* displayModeList;
			DXGI_ADAPTER_DESC adapterDesc;
			INT error;
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			D3D_FEATURE_LEVEL featureLevel;
			ID3D11Texture2D* backBufferPtr;
			D3D11_TEXTURE2D_DESC depthBufferDesc;
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			D3D11_RASTERIZER_DESC rasterDesc;
			D3D11_VIEWPORT viewport;
			float fieldOfView, screenAspect;

			m_vsync_enabled = vsync;


			//Create a DirectX graphics interface factory
			result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
			if (FAILED(result))	return false;

			// Use the factory to create an adapter for the primary graphics interface (video card).
			result = factory->EnumAdapters(0, &adapter);
			if (FAILED(result)) return false;

			// Enumerate the primary adapter output (monitor).
			result = adapter->EnumOutputs(0, &adapterOutput);
			if (FAILED(result))	return false;

			// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
			result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
			if (FAILED(result)) return false;

			// Create a list to hold all the possible display modes for this monitor/video card combination.
			displayModeList = new DXGI_MODE_DESC[numModes];
			if (!displayModeList) return false;

			// Now fill the display mode list structures.
			result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
			if (FAILED(result)) return false;

			// Now go through all the display modes and find the one that matches the screen width and height.
			// When a match is found store the numerator and denominator of the refresh rate for that monitor.
			for(i=0; i<numModes; i++)
			{
				if ((displayModeList[i].Width == (UINT)screenWidth) &&  displayModeList[i].Height == (UINT)screenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}

			//Get the video card description
			result = adapter->GetDesc(&adapterDesc);
			if (FAILED(result)) return false;

			//store in MB
			m_VideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

			//Convert the name of the video card to a character array and store it.
			error = wcstombs_s(&stringLength, m_VideoCardDescription, 128, adapterDesc.Description, 128);
			if (error != 0) return false;

			//Release ptrs
			delete[] displayModeList;
			displayModeList = nullptr;

			adapterOutput->Release();
			adapterOutput = nullptr;

			adapter->Release();
			adapter = nullptr;

			factory->Release();
			factory = nullptr;

			//Init swap chain (back and front buffers)
			//Self note ZeroMemory == memset(dest,0,size)
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

			//Set to a single back buffer
			swapChainDesc.BufferCount = 1;

			//Set the widths and height of the back buffer
			swapChainDesc.BufferDesc.Width = screenWidth;
			swapChainDesc.BufferDesc.Height = screenHeight;

			//Set regular 32-bit surface for the back buffer
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


			//refresh rate
			if(m_vsync_enabled)
			{
				swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
			} else
			{
				swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			}

			//Set usage of back buffer
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

			//Set the handle for the window to render to
			swapChainDesc.OutputWindow = hwnd;

			//Turn multisampling off
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;

			//Set to fullscreen or windowed mode
			if (fullscreen) swapChainDesc.Windowed = false;
			else swapChainDesc.Windowed = true;

			//Set the scan line ordering and scaling to unspecified
			swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			//Discard the back buffer after presenting
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			//Dont set the advanced flags
			swapChainDesc.Flags = 0;

			//Set feature level (can swap to lower versions to support lower end hardware)
			featureLevel = D3D_FEATURE_LEVEL_11_0;

			//Create swap chain, D3D device and D3d device context - if no dx11 support swap driver type hardward to reference for cpu rendering
			result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext);
			if (FAILED(result)) return false;

			//Get the pointer to the backbuffer
			result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
			if (FAILED(result)) return false;

			//Create render target view with the back buffer pointer
			result = m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView);
			if (FAILED(result)) return false;

			//Release pointer to the back buffer as we no longer need it
			backBufferPtr->Release();
			backBufferPtr = nullptr;

			//Initalize the description and the depth buffer
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

			//Setup the description of the depth buffer (with attached stencil for blur, vol shadows, etc)
			depthBufferDesc.Width = screenWidth;
			depthBufferDesc.Height = screenHeight;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			//Create the texture for the depth buffer using the filled out desc
			result = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
			if (FAILED(result)) return false;

			//setup the depth stensil desc
			//init the description
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

			depthStencilDesc.StencilEnable = true;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			//if the pixel is front facing
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			//if the pixel is back-facing
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			result = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
			if (FAILED(result)) return false;

			//Set the depth stencil state
			m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

			//FOR 2D
			ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
			
			depthDisabledStencilDesc.DepthEnable = false;
			depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

			depthDisabledStencilDesc.StencilEnable = true;
			depthDisabledStencilDesc.StencilReadMask = 0xFF;
			depthDisabledStencilDesc.StencilWriteMask = 0xFF;

			//if the pixel is front facing
			depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			//if the pixel is back-facing
			depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			result = m_Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_DepthDisabledStencilState);
			if (FAILED(result)) return false;


			//initalize the depth stencil view
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			//setup the depth stencil view desc
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;

			//create the depth stencil view
			result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
			if (FAILED(result)) return false;

			//bind the render target view and depth stencil buffer to the output render pipeline
			m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

			//setup the raster description which will determine h ow and what polygons will be drawn
			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = true;
			rasterDesc.FillMode = D3D11_FILL_SOLID;
			rasterDesc.FrontCounterClockwise = false;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			//Create the rasterizer state from the desc
			result = m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
			if (FAILED(result)) return false;

			//Now set the rasterizer state
			m_DeviceContext->RSSetState(m_RasterState);

			//Setup the viewport for rendering
			viewport.Width = (float)screenWidth;
			viewport.Height = (float)screenHeight;
			viewport.MaxDepth = 1.0f;
			viewport.MinDepth = 0.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			//Create the viewport
			m_DeviceContext->RSSetViewports(1, &viewport);

			//Setup Projection Matrix
			fieldOfView = 3.14159f / 4.0f;
			screenAspect = (float)screenWidth / (float)screenHeight;

			//Create the matrix
			m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

			//World Matrix
			m_WorldMatrix = XMMatrixIdentity();

			m_OrthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

			return true;
		}

		void Direct3D::Shutdown()
		{
			//before shutdown set to windowed to avoid throwing exceptions
			if(m_SwapChain)
			{
				m_SwapChain->SetFullscreenState(false, NULL);
			}
			if (m_RasterState)
			{	  
				m_RasterState->Release();
				m_RasterState = nullptr;
			}

			if (m_DepthStencilView)
			{	  
				m_DepthStencilView->Release();
				m_DepthStencilView = nullptr;
			}

			if (m_DepthStencilState)
			{	  
				m_DepthStencilState->Release();
				m_DepthStencilState = nullptr;
			}

			if (m_DepthDisabledStencilState)
			{
				m_DepthDisabledStencilState->Release();
				m_DepthDisabledStencilState = nullptr;
			}
				  
			if (m_DepthStencilBuffer)
			{	  
				m_DepthStencilBuffer->Release();
				m_DepthStencilBuffer = nullptr;
			}

			if (m_RenderTargetView)
			{	  
				m_RenderTargetView->Release();
				m_RenderTargetView = nullptr;
			}

			if (m_DeviceContext)
			{	  
				m_DeviceContext->Release();
				m_DeviceContext = nullptr;
			}

			if (m_Device)
			{	  
				m_Device->Release();
				m_Device = nullptr;
			}

			if (m_SwapChain)
			{	  
				m_SwapChain->Release();
				m_SwapChain = nullptr;
			}
		}

		void Direct3D::BeginScene(float red, float green, float blue, float alpha)
		{
			float color[4];

			color[0] = red;
			color[1] = green;
			color[2] = blue;
			color[3] = alpha;

			//clear the back buffer
			m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

			//clear the depth buffer
			m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

		}

		void Direct3D::EndScene()
		{
			if (m_vsync_enabled)
			{
				//lock refresh rate
				m_SwapChain->Present(1, 0);
			} else
			{
				m_SwapChain->Present(0, 0);
			}
		}


		void Direct3D::GetVideoCardInfo(char* cardName, int& memory)
		{
			strcpy_s(cardName, 128, m_VideoCardDescription);
			memory = m_VideoCardMemory;
		}
	}
}
