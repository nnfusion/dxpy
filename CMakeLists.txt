project(dxpy CXX)
cmake_minimum_required(VERSION 3.10)

# Looking for wsl dx libs
list(APPEND dx_dep_libs "")
if(EXISTS "/dev/dxg")
  find_library(libd3d12 d3d12 HINTS /lib/wsl/lib)
  find_library(libd3d12core d3d12core HINTS /lib/wsl/lib)
  find_library(libdxcore dxcore HINTS /lib/wsl/lib)
  list(APPEND dx_dep_libs ${libd3d12} ${libd3d12core} ${libdxcore} ${libdxc})
endif()

# Looking for dxcompiler
set(DIRECTX_SHADER_COMPILER "thirdparty/")
set(DIRECTX_SHADER_COMPILER_TAR_GZ "${CMAKE_SOURCE_DIR}/thirdparty/DirectXShaderCompiler/DirectXShaderCompiler.x64.linux.tar.gz")
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/DirectXShaderCompiler/lib/libdxcompiler.so")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvzf "${DIRECTX_SHADER_COMPILER_TAR_GZ}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${DIRECTX_SHADER_COMPILER}")
endif()

find_library(libdxc dxcompiler HINTS ${DIRECTX_SHADER_COMPILER}/lib ${DIRECTX_SHADER_COMPILER}/bin)
include_directories(${DIRECTX_SHADER_COMPILER}/include)
list(APPEND dx_dep_libs ${libdxc})

# Create dxruntime library
#file(GLOB DXRUNTIME runtime/*.cc)
#add_library(librt ${DXRUNTIME})
#target_link_libraries(librt PRIVATE Microsoft::DirectX-Headers ${dx_dep_libs})
#target_include_directories(librt PRIVATE ${DIRECTX_SHADER_COMPILER}/include)