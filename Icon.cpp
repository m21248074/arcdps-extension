#include "Icon.h"

#include <stdexcept>
#include <string>
#include <thread>
#include <wincodec.h>

#ifndef PRINT_LINE
#define PRINT_LINE()
#endif

IconLoader iconLoader;

Icon::Icon(UINT name, HMODULE dll, IDirect3DDevice9* d3d9Device, ID3D11Device* d3d11Device) {
	PRINT_LINE()

	std::string nameString(std::to_string(name));

	PRINT_LINE()

	HRSRC imageResHandle = FindResource(dll, MAKEINTRESOURCE(name), L"PNG");
	if (!imageResHandle) {
		// not found
		std::string text = "Error finding Resource: ";
		text.append(nameString);
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	// does not need to be freed
	HGLOBAL imageResDataHandle = LoadResource(dll, imageResHandle);
	if (!imageResDataHandle) {
		// loading failed
		std::string text = "Error loading resource: ";
		text.append(nameString);
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	LPVOID imageFile = LockResource(imageResDataHandle);
	if (!imageFile) {
		// locking failed
		std::string text = "Error locking resource: ";
		text.append(nameString);
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	DWORD imageFileSize = SizeofResource(dll, imageResHandle);
	if (!imageFileSize) {
		// error getting size of file
		std::string text = "Error getting Size of Resource: ";
		text.append(nameString);
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	HRESULT coInitializeResult = CoInitialize(NULL);
	if (!SUCCEEDED(coInitializeResult)) {
		// error coInitalizing instance
		std::string text = "Error creating WIC intance: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(coInitializeResult));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICImagingFactory* m_pIWICFactory = NULL;
	HRESULT createInstance = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIWICFactory));
	if (!SUCCEEDED(createInstance)) {
		// error creating instance
		std::string text = "Error creating WIC intance: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(createInstance));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICStream* pIWICStream = NULL;
	HRESULT streamRes = m_pIWICFactory->CreateStream(&pIWICStream);
	if (!SUCCEEDED(streamRes)) {
		// creating stream failed
		std::string text = "Error creating WIC stream: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(streamRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	HRESULT initializeFromMemoryRes = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(imageFile), imageFileSize);
	if (!SUCCEEDED(initializeFromMemoryRes)) {
		// error initializing from memory
		std::string text = "Error initializing WICStream from memory: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(initializeFromMemoryRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICBitmapDecoder* pIDecoder = NULL;
	HRESULT decoderFromStreamRes = m_pIWICFactory->CreateDecoderFromStream(pIWICStream, NULL, WICDecodeMetadataCacheOnLoad, &pIDecoder);
	if (!SUCCEEDED(decoderFromStreamRes)) {
		// error creating decode from stream
		std::string text = "Error creating decoder from stream: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(decoderFromStreamRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICBitmapFrameDecode* pIDecodeFrame = NULL;
	HRESULT getFrameRes = pIDecoder->GetFrame(0, &pIDecodeFrame);
	if (!SUCCEEDED(getFrameRes)) {
		// error getting frame from decoder
		std::string text = "Error getting frame 0: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(getFrameRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	WICPixelFormatGUID pixelFormat;
	HRESULT pixelFormatRes = pIDecodeFrame->GetPixelFormat(&pixelFormat);
	if (!SUCCEEDED(pixelFormatRes)) {
		// error getting pixel format
		std::string text = "Error getting pixel format: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(pixelFormatRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICComponentInfo* pIComponentInfo = NULL;
	HRESULT componentInfoRes = m_pIWICFactory->CreateComponentInfo(pixelFormat, &pIComponentInfo);
	if (!SUCCEEDED(componentInfoRes)) {
		// error creating component info
		std::string text = "Error creating component info: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(componentInfoRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	IWICPixelFormatInfo* pIPixelFormatInfo;
	HRESULT pixelFormatInfoRes = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));
	if (!SUCCEEDED(pixelFormatInfoRes)) {
		// error querying pixel format info
		std::string text = "Error querying format info: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(pixelFormatInfoRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	UINT bitsPerPixel;
	HRESULT bitsPerPixelRes = pIPixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	if (!SUCCEEDED(bitsPerPixelRes)) {
		// error getting bits per pixel
		std::string text = "Error getting bits per pixel: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(bitsPerPixelRes));
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	pIDecodeFrame->GetSize(&width, &height);

	float totalPixels = bitsPerPixel * width + 7; // +7 forces to next byte if needed
	UINT stride = totalPixels / 8;

	unsigned char* pixelBuffer = new unsigned char[1048576]; // 1MB size
	HRESULT copyPixelsRes = pIDecodeFrame->CopyPixels(NULL, stride, 1048576, pixelBuffer);
	if (!SUCCEEDED(copyPixelsRes)) {
		// error copying pixels to buffer
		std::string text = "Error copying pixels: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(copyPixelsRes));
		delete[] pixelBuffer;
		return;
		// throw std::runtime_error(text);
	}

	PRINT_LINE()

	// Create texture
	if (d3d11Device) {
		PRINT_LINE()
		
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof desc);
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		
		PRINT_LINE()
		
		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof subResource);
		subResource.pSysMem = pixelBuffer;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		
		PRINT_LINE()
		
		ID3D11Texture2D* pTexture = nullptr;
		HRESULT createTexture2DRes = d3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);
		if (!SUCCEEDED(createTexture2DRes)) {
			pTexture->Release();
			// error copying pixels to buffer
			std::string text = "Error creating 2d texture: ";
			text.append(nameString);
			text.append(" - ");
			text.append(std::to_string(createTexture2DRes));
			PRINT_LINE()
			return;
			// throw std::runtime_error(text);
		}
		
		PRINT_LINE()
		
		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		
		PRINT_LINE()
		
		d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, &d11texture);
		if (!SUCCEEDED(createTexture2DRes)) {
			// error copying pixels to buffer
			std::string text = "Error creating shader Resource View: ";
			text.append(nameString);
			text.append(" - ");
			text.append(std::to_string(createTexture2DRes));
			PRINT_LINE()
			return;
		}
		PRINT_LINE()
		
		pTexture->Release();
		
		PRINT_LINE()
	} else if (d3d9Device) {
		PRINT_LINE()

		HRESULT createTextureRes = d3d9Device->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d9texture, NULL);
		if (!SUCCEEDED(createTextureRes)) {
			// error creating d3d9 texture
			std::string text = "Error creating d3d9 texture: ";
			text.append(nameString);
			text.append(" - ");
			text.append(std::to_string(createTextureRes));
			delete[] pixelBuffer;
			return;
		}

		PRINT_LINE()

		D3DLOCKED_RECT rect;
		d9texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
		unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
		memcpy(dest, pixelBuffer, sizeof(char) * width * height * 4);
		d9texture->UnlockRect(0);
	}

	PRINT_LINE()

	// cleanup / this will not be called when function is failing, which results in a small leak
	delete[] pixelBuffer;
	m_pIWICFactory->Release();
	pIWICStream->Release();
	pIDecoder->Release();
	pIDecodeFrame->Release();
	pIComponentInfo->Release();
	pIPixelFormatInfo->Release();

	PRINT_LINE()
}

Icon::~Icon() {
	PRINT_LINE()
	if (d9texture)
		d9texture->Release();
	if (d11texture)
		d11texture->Release();
	PRINT_LINE()
}

void* Icon::getTexture() const {
	if (d9texture) {
		return d9texture;
	}
	if (d11texture) {
		return d11texture;
	}
	return nullptr;
}

void IconLoader::Setup(HMODULE new_dll, IDirect3DDevice9* new_d3d9Device, ID3D11Device* new_d3d11device) {
	dll = new_dll;
	d3d11device = new_d3d11device;
	d3d9Device = new_d3d9Device;
}

void* IconLoader::getTexture(UINT name) {
	PRINT_LINE()

	if (!d3d9Device && !d3d11device) {
		return nullptr;
	}

	auto texture = textures.find(name);
	if (texture != textures.end()) {
		const auto& tex = texture->second;
		return tex.getTexture();
	}

	const auto& tryEmplace = textures.try_emplace(name, name, dll, d3d9Device, d3d11device);

	return tryEmplace.first->second.getTexture();
}

void IconLoader::Shutdown() {
	textures.clear();
}
