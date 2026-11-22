// Wallpaper Engine 原生渲染器 C 接口（供 PCL2 P/Invoke 调用）
#include <windows.h>
#include <GL/glew.h>
#include <cstring>
#include <string>
#include <vector>

#include "WallpaperEngine/Application/ApplicationContext.h"
#include "WallpaperEngine/Application/WallpaperApplication.h"
#include "WallpaperEngine/Render/Drivers/VideoFactories.h"

void registerGLFWOpenGLDriver ();

using namespace WallpaperEngine::Application;

struct WpeHandle {
    ApplicationContext* context = nullptr;
    WallpaperApplication* app = nullptr;
    GLuint fbo = 0;
    GLuint texture = 0;
    GLuint readFbo = 0;
    GLuint readTexture = 0;
    int width = 0;
    int height = 0;
    std::vector<unsigned char> frame;   // RGBA，顶部优先（已翻转）
    std::vector<unsigned char> rawFrame; // GL 原始（底部优先）
    bool ready = false;
    void* parentHwnd = nullptr;
    int winX = 0, winY = 0;
};

extern "C" {

__declspec(dllexport) void* wpe_create (const char* projectPath, int width, int height) {
    if (projectPath == nullptr || width <= 0 || height <= 0) {
	return nullptr;
    }

    auto* handle = new WpeHandle ();
    handle->width = width;
    handle->height = height;
    handle->frame.resize ((size_t) width * height * 4);
    handle->rawFrame.resize ((size_t) width * height * 4);

    try {
	// 构造命令行参数
	std::string geometry = "0x0x" + std::to_string (width) + "x" + std::to_string (height);
	std::string project = projectPath;
	char* argv[] = {
	    (char*) "wpe",
	    (char*) "-w",
	    (char*) geometry.c_str (),
	    (char*) project.c_str (),
	    nullptr
	};
	int argc = 4;

	handle->context = new ApplicationContext (argc, argv);
	handle->context->loadSettingsFromArgv ();
	// 引擎内部默认限帧 30，叠加外层循环会导致实际只有十几帧；统一提到 60
	handle->context->settings.render.maximumFPS = 60;

	// 注册 GLFW 驱动（MSVC 无 constructor 属性）
	registerGLFWOpenGLDriver ();

	handle->app = new WallpaperApplication (*handle->context);
	handle->app->setup ();

	// 创建目标 FBO（渲染到离屏纹理）
	glGenTextures (1, &handle->texture);
	glBindTexture (GL_TEXTURE_2D, handle->texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture (GL_TEXTURE_2D, 0);

	glGenFramebuffers (1, &handle->fbo);
	glBindFramebuffer (GL_FRAMEBUFFER, handle->fbo);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle->texture, 0);
	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	    glBindFramebuffer (GL_FRAMEBUFFER, 0);
	    throw std::runtime_error ("FBO not complete");
	}
	glBindFramebuffer (GL_FRAMEBUFFER, 0);

	// 读回 FBO（窗口尺寸，用于 GPU 缩放读取）
	glGenTextures (1, &handle->readTexture);
	glBindTexture (GL_TEXTURE_2D, handle->readTexture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture (GL_TEXTURE_2D, 0);
	glGenFramebuffers (1, &handle->readFbo);
	glBindFramebuffer (GL_FRAMEBUFFER, handle->readFbo);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle->readTexture, 0);
	glBindFramebuffer (GL_FRAMEBUFFER, 0);

	handle->app->setDestinationFramebuffer (handle->fbo);
	handle->ready = true;
	return handle;
    } catch (const std::exception& e) {
	printf ("[wpe] create failed: %s\n", e.what ()); fflush (stdout);
	if (handle->app != nullptr) { delete handle->app; handle->app = nullptr; }
	if (handle->context != nullptr) { delete handle->context; handle->context = nullptr; }
	delete handle;
	return nullptr;
    }
}

__declspec(dllexport) int wpe_render (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr || !handle->ready) {
	return -1;
    }

    try {
	handle->app->render ();

	// 从 wallpaper 内部 framebuffer 读取（与 exe takeScreenshot 路径一致）
	GLuint srcFbo = handle->app->getWallpaperFramebufferFirst ();
	int fboW = handle->app->getWallpaperWidthFirst ();
	int fboH = handle->app->getWallpaperHeightFirst ();
	if (fboW <= 0 || fboH <= 0) {
	    fboW = handle->width;
	    fboH = handle->height;
	}

	// GPU 缩放到窗口尺寸（避免每帧读 1920x1200 全尺寸导致 UI 线程压力过大）
	if (srcFbo != 0 && handle->readFbo != 0) {
	    glBindFramebuffer (GL_READ_FRAMEBUFFER, srcFbo);
	    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, handle->readFbo);
	    glBlitFramebuffer (0, 0, fboW, fboH, 0, 0, handle->width, handle->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	    glBindFramebuffer (GL_FRAMEBUFFER, handle->readFbo);
	} else if (srcFbo != 0) {
	    glBindFramebuffer (GL_FRAMEBUFFER, srcFbo);
	} else {
	    glBindFramebuffer (GL_FRAMEBUFFER, 0);
	}
	glPixelStorei (GL_PACK_ALIGNMENT, 1);
	glReadPixels (0, 0, handle->width, handle->height, GL_BGRA, GL_UNSIGNED_BYTE, handle->rawFrame.data ());
	glBindFramebuffer (GL_FRAMEBUFFER, 0);

	// 上下翻转（GL 底部优先 → PNG/UI 顶部优先）
	const int stride = handle->width * 4;
	for (int y = 0; y < handle->height; y++) {
	    std::memcpy (
		handle->frame.data () + (size_t) y * stride,
		handle->rawFrame.data () + (size_t) (handle->height - 1 - y) * stride, stride
	    );
	}
	return 1;
    } catch (...) {
	return -2;
    }
}

__declspec(dllexport) int wpe_copy_frame (void* handlePtr, unsigned char* out, int outSize) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr || !handle->ready || out == nullptr) {
	return -1;
    }
    const int needed = handle->width * handle->height * 4;
    if (outSize < needed) {
	return -2;
    }
    std::memcpy (out, handle->frame.data (), (size_t) needed);
    return needed;
}

// ===== 窗口直出模式（GPU 零拷贝：渲染到可见窗口，PCL 半透明透出） =====
__declspec(dllexport) void* wpe_create_windowed (const char* projectPath, void* parentHwnd, int x, int y, int width, int height) {
    if (projectPath == nullptr || width <= 0 || height <= 0) {
	return nullptr;
    }
    auto* handle = new WpeHandle ();
    handle->width = width;
    handle->height = height;
    handle->parentHwnd = parentHwnd;
    handle->winX = x;
    handle->winY = y;
    try {
	std::string geometry = "0x0x" + std::to_string (width) + "x" + std::to_string (height);
	std::string project = projectPath;
	char* argv[] = { (char*) "wpe", (char*) "-w", (char*) geometry.c_str (), (char*) project.c_str (), nullptr };
	int argc = 4;
	handle->context = new ApplicationContext (argc, argv);
	handle->context->loadSettingsFromArgv ();
	handle->context->settings.render.maximumFPS = 60;
	registerGLFWOpenGLDriver ();
	handle->app = new WallpaperApplication (*handle->context);
	handle->app->setup ();
	handle->app->setDestinationFramebuffer (0);
	// 显示渲染窗口 + 定位到 PCL 下方
	handle->app->setRenderWindowVisible (true);
	handle->app->moveRenderWindow (x, y, width, height);
	if (parentHwnd != nullptr) {
	    handle->app->setRenderWindowParent (parentHwnd);
	}
	handle->ready = true;
	return handle;
    } catch (const std::exception& e) {
	printf ("[wpe] create_windowed failed: %s\n", e.what ()); fflush (stdout);
	if (handle->app != nullptr) { delete handle->app; handle->app = nullptr; }
	if (handle->context != nullptr) { delete handle->context; handle->context = nullptr; }
	delete handle;
	return nullptr;
    } catch (...) {
	delete handle;
	return nullptr;
    }
}

__declspec(dllexport) int wpe_render_windowed (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr || !handle->ready) {
	return -1;
    }
    try {
	handle->app->render ();
	return 1;
    } catch (...) {
	return -2;
    }
}

__declspec(dllexport) void wpe_move_window (void* handlePtr, int x, int y, int width, int height) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr || !handle->ready) {
	return;
    }
    try {
	handle->winX = x;
	handle->winY = y;
	handle->width = width;
	handle->height = height;
	handle->app->moveRenderWindow (x, y, width, height);
    } catch (...) {
    }
}

__declspec(dllexport) void wpe_set_window_visible (void* handlePtr, int visible) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr || !handle->ready) {
	return;
    }
    try {
	handle->app->setRenderWindowVisible (visible != 0);
    } catch (...) {
    }
}
// 获取 wallpaper 内部 framebuffer（与 exe 截图路径一致）
__declspec(dllexport) unsigned int wpe_get_wallpaper_fbo (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    return handle == nullptr || handle->app == nullptr ? 0 : handle->app->getWallpaperFramebufferFirst ();
}

__declspec(dllexport) int wpe_get_wallpaper_width (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    return handle == nullptr || handle->app == nullptr ? 0 : handle->app->getWallpaperWidthFirst ();
}

__declspec(dllexport) int wpe_get_wallpaper_height (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    return handle == nullptr || handle->app == nullptr ? 0 : handle->app->getWallpaperHeightFirst ();
}

__declspec(dllexport) int wpe_get_width (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    return handle == nullptr ? 0 : handle->width;
}

__declspec(dllexport) int wpe_get_height (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    return handle == nullptr ? 0 : handle->height;
}

__declspec(dllexport) void wpe_destroy (void* handlePtr) {
    auto* handle = static_cast<WpeHandle*> (handlePtr);
    if (handle == nullptr) {
	return;
    }
    try {
	if (handle->app != nullptr) {
	    delete handle->app;
	    handle->app = nullptr;
	}
	if (handle->context != nullptr) {
	    delete handle->context;
	    handle->context = nullptr;
	}
    } catch (...) {
    }
    delete handle;
}

} // extern "C"