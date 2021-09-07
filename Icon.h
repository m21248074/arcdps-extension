#pragma once

#include <atomic>
#include <Windows.h>
#include <d3d11.h>
#include <map>
#include <mutex>
#include <vector>

class IconLoader;

class Icon {
	friend IconLoader;
public:
	Icon(UINT name, HMODULE dll, ID3D11Device* d3d11Device);
	Icon() = delete;
	~Icon();

	// delete copy/move
	Icon(const Icon& other) = delete;
	Icon(Icon&& other) noexcept = delete;
	Icon& operator=(const Icon& other) = delete;
	Icon& operator=(Icon&& other) noexcept = delete;
	
private:
	UINT width;
	UINT height;
	ID3D11ShaderResourceView* texture;
};

/**
 * Call `Setup()` in `mod_init()`. This is needed, so this class knows about the dll and the directx device!
 * If this call is missing not Icons can be found, which will cause a runtime exception
 */
class IconLoader {
public:
	void Setup(HMODULE new_dll, ID3D11Device* new_d3d11device);
	ID3D11ShaderResourceView* getTexture(UINT name);

private:
	HMODULE dll = nullptr;
	ID3D11Device* d3d11device = nullptr;
	std::map<UINT, Icon> textures;
	std::vector<UINT> queue;
	std::mutex queueMutex;

	std::atomic_bool thread_running = false;

	void thread_fun();
};

extern IconLoader iconLoader;
