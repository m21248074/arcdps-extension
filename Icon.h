#pragma once

#include <Windows.h>
#include <d3d9.h>

class Icon {
public:
	IDirect3DTexture9* texture;
	Icon() = delete;
	Icon(UINT name, HMODULE dll, IDirect3DDevice9* d3d9Device);
	~Icon();

	// delete copy/move
	Icon(const Icon& other) = delete;
	Icon(Icon&& other) noexcept = delete;
	Icon& operator=(const Icon& other) = delete;
	Icon& operator=(Icon&& other) noexcept = delete;
	
private:
	UINT width;
	UINT height;
};
