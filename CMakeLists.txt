project(dxpy CXX)
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0007 NEW)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Looking for system wsl dx libs
list(APPEND dx_dep_libs "")
if(EXISTS "/dev/dxg")
  find_library(libd3d12 d3d12 HINTS /lib/wsl/lib)
  find_library(libd3d12core d3d12core HINTS /lib/wsl/lib)
  find_library(libdxcore dxcore HINTS /lib/wsl/lib)
  list(APPEND dx_dep_libs ${libd3d12} ${libd3d12core} ${libdxcore} ${libdxc})
endif()

# Looking for dxcompiler
set(DIRECTX_SHADER_COMPILER "thirdparty/DirectXShaderCompiler")
set(DIRECTX_SHADER_COMPILER_TAR_GZ "${CMAKE_SOURCE_DIR}/thirdparty/DirectXShaderCompiler/DirectXShaderCompiler.x64.linux.tar.gz")
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/DirectXShaderCompiler/lib/libdxcompiler.so")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvzf "${DIRECTX_SHADER_COMPILER_TAR_GZ}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/thirdparty")
endif()
find_library(libdxc dxcompiler HINTS ${DIRECTX_SHADER_COMPILER}/lib ${DIRECTX_SHADER_COMPILER}/bin)
list(APPEND dx_dep_libs ${libdxc})

set(DIRECTX_HEADERS "thirdparty/DirectX-Headers/")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/${DIRECTX_SHADER_COMPILER}/include)
include_directories(${CMAKE_SOURCE_DIR}/${DIRECTX_HEADERS}/include)
include_directories(${CMAKE_SOURCE_DIR}/${DIRECTX_HEADERS}/include/wsl/stubs)

# add pybind11
add_subdirectory(thirdparty/pybind11)

# Create dxruntime library
file(GLOB DXRUNTIME src/runtime/*.cc)
add_library(dxpy_runtime SHARED ${DXRUNTIME})
target_link_libraries(dxpy_runtime PRIVATE ${dx_dep_libs})

file(GLOB DXBIND src/pybind/*.cc)
pybind11_add_module(pybind ${DXBIND})
target_link_libraries(pybind PRIVATE ${dx_dep_libs} dxpy_runtime)