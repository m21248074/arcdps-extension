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
#include <filesystem>
#include <ranges>
#include <utility>
#include <variant>

#ifndef ARCDPS_EXTENSION_NO_CPR
#include <cpr/cpr.h>
#endif

/**
 * Call `Setup()` in `mod_init()`. This is needed, so this class knows about the dll and the directx device!
 *
 * Always have an enum or any other list of numbers as unique ID.
 * Load textures:
 * - GetTexture(pResourceId):
 *     Load Texture from a resource within this DLL
 * - GetTexture(pFilePath):
 *	   Load Texture from file.
 *	   There is a direct conversion from string to filesystem::path.
 *	   If such a file is not found, you will always get a nullptr as result (no file reload performed)
 * - GetTexture(pUrl):
 *     Load Texture from url.
 *     It will be saved in AppData/Local/Temp/GW2-arcdps-extension and loaded from there.
 *	   If the file already exists, it will just be loaded.
 *	   Only available if cpr is loaded (can be disabled with `ARCDPS_EXTENSION_NO_CPR`)
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
	void* GetTexture(IdType pId, const std::filesystem::path& pFilePath);

#ifndef ARCDPS_EXTENSION_NO_CPR
	void* GetTexture(IdType pId, const cpr::Url& pUrl);
#endif

private:
	class Texture {
	public:
		/**
		 *          /  PendingResource  \
		 * Pending                         Loaded  -  Finished
		 *          \  PendingFile      /
		 *
		 *	Fallout: Error
		 */
		enum class Status {
			Pending,
			PendingResource,
			PendingFile,
			PendingCpr,
			Loaded,
			Finished,
			Error,
		};

		UINT Width = 0;
		UINT Height = 0;
		Status Status = Status::Pending;

		void* GetTexture(UINT pResourceId);
		void* GetTexture(const std::filesystem::path& pFilePath);
	#ifndef ARCDPS_EXTENSION_NO_CPR
		void* GetTexture(const cpr::Url& pUrl);
		void LoadFromCpr();
#endif
		void Load();

		explicit Texture(::IconLoader<IdType>& pIconLoader)
			: mIconLoader(pIconLoader) {}

	private:
		std::unique_ptr<unsigned char[]> mPixelBuffer;
		IDirect3DTexture9* mD9Texture = nullptr;
		ID3D11ShaderResourceView* mD11Texture = nullptr;
		::IconLoader<IdType>& mIconLoader;
	#ifndef ARCDPS_EXTENSION_NO_CPR
		std::variant<UINT, std::filesystem::path, cpr::Url> mLoadTarget;
	#else
		std::variant<UINT, std::filesystem::path> mLoadTarget;
	#endif
		DXGI_FORMAT mDxgiFormat = DXGI_FORMAT_UNKNOWN;

		void* GetTexture() const;
		void LoadFrame(const CComPtr<IWICBitmapFrameDecode>& pIDecodeFrame, const CComPtr<IWICImagingFactory>& pIWICFactory);
		DXGI_FORMAT GetFormatDx11(WICPixelFormatGUID pPixelFormat);
		_D3DFORMAT GetFormatDx9();
		void LoadIntoDirectXDevice();
		void LoadFromFile();
		void LoadFromResource();
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
void* IconLoader<IdType>::GetTexture(IdType pId, const std::filesystem::path& pFilePath) {
	if (!mD3d9Device && !mD3d11Device) {
		return nullptr;
	}

	const auto& texture = mTextures.find(pId);
	if (texture == mTextures.end()) {
		const auto& tryEmplace = mTextures.try_emplace(pId, *this);
		return tryEmplace.first->second.GetTexture(pFilePath);
	}

	return texture->second.GetTexture(pFilePath);
}

/*
 * Array of needed GUID conversions.
 * CurrentGUID -> WantedGUID
 */
static const std::vector<std::pair<GUID, GUID>> WIC_CONVERT = 
{
    // Note target GUID in this conversion table must be one of those directly supported formats (above).

    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

#ifdef DXGI_1_2_FORMATS

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

#else

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGRA5551,         GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGR565,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

#endif // DXGI_1_2_FORMATS

    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

    { GUID_WICPixelFormat96bppRGBFixedPoint,    GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif

    // We don't support n-channel formats
};

template <typename IdType>
DXGI_FORMAT IconLoader<IdType>::Texture::GetFormatDx11(WICPixelFormatGUID pPixelFormat) {
	if (pPixelFormat == GUID_WICPixelFormat128bppRGBAFloat) {
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	if (pPixelFormat == GUID_WICPixelFormat64bppRGBAHalf) {
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	if (pPixelFormat == GUID_WICPixelFormat64bppRGBA) {
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppRGBA) {
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppBGRA) {
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppBGR) {
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppRGBA1010102XR) {
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppRGBA1010102) {
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppRGBE) {
		return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	}
	if (pPixelFormat == GUID_WICPixelFormat16bppBGRA5551) {
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat16bppBGR565) {
		return DXGI_FORMAT_B5G6R5_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat32bppGrayFloat) {
		return DXGI_FORMAT_R32_FLOAT;
	}
	if (pPixelFormat == GUID_WICPixelFormat16bppGrayHalf) {
		return DXGI_FORMAT_R16_FLOAT;
	}
	if (pPixelFormat == GUID_WICPixelFormat16bppGray) {
		return DXGI_FORMAT_R16_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat8bppGray) {
		return DXGI_FORMAT_R8_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat8bppAlpha) {
		return DXGI_FORMAT_A8_UNORM;
	}
	if (pPixelFormat == GUID_WICPixelFormat96bppRGBFloat) {
		return DXGI_FORMAT_R32G32B32_FLOAT;
	}

	throw std::runtime_error("Given DX11 Format is not supported!");
}

static const std::map<DXGI_FORMAT, _D3DFORMAT> DX9_FORMAT = {
	{DXGI_FORMAT_B8G8R8A8_UNORM , D3DFMT_A8R8G8B8},
	{DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, D3DFMT_A8R8G8B8},
	{DXGI_FORMAT_B8G8R8X8_UNORM, D3DFMT_X8R8G8B8},
	{DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, D3DFMT_X8R8G8B8},
	{DXGI_FORMAT_B5G6R5_UNORM, D3DFMT_R5G6B5},
	// {DXGI_FORMAT_B5G5R5A1_UNORM, D3DFMT_A1R5G5B5},
	// {DXGI_FORMAT_B4G4R4A4_UNORM, D3DFMT_A4R4G4B4},
	{DXGI_FORMAT_A8_UNORM, D3DFMT_A8},
	{DXGI_FORMAT_R8G8B8A8_UNORM, D3DFMT_A8B8G8R8}, // TODO
	{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3DFMT_A8B8G8R8}, // TODO
	{DXGI_FORMAT_R16G16_UNORM, D3DFMT_G16R16},
	{DXGI_FORMAT_R16G16B16A16_UNORM, D3DFMT_A16B16G16R16},
	{DXGI_FORMAT_R8_UNORM, D3DFMT_L8},
	// {DXGI_FORMAT_R8G8_UNORM, D3DFMT_A8L8},
	// {DXGI_FORMAT_R8G8_SNORM, D3DFMT_V8U8},
	// {DXGI_FORMAT_R8G8B8A8_SNORM, D3DFMT_Q8W8V8U8},
	// {DXGI_FORMAT_R16G16_SNORM, D3DFMT_V16U16},
	// {DXGI_FORMAT_G8R8_G8B8_UNORM, D3DFMT_R8G8_B8G8},
	// {DXGI_FORMAT_R8G8_B8G8_UNORM, D3DFMT_G8R8_G8B8},
	// {DXGI_FORMAT_BC1_UNORM, D3DFMT_DXT1},
	// {DXGI_FORMAT_BC1_UNORM_SRGB, D3DFMT_DXT1},
	// {DXGI_FORMAT_BC1_UNORM, D3DFMT_DXT2},
	// {DXGI_FORMAT_BC1_UNORM_SRGB, D3DFMT_DXT2},
	// {DXGI_FORMAT_BC2_UNORM, D3DFMT_DXT3},
	// {DXGI_FORMAT_BC2_UNORM_SRGB, D3DFMT_DXT3},
	// {DXGI_FORMAT_BC2_UNORM, D3DFMT_DXT4},
	// {DXGI_FORMAT_BC2_UNORM_SRGB, D3DFMT_DXT4},
	// {DXGI_FORMAT_BC3_UNORM, D3DFMT_DXT5},
	// {DXGI_FORMAT_BC3_UNORM_SRGB, D3DFMT_DXT5},
	// {DXGI_FORMAT_D16_UNORM, D3DFMT_D16},
	// {DXGI_FORMAT_D32_FLOAT, D3DFMT_D32F_LOCKABLE},
	{DXGI_FORMAT_R16_UNORM, D3DFMT_L16},
	// {DXGI_FORMAT_R16_UINT, D3DFMT_INDEX16},
	// {DXGI_FORMAT_R32_UINT, D3DFMT_INDEX32},
	// {DXGI_FORMAT_R16G16B16A16_SNORM, D3DFMT_Q16W16V16U16},
	{DXGI_FORMAT_R16_FLOAT, D3DFMT_R16F},
	{DXGI_FORMAT_R16G16_FLOAT, D3DFMT_G16R16F},
	{DXGI_FORMAT_R16G16B16A16_FLOAT, D3DFMT_A16B16G16R16F},
	{DXGI_FORMAT_R32_FLOAT, D3DFMT_R32F},
	{DXGI_FORMAT_R32G32_FLOAT, D3DFMT_G32R32F},
	{DXGI_FORMAT_R32G32B32A32_FLOAT, D3DFMT_A32B32G32R32F},
};

template <typename IdType>
_D3DFORMAT IconLoader<IdType>::Texture::GetFormatDx9() {
	const auto& pair = DX9_FORMAT.find(mDxgiFormat);
	if (pair != DX9_FORMAT.end()) {
		return pair->second;
	}
	throw std::runtime_error("Given DX9 Format is not supported!");
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
		desc.Format = mDxgiFormat;
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
		srvDesc.Format = mDxgiFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		
		mIconLoader.mD3d11Device->CreateShaderResourceView(pTexture, &srvDesc, &mD11Texture);
		if (FAILED(createTexture2DRes)) {
			// error copying pixels to buffer
			std::string text = "Error creating shader Resource View: ";
			text.append(std::to_string(createTexture2DRes));
			throw std::runtime_error(text);
		}
	} else if (mIconLoader.mD3d9Device) {
		_D3DFORMAT d3Dformat = GetFormatDx9();

		// This is really dirty, but i don't know how to fix it properly :(
		// Also DX9 support will be dropped in the near future.
		if (d3Dformat == D3DFMT_A8B8G8R8) {
			// convert from abgr to argb
			uint8_t* point = mPixelBuffer.get();
			size_t size = Width * Height;
			uint8_t val = 0;
			for (size_t i = 0; i < size; ++i) {
				val = point[0];
				point[0] = point[2];
				point[2] = val;
				point += 4;
			}

			d3Dformat = D3DFMT_A8R8G8B8;
		}
	
		HRESULT createTextureRes = mIconLoader.mD3d9Device->CreateTexture(Width, Height, 1, 0, d3Dformat, D3DPOOL_MANAGED, &mD9Texture, NULL);
		if (FAILED(createTextureRes)) {
			// error creating d3d9 texture
			std::string text = "Error creating d3d9 texture: ";
			text.append(std::to_string(createTextureRes));
			throw std::runtime_error(text);
		}

		D3DLOCKED_RECT rect;
		mD9Texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
		unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
		memcpy(dest, mPixelBuffer.get(), sizeof(uint8_t) * Width * Height * 4);
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
		mLoadTarget = pResourceId;
		Status = Status::PendingResource;
	}

	if (mIconLoader.mThreadMutex.try_lock()) {
		mIconLoader.mThreadCondition.notify_one();
		mIconLoader.mThreadMutex.unlock();
	}

	return nullptr;
}

template <typename IdType>
void* IconLoader<IdType>::Texture::GetTexture(const std::filesystem::path& pFilePath) {
	if (Status == Status::Finished) {
		return GetTexture();
	}

	if (Status == Status::Loaded) {
		LoadIntoDirectXDevice();

		return GetTexture();
	}

	if (Status == Status::Pending) {
		mLoadTarget = pFilePath;
		Status = Status::PendingFile;
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
		return;
	}
	if (Status == Status::PendingFile) {
		LoadFromFile();
		return;
	}
#ifndef ARCDPS_EXTENSION_NO_CPR
	if (Status == Status::PendingCpr) {
		LoadFromCpr();
	}
#endif
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadFromFile() {
	const auto& path = std::get<std::filesystem::path>(mLoadTarget);
	if (!exists(path)) {
		Status = Status::Error;
		return;
	}

	{
		ULONG_PTR contextToken;
		if (CoGetContextToken(&contextToken) == CO_E_NOTINITIALIZED) {
			HRESULT coInitializeResult = CoInitialize(NULL);
			if (FAILED(coInitializeResult)) {
				return;
			}
		}
	}

	CComPtr<IWICImagingFactory> pIWICFactory;
	// IWICImagingFactory* m_pIWICFactory = NULL;
	HRESULT createInstance = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	if (FAILED(createInstance)) {
		return;
	}

	CComPtr<IWICBitmapDecoder> wicDecoder;
	HRESULT fromFilenameRes = pIWICFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &wicDecoder);
	if (FAILED(fromFilenameRes)) {
		return;
	}

	CComPtr<IWICBitmapFrameDecode> pIDecodeFrame;
	HRESULT getFrameRes = wicDecoder->GetFrame(0, &pIDecodeFrame);
	if (FAILED(getFrameRes)) {
		return;
	}

	LoadFrame(pIDecodeFrame, pIWICFactory);
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadFrame(const CComPtr<IWICBitmapFrameDecode>& pIDecodeFrame, const CComPtr<IWICImagingFactory>& pIWICFactory) {
	HRESULT getSizeRes = pIDecodeFrame->GetSize( &Width, &Height );
	if (FAILED(getSizeRes)) {
		return;
	}

	if (Width <= 0 || Height <= 0) {
		return;
	}

	// get pixel format (color-depth)
    WICPixelFormatGUID pixelFormat;
	HRESULT pixelFormatRes = pIDecodeFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(pixelFormatRes)) {
		return;
	}

	// not auto cause intellisense is not able to deduct it...
	const std::ranges::iterator_t<decltype(WIC_CONVERT)>& convertFormat = std::ranges::find_if(WIC_CONVERT, [&pixelFormat](const auto& pPair) { return std::get<0>(pPair) == pixelFormat; });

	WICPixelFormatGUID targetFormat = pixelFormat;

	if (convertFormat != WIC_CONVERT.end()) {
		targetFormat = convertFormat->second;
	}

	CComPtr<IWICComponentInfo> pIComponentInfo;
	HRESULT componentInfoRes = pIWICFactory->CreateComponentInfo(targetFormat, &pIComponentInfo);
	if (FAILED(componentInfoRes)) {
		return;
	}

	WICComponentType componentType;
	HRESULT componentTypeRes = pIComponentInfo->GetComponentType(&componentType);
    if (FAILED(componentTypeRes)) {
	    return;
    }

	if ( componentType != WICPixelFormat ) {
		return;
	}

	CComPtr<IWICPixelFormatInfo> pIPixelFormatInfo;
	HRESULT pixelFormatInfoRes = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));
	if (FAILED(pixelFormatInfoRes)) {
		return;
	}

	UINT bitsPerPixel;
	HRESULT bitsPerPixelRes = pIPixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	if (FAILED(bitsPerPixelRes)) {
		return;
	}

	// Allocate temporary memory for image
    size_t rowPitch = ( Width * bitsPerPixel + 7 ) / 8;
    size_t imageSize = rowPitch * Height;

    std::unique_ptr<uint8_t[]> pixelBuffer(new uint8_t[imageSize]);
	mPixelBuffer = std::move(pixelBuffer);

	if (convertFormat == WIC_CONVERT.end()) {
		// no conversion needed, just copy it
		HRESULT copyPixelsRes = pIDecodeFrame->CopyPixels(NULL, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), mPixelBuffer.get());
		if (FAILED(copyPixelsRes)) {
			return;
		}
	} else {
		// convert it
		CComPtr<IWICFormatConverter> formatConverter;
		HRESULT formatConverterRes = pIWICFactory->CreateFormatConverter(&formatConverter);
		if (FAILED(formatConverterRes)) {
			return;
		}

		HRESULT initConverterRes = formatConverter->Initialize(pIDecodeFrame, targetFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(initConverterRes)) {
			return;
		}

		HRESULT copyPixelsRes = formatConverter->CopyPixels(NULL, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), mPixelBuffer.get());
		if (FAILED(copyPixelsRes)) {
			return;
		}
	}

	mDxgiFormat = GetFormatDx11(targetFormat);

	Status = Status::Loaded;
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadFromResource() {
	HRSRC imageResHandle = FindResource(mIconLoader.mDll, MAKEINTRESOURCE(std::get<UINT>(mLoadTarget)), L"PNG");
	if (!imageResHandle) {
		return;
	}

	// does not need to be freed
	HGLOBAL imageResDataHandle = LoadResource(mIconLoader.mDll, imageResHandle);
	if (!imageResDataHandle) {
		return;
	}

	LPVOID imageFile = LockResource(imageResDataHandle);
	if (!imageFile) {
		return;
	}

	DWORD imageFileSize = SizeofResource(mIconLoader.mDll, imageResHandle);
	if (!imageFileSize) {
		return;
	}

	{
		ULONG_PTR contextToken;
		if (CoGetContextToken(&contextToken) == CO_E_NOTINITIALIZED) {
			HRESULT coInitializeResult = CoInitialize(NULL);
			if (FAILED(coInitializeResult)) {
				return;
			}
		}
	}

	CComPtr<IWICImagingFactory> pIWICFactory;
	// IWICImagingFactory* m_pIWICFactory = NULL;
	HRESULT createInstance = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));
	if (FAILED(createInstance)) {
		return;
	}

	CComPtr<IWICStream> pIWICStream;
	HRESULT streamRes = pIWICFactory->CreateStream(&pIWICStream);
	if (FAILED(streamRes)) {
		return;
	}

	HRESULT initializeFromMemoryRes = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(imageFile), imageFileSize);
	if (FAILED(initializeFromMemoryRes)) {
		return;
	}

	CComPtr<IWICBitmapDecoder> pIDecoder;
	HRESULT decoderFromStreamRes = pIWICFactory->CreateDecoderFromStream(pIWICStream, NULL, WICDecodeMetadataCacheOnLoad, &pIDecoder);
	if (FAILED(decoderFromStreamRes)) {
		return;
	}

	CComPtr<IWICBitmapFrameDecode> pIDecodeFrame;
	HRESULT getFrameRes = pIDecoder->GetFrame(0, &pIDecodeFrame);
	if (FAILED(getFrameRes)) {
		return;
	}

	LoadFrame(pIDecodeFrame, pIWICFactory);
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

#ifndef ARCDPS_EXTENSION_NO_CPR

template <typename IdType>
void* IconLoader<IdType>::GetTexture(IdType pId, const cpr::Url& pUrl) {
	if (!mD3d9Device && !mD3d11Device) {
		return nullptr;
	}

	const auto& texture = mTextures.find(pId);
	if (texture == mTextures.end()) {
		const auto& tryEmplace = mTextures.try_emplace(pId, *this);
		return tryEmplace.first->second.GetTexture(pUrl);
	}

	return texture->second.GetTexture(pUrl);
}

template <typename IdType>
void* IconLoader<IdType>::Texture::GetTexture(const cpr::Url& pUrl) {
	if (Status == Status::Finished) {
		return GetTexture();
	}

	if (Status == Status::Loaded) {
		LoadIntoDirectXDevice();

		return GetTexture();
	}

	if (Status == Status::Pending) {
		mLoadTarget = pUrl;
		Status = Status::PendingCpr;
	}

	if (mIconLoader.mThreadMutex.try_lock()) {
		mIconLoader.mThreadCondition.notify_one();
		mIconLoader.mThreadMutex.unlock();
	}

	return nullptr;
}

template <typename IdType>
void IconLoader<IdType>::Texture::LoadFromCpr() {
	auto& url = std::get<cpr::Url>(mLoadTarget);
	std::string urlString = url.str();

	size_t size = urlString.find("//");
	urlString = urlString.replace(0, size + 2, "");
	std::replace( urlString.begin(), urlString.end(), '/', '\\');

	auto filePath = std::filesystem::temp_directory_path();
	filePath.append("GW2-arcdps-extension");

	// create needed subdirs
	filePath.append(urlString);

	if (exists(filePath)) {
		mLoadTarget = filePath;
		LoadFromFile();
		return;
	}

	auto dirPath = filePath;
	dirPath.remove_filename();

	std::error_code err;
	std::filesystem::create_directories(dirPath, err);
	if (err) {
		Status = Status::Error;
		return;
	}

	std::ofstream output(filePath, std::ios::binary);

	cpr::Response response = cpr::Download(output, url);
	if (response.status_code != 200) {
		Status = Status::Error;
		return;
	}

	output.close();
	if (output.fail()) {
		Status = Status::Error;
		return;
	}

	mLoadTarget = filePath;
	LoadFromFile();
}

#endif
