# dxpy
DirectX Python Runtime, easy for test DirectX kernel in WSL. 

### Levels of launching kernel:  
[HLSL code]: HLSL code.  
[dxpy + numpy]: DirectX Python Runtime.  
[DXIL + libdxcompiler.so]: DirectX Shader Compiler(with NNFusion Patch) + DXIL.  
[/lib/wsl/libdx*.so]: DirectX Runtime.  
[/dev/dxg]: Kernel level device driver.


### Related Projects
In Linux, original DirectX Shader Compiler disables reflection blob and has different IUnknown interface. So we have a patched repo in here: https://github.com/nnfusion/DirectXShaderCompiler
