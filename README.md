# dxpy : Native WSL DirectX Python Runtime
dxpy is a simple runtime to test your DirectX kernel in WSL.    
```shell
git clone --recursive https://github.com/wenxcs/dxpy.git
pip install -r dxpy/requirements.txt
git clone git@github.com:microsoft/nnfusion.git
python dxpy --operator-name=TopK --nnfusion-home=nnfusion/src/contrib
```
## Source code comment
src/runtime : Native DirectX Runtime to 1. Compile HLSL into DXIL kernel; 2. Launch DXIL kernel through /dev/dxg;   
src/pybind : Source code for binding above runtime with Python module;   
\_\_main\_\_.py : 1. Compile above native code if necessary; 2. Invoke script in NNFUSION_HOME/src/contrib/script to get operator's test cases and kernels; 3. Check kernel correctness;   

## Requirement
1. WSL and DirectX user mode driver: [Link](https://devblogs.microsoft.com/directx/directx-heart-linux/)
2. Python3 enviroment
3. CMake and C++ compiler

## Related Projects
1. DirectX Shader Compiler   
In Linux, original DirectX Shader Compiler (1)disables reflection blob and (2)has different IUnknown interface. So we have a patched repo in here: https://github.com/nnfusion/DirectXShaderCompiler
2. DirectX-Headers   
Original d3d12shader.h has some issue in WSL but it works in Windows.
