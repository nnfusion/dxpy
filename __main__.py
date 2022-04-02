#!python
from __build__ import prepare_env
prepare_env()

# import deps
import os, sys, json
import argparse
import torch
import numpy as np
import pybind
import logging
import importlib.machinery

logging.basicConfig(
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO
)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='General DirectX Runtime for Testing Contributed Kernel.')
    parser.add_argument('--operator-name', metavar='string',
                        type=str, help='Operator name')
    parser.add_argument('--nnfusion-home', metavar='string',
                        type=str, help='Home folder of NNFusion contibuted folder.')
    args = parser.parse_args()
    if args.nnfusion_home is None:
        args.nnfusion_home = os.environ["NNFUSION_HOME"]
    logging.info("Try to test operator: {0} at folder {1}.".format(args.operator_name, args.nnfusion_home))

    op_file = os.path.join(args.nnfusion_home, "custom_op", "script", "__operator__.py")
    loader = importlib.machinery.SourceFileLoader("__operator__", op_file)
    handle = loader.load_module("__operator__")
    test_cases = handle.get_operator_tests(args.operator_name)
    no_error_flag = True
    pybind.init()
    for test_case in test_cases:
        if "kernel" not in test_case:
            continue
        logging.info("Test case:{0}, Kernel code len:{1}, input:{2}, output:{3}".format(test_case["test"], len(test_case["kernel"]["hlsl_kernel"]), test_case["kernel"]["input"], test_case["kernel"]["output"]))
        k = pybind.add_kernel(test_case["kernel"]["hlsl_kernel"])
        host_alloc_list = []
        dev_alloc_list = []
        output_host_alloc_list = []
        output_dev_alloc_list = []
        launch_config = test_case["kernel"]["launch_config"][0] + test_case["kernel"]["launch_config"][1]

        for array_ in test_case["input"]:
            bytes_ = array_.itemsize
            nums_ = array_.size
            host_ = pybind.create_host_buffer(bytes_, nums_)
            pybind.copy_to_host_buffer(host_, bytes_ * nums_, array_.ctypes.data)
            dev_ = pybind.create_device_buffer(bytes_, nums_)
            pybind.copy(dev_, host_)
            host_alloc_list.append(host_)
            dev_alloc_list.append(dev_)

        for array_ in test_case["output"]:
            bytes_ = array_.itemsize
            nums_ = array_.size
            dev_ = pybind.create_device_buffer(bytes_, nums_)
            host_ = pybind.create_host_buffer(bytes_, nums_)
            host_alloc_list.append(host_)
            dev_alloc_list.append(dev_)
            output_host_alloc_list.append(host_)
            output_dev_alloc_list.append(dev_)

        pybind.dispatch(k, dev_alloc_list, test_case["kernel"]["entry_point"], launch_config)

        output_array = []
        for i in range(0, len(test_case["output"])):
            array_ = test_case["output"][i]
            bytes_ = array_.itemsize
            nums_ = array_.size
            t = np.zeros(array_.shape, dtype=array_.dtype)
            host_ = output_host_alloc_list[i]
            dev_ = output_dev_alloc_list[i]
            pybind.copy(host_, dev_)
            pybind.copy_host_buffer_to(t.ctypes.data, bytes_ * nums_, host_)
            output_array.append(t)

        no_error_flag = test_case["allclose"](test_case["output"], output_array)

        for id in host_alloc_list:
            pybind.release_buffer(id)
        for id in dev_alloc_list:
            pybind.release_buffer(id)

        if no_error_flag:
            logging.info("No error for test case: {0}".format(test_case["test"]))
    
    logging.info("Press CTRL+C to exit if program hang.")