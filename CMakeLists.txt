# Windows 移植版 CMake（linux-wallpaperengine → PCL2 原生场景渲染 DLL）
# 用法: cmake -S . -B build-win -G "Visual Studio 17 2022" -A x64
cmake_minimum_required(VERSION 3.20)
project(wallpaperengine-win CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE "LEGACY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /EHsc /W0 /MP")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
set(CMAKE_C_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")

find_package(OpenGL REQUIRED)

# ===== 本地预编译/源码依赖 =====
set(TP "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

# zlib（源码）
set(ZLIB_DIR "${TP}/zlib/zlib-1.3.1")
file(GLOB ZLIB_SOURCES "${ZLIB_DIR}/*.c")
list(FILTER ZLIB_SOURCES EXCLUDE REGEX ".*/(example|minigzip|gzclose|gzlib|gzread|gzwrite|compress|uncompr|test|infback).*")
add_library(zlib-local STATIC ${ZLIB_SOURCES})
target_include_directories(zlib-local PUBLIC ${ZLIB_DIR})

# lz4（源码）
set(LZ4_DIR "${TP}/lz4/lz4-1.10.0")
add_library(lz4-local STATIC "${LZ4_DIR}/lib/lz4.c" "${LZ4_DIR}/lib/lz4hc.c")
target_include_directories(lz4-local PUBLIC "${LZ4_DIR}/lib")

# GLFW 3.4 预编译
set(GLFW_DIR "${TP}/glfw/glfw-3.4.bin.WIN64")
add_library(glfw-local STATIC IMPORTED)
set_target_properties(glfw-local PROPERTIES IMPORTED_LOCATION "${GLFW_DIR}/lib-vc2022/glfw3.lib")
target_include_directories(glfw-local INTERFACE "${GLFW_DIR}/include")
# GLEW 2.2.0 预编译 x64
set(GLEW_DIR "${TP}/glew/glew-2.2.0")
add_library(glew-local STATIC IMPORTED)
set_target_properties(glew-local PROPERTIES IMPORTED_LOCATION "${GLEW_DIR}/lib/Release/x64/glew32.lib")
target_include_directories(glew-local INTERFACE "${GLEW_DIR}/include")
# SDL2 2.30.9 预编译 x64
set(SDL2_DIR "${TP}/sdl2/SDL2-2.30.9")
add_library(sdl2-local STATIC IMPORTED)
set_target_properties(sdl2-local PROPERTIES IMPORTED_LOCATION "${SDL2_DIR}/lib/x64/SDL2.lib")
target_include_directories(sdl2-local INTERFACE "${SDL2_DIR}/include")

# ===== 第三方源码子模块 =====
add_subdirectory(src/External/glslang-WallpaperEngine glslang)
add_subdirectory(src/External/SPIRV-Cross-WallpaperEngine spirv-cross)
set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)
set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)
set(KISSFFT_PKGCONFIG OFF CACHE BOOL "" FORCE)
add_subdirectory(src/External/kissfft kissfft)
add_subdirectory(src/External/argparse argparse)
set(QJS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_BIGNUM OFF CACHE BOOL "" FORCE)
add_subdirectory(src/External/quickjs quickjs)

# ===== 生成 Builtins.generated.h（场景脚本内置 JS） =====
set(WPENGINE_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(WPENGINE_BUILTINS_JS "${CMAKE_CURRENT_SOURCE_DIR}/src/WallpaperEngine/Scripting/resources/builtins.js")
file(READ "${WPENGINE_BUILTINS_JS}" WPENGINE_SCRIPT_BUILTINS)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WallpaperEngine/Scripting/resources/Builtins.generated.h.in"
    "${WPENGINE_GENERATED_DIR}/WallpaperEngine/Scripting/Builtins.generated.h"
    @ONLY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${WPENGINE_BUILTINS_JS}")
# FreeType 预编译
set(FREETYPE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/freetype/freetype-windows-binaries-master")
add_library(freetype-local STATIC IMPORTED)
set_target_properties(freetype-local PROPERTIES IMPORTED_LOCATION "${FREETYPE_DIR}/release dll/x64/freetype.lib")
target_include_directories(freetype-local INTERFACE "${FREETYPE_DIR}/include")

set(WIN_SOURCES
    src/External/MimeTypes/MimeTypes.cpp
    src/External/MimeTypes/MimeTypes.h
    src/Steam/FileSystem/FileSystem.cpp
    src/Steam/FileSystem/FileSystem.h
    src/WallpaperEngine/Logging/Log.cpp
    src/WallpaperEngine/Logging/Log.h
    src/WallpaperEngine/Debugging/CallStack.cpp
    src/WallpaperEngine/Debugging/CallStack.h
    src/WallpaperEngine/Application/ApplicationContext.cpp
    src/WallpaperEngine/Application/ApplicationContext.h
    src/WallpaperEngine/Application/WallpaperApplication.cpp
    src/WallpaperEngine/Application/WallpaperApplication.h
    src/WallpaperEngine/Assets/AssetLoadException.cpp
    src/WallpaperEngine/Assets/AssetLoadException.h
    src/WallpaperEngine/Assets/AssetLocator.cpp
    src/WallpaperEngine/Assets/AssetLocator.h
    src/WallpaperEngine/FileSystem/Container.h
    src/WallpaperEngine/FileSystem/Container.cpp
    src/WallpaperEngine/FileSystem/Adapters/Types.h
    src/WallpaperEngine/FileSystem/Adapters/Directory.cpp
    src/WallpaperEngine/FileSystem/Adapters/Directory.h
    src/WallpaperEngine/FileSystem/Adapters/Package.cpp
    src/WallpaperEngine/FileSystem/Adapters/Package.h
    src/WallpaperEngine/FileSystem/Adapters/Virtual.cpp
    src/WallpaperEngine/FileSystem/Adapters/Virtual.h
    src/WallpaperEngine/FileSystem/Adapters/MediaCover.h
    src/WallpaperEngine/FileSystem/Adapters/MediaCover.cpp
    src/WallpaperEngine/Audio/Drivers/Recorders/PlaybackRecorder.cpp
    src/WallpaperEngine/Audio/Drivers/Recorders/PlaybackRecorder.h
    src/WallpaperEngine/Audio/Drivers/Detectors/AudioPlayingDetector.cpp
    src/WallpaperEngine/Audio/Drivers/Detectors/AudioPlayingDetector.h
    src/WallpaperEngine/Audio/Drivers/AudioDriver.cpp
    src/WallpaperEngine/Audio/Drivers/AudioDriver.h
    src/WallpaperEngine/Input/InputContext.cpp
    src/WallpaperEngine/Input/InputContext.h
    src/WallpaperEngine/Input/MouseInput.h
    src/WallpaperEngine/Input/Drivers/GLFWMouseInput.cpp
    src/WallpaperEngine/Input/Drivers/GLFWMouseInput.h
    src/WallpaperEngine/Media/MediaSource.h
    src/WallpaperEngine/Media/MediaSource.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariable.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariable.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableFloat.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableFloat.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableInteger.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableInteger.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector2.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector2.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector3.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector3.cpp
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector4.h
    src/WallpaperEngine/Render/Shaders/Variables/ShaderVariableVector4.cpp
    src/WallpaperEngine/Render/Shaders/Shader.h
    src/WallpaperEngine/Render/Shaders/Shader.cpp
    src/WallpaperEngine/Render/Shaders/ShaderUnit.cpp
    src/WallpaperEngine/Render/Shaders/ShaderUnit.h
    src/WallpaperEngine/Render/Shaders/GLSLContext.cpp
    src/WallpaperEngine/Render/Shaders/GLSLContext.h
    src/WallpaperEngine/Render/Helpers/ContextAware.cpp
    src/WallpaperEngine/Render/Helpers/ContextAware.h
    src/WallpaperEngine/Render/Drivers/VideoFactories.cpp
    src/WallpaperEngine/Render/Drivers/VideoFactories.h
    src/WallpaperEngine/Render/Drivers/Detectors/FullScreenDetector.cpp
    src/WallpaperEngine/Render/Drivers/Detectors/FullScreenDetector.h
    src/WallpaperEngine/Render/Drivers/Output/Output.cpp
    src/WallpaperEngine/Render/Drivers/Output/Output.h
    src/WallpaperEngine/Render/Drivers/Output/GLFWWindowOutput.cpp
    src/WallpaperEngine/Render/Drivers/Output/GLFWWindowOutput.h
    src/WallpaperEngine/Render/Drivers/Output/GLFWOutputViewport.cpp
    src/WallpaperEngine/Render/Drivers/Output/GLFWOutputViewport.h
    src/WallpaperEngine/Render/Drivers/Output/OutputViewport.cpp
    src/WallpaperEngine/Render/Drivers/Output/OutputViewport.h
    src/WallpaperEngine/Render/Drivers/GLFWOpenGLDriver.h
    src/WallpaperEngine/Render/Drivers/GLFWOpenGLDriver.cpp
    src/WallpaperEngine/Render/Drivers/VideoDriver.h
    src/WallpaperEngine/Render/Drivers/VideoDriver.cpp
    src/WallpaperEngine/Render/RenderContext.h
    src/WallpaperEngine/Render/RenderContext.cpp
    src/WallpaperEngine/Render/TextureCache.h
    src/WallpaperEngine/Render/TextureCache.cpp
    src/WallpaperEngine/Render/FBOProvider.cpp
    src/WallpaperEngine/Render/FBOProvider.h
    src/WallpaperEngine/Render/Helpers/ContextAware.cpp
    src/WallpaperEngine/Render/Helpers/ContextAware.h
    src/WallpaperEngine/Render/CWallpaper.h
    src/WallpaperEngine/Render/CWallpaper.cpp
    src/WallpaperEngine/Render/WallpaperState.h
    src/WallpaperEngine/Render/WallpaperState.cpp
    src/WallpaperEngine/Render/Wallpapers/CScene.h
    src/WallpaperEngine/Render/Wallpapers/CScene.cpp
    src/WallpaperEngine/Render/Camera.h
    src/WallpaperEngine/Render/Camera.cpp
    src/WallpaperEngine/Render/CObject.h
    src/WallpaperEngine/Render/CObject.cpp
    src/WallpaperEngine/Render/CTexture.cpp
    src/WallpaperEngine/Render/CTexture.h
    src/WallpaperEngine/Render/AlbumTexture.h
    src/WallpaperEngine/Render/AlbumTexture.cpp
    src/WallpaperEngine/Render/TextureProvider.h
    src/WallpaperEngine/Render/Objects/CRenderable.cpp
    src/WallpaperEngine/Render/Objects/CRenderable.h
    src/WallpaperEngine/Render/Objects/CImage.h
    src/WallpaperEngine/Render/Objects/CImage.cpp
    src/WallpaperEngine/Render/Objects/CParticle.h
    src/WallpaperEngine/Render/Objects/CParticle.cpp
    src/WallpaperEngine/Render/Objects/CText.h
    src/WallpaperEngine/Render/Objects/CText.cpp
    src/WallpaperEngine/Render/CFBO.h
    src/WallpaperEngine/Render/CFBO.cpp
    src/WallpaperEngine/Render/Objects/Effects/CPass.h
    src/WallpaperEngine/Render/Objects/Effects/CPass.cpp
    src/WallpaperEngine/Data/Assets/Types.h
    src/WallpaperEngine/Data/Assets/Texture.h
    src/WallpaperEngine/Data/Assets/Package.h
    src/WallpaperEngine/Data/Model/Types.h
    src/WallpaperEngine/Data/Model/Project.h
    src/WallpaperEngine/Data/Model/Wallpaper.h
    src/WallpaperEngine/Data/Model/Object.h
    src/WallpaperEngine/Data/Model/Material.h
    src/WallpaperEngine/Data/Model/Effect.h
    src/WallpaperEngine/Data/Model/Model.h
    src/WallpaperEngine/Data/Model/UserSetting.h
    src/WallpaperEngine/Data/Model/DynamicValue.h
    src/WallpaperEngine/Data/Model/DynamicValue.cpp
    src/WallpaperEngine/Data/Model/Property.h
    src/WallpaperEngine/Data/Utils/TypeCaster.h
    src/WallpaperEngine/Data/Utils/BinaryReader.cpp
    src/WallpaperEngine/Data/Utils/BinaryReader.h
    src/WallpaperEngine/Data/Utils/MemoryStream.h
    src/WallpaperEngine/Data/Utils/SFINAE.h
    src/WallpaperEngine/Data/Parsers/DynamicValueParser.h
    src/WallpaperEngine/Data/Parsers/DynamicValueParser.cpp
    src/WallpaperEngine/Data/Parsers/EffectParser.cpp
    src/WallpaperEngine/Data/Parsers/EffectParser.h
    src/WallpaperEngine/Data/Parsers/ProjectParser.cpp
    src/WallpaperEngine/Data/Parsers/ProjectParser.h
    src/WallpaperEngine/Data/Parsers/WallpaperParser.cpp
    src/WallpaperEngine/Data/Parsers/WallpaperParser.h
    src/WallpaperEngine/Data/Parsers/UserSettingParser.cpp
    src/WallpaperEngine/Data/Parsers/UserSettingParser.h
    src/WallpaperEngine/Data/Parsers/ObjectParser.cpp
    src/WallpaperEngine/Data/Parsers/ObjectParser.h
    src/WallpaperEngine/Data/Parsers/MaterialParser.cpp
    src/WallpaperEngine/Data/Parsers/MaterialParser.h
    src/WallpaperEngine/Data/Parsers/ModelParser.cpp
    src/WallpaperEngine/Data/Parsers/ModelParser.h
    src/WallpaperEngine/Data/Parsers/ShaderConstantParser.cpp
    src/WallpaperEngine/Data/Parsers/ShaderConstantParser.h
    src/WallpaperEngine/Data/Parsers/PackageParser.cpp
    src/WallpaperEngine/Data/Parsers/PackageParser.h
    src/WallpaperEngine/Data/Parsers/PropertyParser.cpp
    src/WallpaperEngine/Data/Parsers/PropertyParser.h
    src/WallpaperEngine/Data/Parsers/TextureParser.cpp
    src/WallpaperEngine/Data/Parsers/TextureParser.h
    src/WallpaperEngine/Data/Builders/ColorBuilder.h
    src/WallpaperEngine/Data/Builders/ColorBuilder.cpp
    src/WallpaperEngine/Data/Builders/UserSettingBuilder.h
    src/WallpaperEngine/Data/Builders/VectorBuilder.cpp
    src/WallpaperEngine/Data/Builders/VectorBuilder.h
    src/WallpaperEngine/Data/Dumpers/StringPrinter.cpp
    src/WallpaperEngine/Data/Dumpers/StringPrinter.h
    src/WallpaperEngine/Data/JSON.cpp
    src/WallpaperEngine/Scripting/Adapters/ObjectAdapter.h
    src/WallpaperEngine/Scripting/Adapters/ObjectAdapter.cpp
    src/WallpaperEngine/Scripting/Adapters/VectorAdapter.h
    src/WallpaperEngine/Scripting/Adapters/VectorAdapter.cpp
    src/WallpaperEngine/Scripting/Adapters/ScriptableObjectAdapter.h
    src/WallpaperEngine/Scripting/Adapters/ScriptableObjectAdapter.cpp
    src/WallpaperEngine/Scripting/Modules/ColorModule.h
    src/WallpaperEngine/Scripting/Modules/ColorModule.cpp
    src/WallpaperEngine/Scripting/Modules/MathModule.h
    src/WallpaperEngine/Scripting/Modules/MathModule.cpp
    src/WallpaperEngine/Scripting/Modules/ScriptModule.h
    src/WallpaperEngine/Scripting/Modules/ScriptModule.cpp
    src/WallpaperEngine/Scripting/ConsoleObject.h
    src/WallpaperEngine/Scripting/ConsoleObject.cpp
    src/WallpaperEngine/Scripting/EngineObject.h
    src/WallpaperEngine/Scripting/EngineObject.cpp
    src/WallpaperEngine/Scripting/InputObject.h
    src/WallpaperEngine/Scripting/InputObject.cpp
    src/WallpaperEngine/Scripting/SceneObject.h
    src/WallpaperEngine/Scripting/SceneObject.cpp
    src/WallpaperEngine/Scripting/ScriptEngine.h
    src/WallpaperEngine/Scripting/ScriptPropertiesObject.h
    src/WallpaperEngine/Scripting/ScriptPropertiesObject.cpp
    src/WallpaperEngine/Scripting/ScriptEngine.cpp
    src/WallpaperEngine/Scripting/ScriptableObject.h
    src/WallpaperEngine/Scripting/ScriptableObject.cpp
    src/WallpaperEngine/Maths.h
    src/WallpaperEngine/Maths.cpp
    win-stub/stubs.cpp
    src/wpe_api.cpp
    wpe-core.rc
)

add_library(wpe-core SHARED ${WIN_SOURCES})



set_target_properties(wpe-core PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(wpe-core PUBLIC
    ${WPENGINE_GENERATED_DIR}
    win-stub
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glm_full/glm-1.0.1"
    "${FREETYPE_DIR}/include"
    src
    src/External/glslang-WallpaperEngine
    src/External/SPIRV-Cross-WallpaperEngine
    src/External/MimeTypes
    src/External/json/include
    src/External/stb
)
target_link_libraries(wpe-core PRIVATE
    OpenGL::GL
    glew-local
    glfw-local
    zlib-local
    lz4-local
    sdl2-local
    freetype-local
    kissfft
    glslang
    spirv-cross-core
    spirv-cross-glsl
    argparse
    qjs
)

add_executable(wallpaperengine-win src/main.cpp)
set_target_properties(wallpaperengine-win PROPERTIES LINK_FLAGS "/STACK:16777216")
target_link_libraries(wallpaperengine-win PRIVATE wpe-core)
target_include_directories(wallpaperengine-win PRIVATE
    win-stub
    src
    src/External/glslang-WallpaperEngine
    src/External/SPIRV-Cross-WallpaperEngine
    src/External/MimeTypes
    src/External/json/include
    src/External/stb
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glew/glew-2.2.0/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw/glfw-3.4.bin.WIN64/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sdl2/SDL2-2.30.9/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glm_full/glm-1.0.1"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/freetype/freetype-windows-binaries-master/include"
)
