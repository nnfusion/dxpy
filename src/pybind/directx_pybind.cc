#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../runtime/directx_common.h"
#include "../runtime/directx_header.h"

namespace py = pybind11;
using namespace std;
using namespace py;
using namespace tvm::runtime::dx;

#define log std::cout<<"0000-00-00 00:00:00,000 - bind - INFO - "

int add(int i, int j) {
    return i + j;
}

DirectXContext dx_ctx;
shared_ptr<DirectXDevice> dx_dev;
vector<shared_ptr<DirectComputeKernel>> kernel_vec;
vector<shared_ptr<DirectBuffer>> buffer_vec;

void init() {
    dx_dev = dx_ctx.get_device(0);
    log << "Enabled Device:\t" << dx_dev->debug_str()<<std::endl;
}

int add_kernel(string src) {
    auto kernel = dx_dev->gen_computer_shader_from_string(src);
    kernel_vec.push_back(kernel);
    log << "Added kernel length:\t" << src.length() <<std::endl;
    return kernel_vec.size() - 1;
}

int create_host_buffer(size_t bytes, size_t num) {
    auto t = dx_dev->host_buffer(bytes * num, {kDLFloat, bytes * 8, bytes});
    buffer_vec.push_back(t);
    return buffer_vec.size() - 1;
}

void copy_host_buffer_to(uint64_t ptr, size_t total_bytes, size_t id) {
    void* p = (void*)ptr;
    auto buf = static_pointer_cast<DirectHostBuffer>(buffer_vec[id]);
    void* src = buf->open_data_ptr();
    memcpy(p, src, total_bytes);
    buf->close_data_ptr();
}

void copy_to_host_buffer(size_t id, size_t total_bytes, uint64_t ptr) {
    void* p = (void*)ptr;
    auto buf = static_pointer_cast<DirectHostBuffer>(buffer_vec[id]);
    void* tgt = buf->open_data_ptr();
    memcpy(tgt , p, total_bytes);
    buf->close_data_ptr();
}

int create_device_buffer(size_t bytes, size_t num) {
    auto t = dx_dev->device_buffer(bytes * num, {kDLFloat, bytes * 8, bytes});
    buffer_vec.push_back(t);
    return buffer_vec.size() - 1;
}

void copy(size_t to, size_t from) {
    dx_dev->buffer_copy(buffer_vec[to], buffer_vec[from], false);
}

void release_buffer(size_t handle) {
    buffer_vec[handle] = nullptr;
}

void dispatch(size_t kernel_handle, const vector<int>& buffers, string kernel_name, const vector<int>& launch_grid) {
    auto kernel_shader = kernel_vec[kernel_handle];
    vector<shared_ptr<DirectBuffer>> buf_vec;
    for(auto i: buffers)
    {
        auto buf = buffer_vec[i];
        buf_vec.push_back(buf);
    }
    dx_dev->dispatch(kernel_shader, buf_vec, kernel_name, {launch_grid[0], launch_grid[1], launch_grid[2],}, {launch_grid[3], launch_grid[4], launch_grid[5],});
}

PYBIND11_MODULE(pybind, dx) {
    dx.def("init", []() {
        init();
    });
    dx.def("add_kernel", [](string src) {
        return add_kernel(src);
    });
    dx.def("create_host_buffer", [](size_t bytes, size_t num){
        auto i = create_host_buffer(bytes, num);
        return i;
    });
    dx.def("create_device_buffer", [](size_t bytes, size_t num){
        auto i = create_device_buffer(bytes, num);
        return i;
    });
    dx.def("copy", [](size_t to, size_t from){
        copy(to, from);
    });
    dx.def("release_buffer", [](size_t id){
        release_buffer(id);
    });
    dx.def("copy_host_buffer_to", [](size_t ptr, size_t total_bytes, size_t id){
        return copy_host_buffer_to(ptr, total_bytes, id);
    });
    dx.def("copy_to_host_buffer", [](size_t id, size_t total_bytes, size_t ptr){
        return copy_to_host_buffer(id, total_bytes, ptr);
    });
    dx.def("dispatch", [](size_t kernel_handle, const vector<int>& buffers, string kernel_name, const vector<int>& launch_grid){
        dispatch(kernel_handle, buffers, kernel_name, launch_grid);
    });
}