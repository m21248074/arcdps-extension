#pragma once

#include "Singleton.h"

#include <condition_variable>
#include <Windows.h>
#include <d3d9.h>
#include <d3d11.h>
#include <map>
#include <thread>
#include <wincodec.h>
#include <atlbase.h>

/**
 * Call `Setup()` in `mod_init()`. This is needed, so this class knows about the dll and the directx device!
 * Call `Shutdown()` in `mod_release()`. This is needed, so gw2 does not crash while closing.
 */
template<typename IdType>
class IconLoader : public Singleton<IconLoader<IdType>> {
public:
	IconLoader() {
		mLoadThread = std::jthread([this](std::stop_token pToken){LoadThreadFunc(pToken);});
	}

	~IconLoader() override;

	void Setup(HMODULE new_dll, IDirect3DDevice9* d3d9Device, ID3D11Device* new_d3d11device);
	void* GetTexture(IdType pId, UINT pResourceId);

private:
	class Texture {
	public:
		enum class Status {
			Pending,
			PendingResource,
			Loaded,
			Finished,
		};

		UINT Width = 0;
		UINT Height = 0;
		Status Status = Status::Pending;
		UINT LoadResourceId = 0;

		void LoadIntoDirectXDevice();
		void* GetTexture(UINT pResourceId);
		void Load();
		void LoadFromResource();

		explicit Texture(::IconLoader<IdType>& pIconLoader)
			: mIconLoader(pIconLoader) {}

	private:
		std::unique_ptr<unsigned char[]> mPixelBuffer;
		IDirect3DTexture9* mD9Texture = nullptr;
		ID3D11ShaderResourceView* mD11Texture = nullptr;
		::IconLoader<IdType>& mIconLoader;

		void* GetTexture() const;
	};

	void LoadThreadFunc(std::stop_token stop_token);

	HMODULE mDll = nullptr;
	ID3D11Device* mD3d11Device = nullptr;
	IDirect3DDevice9* mD3d9Device = nullptr;

	std::map<IdType, Texture> mTextures;

	std::jthread mLoadThread;
	std::condition_variable_any mThreadCondition;
	std::mutex mThreadMutex;
};

template <typename IdType>
IconLoader<IdType>::~IconLoader() {
	mLoadThread.request_stop();
	mLoadThread.join();
	mTextures.clear();
}

template<typename IdType>
void IconLoader<IdType>::Setup(HMODULE new_dll, IDirect3DDevice9* new_d3d9Device, ID3D11Device* new_d3d11device) {
	mDll = new_dll;
	mD3d11Device = new_d3d11device;
	mD3d9Device = new_d3d9Device;
}

template<typename IdType>
void* IconLoader<IdType>::GetTexture(IdType pId, UINT pResourceId) {
	if (!mD3d9Device && !mD3d11Device) {
		return nullptr;
	}

	const auto& texture = mTextures.find(pId);
	if (texture == mTextures.end()) {
		const auto& tryEmplace = mTextures.try_emplace(pId, *this);
		return tryEmplace.first->second.GetTexture(pResourceId);
	}

	return texture->second.GetTexture(pResourceId);
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadIntoDirectXDevice() {
	
	// Create texture
	if (mIconLoader.mD3d11Device) {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof desc);
		desc.Width = Width;
		desc.Height = Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		
		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof subResource);
		subResource.pSysMem = static_cast<const void*>(mPixelBuffer.get());
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		
		CComPtr<ID3D11Texture2D> pTexture;
		HRESULT createTexture2DRes = mIconLoader.mD3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);
		if (!SUCCEEDED(createTexture2DRes)) {
			std::string text = "Error creating 2d texture: ";
			text.append(std::to_string(createTexture2DRes));
			throw std::runtime_error(text);
		}
		
		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		
		mIconLoader.mD3d11Device->CreateShaderResourceView(pTexture, &srvDesc, &mD11Texture);
		if (!SUCCEEDED(createTexture2DRes)) {
			// error copying pixels to buffer
			std::string text = "Error creating shader Resource View: ";
			text.append(std::to_string(createTexture2DRes));
			throw std::runtime_error(text);
		}
	} else if (mIconLoader.mD3d9Device) {
		HRESULT createTextureRes = mIconLoader.mD3d9Device->CreateTexture(Width, Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mD9Texture, NULL);
		if (!SUCCEEDED(createTextureRes)) {
			// error creating d3d9 texture
			std::string text = "Error creating d3d9 texture: ";
			text.append(std::to_string(createTextureRes));
			throw std::runtime_error(text);
		}

		D3DLOCKED_RECT rect;
		mD9Texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
		unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
		memcpy(dest, mPixelBuffer.get(), sizeof(char) * Width * Height * 4);
		mD9Texture->UnlockRect(0);
	}

	Status = Status::Finished;
}

template <typename IdType>
void* IconLoader<IdType>::Texture::GetTexture() const {
	if (mD11Texture) {
		return mD11Texture;
	}
	return mD9Texture;
}

template <typename IdType>
void* IconLoader<IdType>::Texture::GetTexture(UINT pResourceId) {
	if (Status == Status::Finished) {
		return GetTexture();
	}

	if (Status == Status::Loaded) {
		LoadIntoDirectXDevice();

		return GetTexture();
	}

	if (Status == Status::Pending) {
		LoadResourceId = pResourceId;
		Status = Status::PendingResource;
	}

	if (mIconLoader.mThreadMutex.try_lock()) {
		mIconLoader.mThreadCondition.notify_one();
		mIconLoader.mThreadMutex.unlock();
	}

	return nullptr;
}

template <typename IdType>
void IconLoader<IdType>::Texture::Load() {
	if (Status == Status::PendingResource) {
		LoadFromResource();
	}
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadFromResource() {
	HRSRC imageResHandle = FindResource(mIconLoader.mDll, MAKEINTRESOURCE(LoadResourceId), L"PNG");
	if (!imageResHandle) {
		// not found
		// std::string text = "Error finding Resource: ";
		// text.append(nameString);
		// throw std::runtime_error(text);
		return;
	}

	// does not need to be freed
	HGLOBAL imageResDataHandle = LoadResource(mIconLoader.mDll, imageResHandle);
	if (!imageResDataHandle) {
		// loading failed
		// std::string text = "Error loading resource: ";
		// text.append(nameString);
		// throw std::runtime_error(text);
		return;
	}

	LPVOID imageFile = LockResource(imageResDataHandle);
	if (!imageFile) {
		// locking failed
		// std::string text = "Error locking resource: ";
		// text.append(nameString);
		// throw std::runtime_error(text);
		return;
	}

	DWORD imageFileSize = SizeofResource(mIconLoader.mDll, imageResHandle);
	if (!imageFileSize) {
		// error getting size of file
		// std::string text = "Error getting Size of Resource: ";
		// text.append(nameString);
		// throw std::runtime_error(text);
		return;
	}

	{
		ULONG_PTR contextToken;
		if (CoGetContextToken(&contextToken) == CO_E_NOTINITIALIZED) {
			HRESULT coInitializeResult = CoInitialize(NULL);
			if (!SUCCEEDED(coInitializeResult)) {
				// error coInitializing instance
				// std::string text = "Error creating WIC intance: ";
				// text.append(nameString);
				// text.append(" - ");
				// text.append(std::to_string(coInitializeResult));
				// throw std::runtime_error(text);
				return;
			}
		}
	}

	CComPtr<IWICImagingFactory> m_pIWICFactory;
	// IWICImagingFactory* m_pIWICFactory = NULL;
	HRESULT createInstance = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIWICFactory));
	if (!SUCCEEDED(createInstance)) {
		// error creating instance
		// std::string text = "Error creating WIC intance: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(createInstance));
		// throw std::runtime_error(text);
		return;
	}

	CComPtr<IWICStream> pIWICStream;
	HRESULT streamRes = m_pIWICFactory->CreateStream(&pIWICStream);
	if (!SUCCEEDED(streamRes)) {
		// creating stream failed
		// std::string text = "Error creating WIC stream: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(streamRes));
		// throw std::runtime_error(text);
		return;
	}

	HRESULT initializeFromMemoryRes = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(imageFile), imageFileSize);
	if (!SUCCEEDED(initializeFromMemoryRes)) {
		// error initializing from memory
		// std::string text = "Error initializing WICStream from memory: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(initializeFromMemoryRes));
		// throw std::runtime_error(text);
		return;
	}

	CComPtr<IWICBitmapDecoder> pIDecoder;
	HRESULT decoderFromStreamRes = m_pIWICFactory->CreateDecoderFromStream(pIWICStream, NULL, WICDecodeMetadataCacheOnLoad, &pIDecoder);
	if (!SUCCEEDED(decoderFromStreamRes)) {
		// error creating decode from stream
		// std::string text = "Error creating decoder from stream: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(decoderFromStreamRes));
		// throw std::runtime_error(text);
		return;
	}

	CComPtr<IWICBitmapFrameDecode> pIDecodeFrame;
	HRESULT getFrameRes = pIDecoder->GetFrame(0, &pIDecodeFrame);
	if (!SUCCEEDED(getFrameRes)) {
		// error getting frame from decoder
		// std::string text = "Error getting frame 0: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(getFrameRes));
		// throw std::runtime_error(text);
		return;
	}

	WICPixelFormatGUID pixelFormat;
	HRESULT pixelFormatRes = pIDecodeFrame->GetPixelFormat(&pixelFormat);
	if (!SUCCEEDED(pixelFormatRes)) {
		// error getting pixel format
		// std::string text = "Error getting pixel format: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(pixelFormatRes));
		// throw std::runtime_error(text);
		return;
	}

	CComPtr<IWICComponentInfo> pIComponentInfo;
	HRESULT componentInfoRes = m_pIWICFactory->CreateComponentInfo(pixelFormat, &pIComponentInfo);
	if (!SUCCEEDED(componentInfoRes)) {
		// error creating component info
		// std::string text = "Error creating component info: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(componentInfoRes));
		// throw std::runtime_error(text);
		return;
	}

	CComPtr<IWICPixelFormatInfo> pIPixelFormatInfo;
	// HRESULT pixelFormatInfoRes = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));
	HRESULT pixelFormatInfoRes = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));
	if (!SUCCEEDED(pixelFormatInfoRes)) {
		// error querying pixel format info
		// std::string text = "Error querying format info: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(pixelFormatInfoRes));
		// throw std::runtime_error(text);
		return;
	}

	UINT bitsPerPixel;
	HRESULT bitsPerPixelRes = pIPixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	if (!SUCCEEDED(bitsPerPixelRes)) {
		// error getting bits per pixel
		// std::string text = "Error getting bits per pixel: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(bitsPerPixelRes));
		// throw std::runtime_error(text);
		return;
	}

	pIDecodeFrame->GetSize(&Width, &Height);

	float totalPixels = (float)(bitsPerPixel) * (float)(Width) + 7.f; // +7 forces to next byte if needed
	UINT stride = (UINT)(totalPixels / 8.f);

	UINT totalPixelAmount = Width * Height * stride;

	// unsigned char* pixelBuffer = new unsigned char[1048576]; // 1MB size
	std::unique_ptr<unsigned char[]> pixelBuffer(new unsigned char[totalPixelAmount]);
	mPixelBuffer = std::move(pixelBuffer);
	HRESULT copyPixelsRes = pIDecodeFrame->CopyPixels(NULL, stride, totalPixelAmount, mPixelBuffer.get());
	if (!SUCCEEDED(copyPixelsRes)) {
		// error copying pixels to buffer
		// std::string text = "Error copying pixels: ";
		// text.append(nameString);
		// text.append(" - ");
		// text.append(std::to_string(copyPixelsRes));
		// throw std::runtime_error(text);
		return;
	}

	Status = Status::Loaded;
}

template<typename IdType>
void IconLoader<IdType>::LoadThreadFunc(std::stop_token stop_token) {
	while (true) {
		mThreadCondition.wait(mThreadMutex, stop_token, []{ return true; });

		if (stop_token.stop_requested()) return;

		for (auto& texture : mTextures) {
			texture.second.Load();
		}
	}
}
