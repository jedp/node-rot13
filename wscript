import Options

import os
import os.path as path

srcdir = "."
blddir = "build"
VERSION = "0.1.0"

def set_options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.check_tool("node_addon")

def build(bld):
    obj = bld.new_task_gen("cxx", "shlib", "node_addon")
    obj.cxxflags = ['-g', 
                    '-D_FILE_OFFSET_BITS=64',
                    '-D_LARGEFILE_SOURCE', 
                    '-Wall']
    obj.target = "rot13"
    obj.source = "src/rot13.cpp"

def shutdown():
    if Options.commands['clean']:
        if fs.exists('rot13.node'):
            os.unlink('rot13.node')
        if fs.exists('build/default/rot13.node'):
            os.unlink('build/default/rot13.node')
    
    if fs.exists('build/default/rot13.node') and not fs.exists('rot13.node'):
        os.symlink('build/default/rot13.node', 'rot13.node')
