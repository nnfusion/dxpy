#!python
import os

def prepare_env(force = False):
    rtdir = os.path.dirname(__file__)
    rt_so = os.path.join(rtdir, "libdxpy_runtime.so")
    if not os.path.exists(rt_so) or force:
        os.system("cmake -S {0} -B {1}".format(rtdir, os.path.join(rtdir, "build")))
        os.system("cmake --build {0}".format(os.path.join(rtdir, "build")))
        os.system("cp {0} {1}".format(os.path.join(rtdir, "build", "*.so"), rtdir))