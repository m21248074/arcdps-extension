#pragma once

#include <atomic>
#include <Windows.h>
#include <d3d9.h>
#include <map>
#include <mutex>
#include <vector>

class IconLoader;

class Icon {
	friend IconLoader;
public:
	Icon(UINT name, HMODULE dll, IDirect3DDevice9* d3d9Device);
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
	IDirect3DTexture9* texture;
};

class IconLoader {
public:
	void Setup(HMODULE new_dll, IDirect3DDevice9* new_d3d9device);
	IDirect3DTexture9* getTexture(UINT name);

private:
	HMODULE dll = nullptr;
	IDirect3DDevice9* d3d9device = nullptr;
	std::map<UINT, Icon> textures;
	std::vector<UINT> queue;
	std::mutex queueMutex;

	std::atomic_bool thread_running = false;

	void thread_fun();
};

extern IconLoader iconLoader;
