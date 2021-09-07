#include "Icon.h"

#include <stdexcept>
#include <string>
#include <thread>
#include <wincodec.h>

IconLoader iconLoader;

Icon::Icon(UINT name, HMODULE dll, ID3D11Device* d3d11Device) {
	std::string nameString(std::to_string(name));
	
	HRSRC imageResHandle = FindResource(dll, MAKEINTRESOURCE(name), L"PNG");
	if (!imageResHandle) {
		// not found
		std::string text = "Error finding Resource: "; 
		text.append(nameString);
		throw std::runtime_error(text);
	}

	// does not need to be freed
	HGLOBAL imageResDataHandle = LoadResource(dll, imageResHandle);
	if (!imageResDataHandle) {
		// loading failed
		std::string text = "Error loading resource: ";
		text.append(nameString);
		throw std::runtime_error(text);
	}

	LPVOID imageFile = LockResource(imageResDataHandle);
	if (!imageFile) {
		// locking failed
		std::string text = "Error locking resource: ";
		text.append(nameString);
		throw std::runtime_error(text);
	}

	DWORD imageFileSize = SizeofResource(dll, imageResHandle);
	if (!imageFileSize) {
		// error getting size of file
		std::string text = "Error getting Size of Resource: ";
		text.append(nameString);
		throw std::runtime_error(text);
	}

	HRESULT coInitializeResult = CoInitialize(NULL);
	if (!SUCCEEDED(coInitializeResult)) {
		// error coInitalizing instance
		std::string text = "Error creating WIC intance: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(coInitializeResult));
		throw std::runtime_error(text);
	}

	IWICImagingFactory* m_pIWICFactory = NULL;
	HRESULT createInstance = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIWICFactory));
	if (!SUCCEEDED(createInstance)) {
		// error creating instance
		std::string text = "Error creating WIC intance: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(createInstance));
		throw std::runtime_error(text);
	}

	IWICStream* pIWICStream = NULL;
	HRESULT streamRes = m_pIWICFactory->CreateStream(&pIWICStream);
	if (!SUCCEEDED(streamRes)) {
		// creating stream failed
		std::string text = "Error creating WIC stream: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(streamRes));
		throw std::runtime_error(text);
	}

	HRESULT initializeFromMemoryRes = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(imageFile), imageFileSize);
	if (!SUCCEEDED(initializeFromMemoryRes)) {
		// error initializing from memory
		std::string text = "Error initializing WICStream from memory: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(initializeFromMemoryRes));
		throw std::runtime_error(text);
	}

	IWICBitmapDecoder* pIDecoder = NULL;
	HRESULT decoderFromStreamRes = m_pIWICFactory->CreateDecoderFromStream(pIWICStream, NULL, WICDecodeMetadataCacheOnLoad, &pIDecoder);
	if (!SUCCEEDED(decoderFromStreamRes)) {
		// error creating decode from stream
		std::string text = "Error creating decoder from stream: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(decoderFromStreamRes));
		throw std::runtime_error(text);
	}

	IWICBitmapFrameDecode* pIDecodeFrame = NULL;
	HRESULT getFrameRes = pIDecoder->GetFrame(0, &pIDecodeFrame);
	if (!SUCCEEDED(getFrameRes)) {
		// error getting frame from decoder
		std::string text = "Error getting frame 0: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(getFrameRes));
		throw std::runtime_error(text);
	}

	WICPixelFormatGUID pixelFormat;
	HRESULT pixelFormatRes = pIDecodeFrame->GetPixelFormat(&pixelFormat);
	if (!SUCCEEDED(pixelFormatRes)) {
		// error getting pixel format
		std::string text = "Error getting pixel format: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(pixelFormatRes));
		return;
	}

	IWICComponentInfo* pIComponentInfo = NULL;
	HRESULT componentInfoRes = m_pIWICFactory->CreateComponentInfo(pixelFormat, &pIComponentInfo);
	if (!SUCCEEDED(componentInfoRes)) {
		// error creating component info
		std::string text = "Error creating component info: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(componentInfoRes));
		return;
	}

	IWICPixelFormatInfo* pIPixelFormatInfo;
	HRESULT pixelFormatInfoRes = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));
	if (!SUCCEEDED(pixelFormatInfoRes)) {
		// error querying pixel format info
		std::string text = "Error querying format info: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(pixelFormatInfoRes));
		return;
	}

	UINT bitsPerPixel;
	HRESULT bitsPerPixelRes = pIPixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	if (!SUCCEEDED(bitsPerPixelRes)) {
		// error getting bits per pixel
		std::string text = "Error getting bits per pixel: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(bitsPerPixelRes));
		return;
	}

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
	}

	// Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // d3d9: D3DFMT_A8R8G8B8
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = pixelBuffer;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT createTexture2DRes = d3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);
	if (!SUCCEEDED(createTexture2DRes)) {
		// error copying pixels to buffer
		std::string text = "Error creating 2d texture: ";
		text.append(nameString);
		text.append(" - ");
		text.append(std::to_string(createTexture2DRes));
		return;
	}

	// Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, &texture);
    pTexture->Release();
	
	// cleanup / this will not be called when function is failing, which results in a small leak
	delete[] pixelBuffer;
	m_pIWICFactory->Release();
	pIWICStream->Release();
	pIDecoder->Release();
	pIDecodeFrame->Release();
	pIComponentInfo->Release();
	pIPixelFormatInfo->Release();
}

Icon::~Icon() {
	texture->Release();
}

void IconLoader::Setup(HMODULE new_dll, ID3D11Device* new_d3d11device) {
	dll = new_dll;
	d3d11device = new_d3d11device;
}

ID3D11ShaderResourceView* IconLoader::getTexture(UINT name) {
	auto texture = textures.find(name);
	if (texture != textures.end()) {
		return texture->second.texture;
	}

	queueMutex.lock();
	if (std::find(queue.begin(), queue.end(), name) == queue.end()) {
		queue.push_back(name);
	}
	queueMutex.unlock();

	bool expected = false;
	if (thread_running.compare_exchange_strong(expected, true)) {
		std::thread t(&IconLoader::thread_fun, this);
		t.detach();
	}

	return nullptr;
}

void IconLoader::thread_fun() {
	while (!queue.empty()) {
		queueMutex.lock();
		UINT name = queue.back();
		queue.pop_back();
		queueMutex.unlock();

		if (textures.find(name) != textures.end()) {
			// texture already loaded, skip
			continue;
		}

		// load texture (texture is loaded in Icon constructor)
		textures.try_emplace(name, name, dll, d3d11device);
	}
	thread_running = false;
}
