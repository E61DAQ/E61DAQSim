#!/bin/bash

#Application path location of applicaiton

ToolDAQapp=`pwd`

source ${ToolDAQapp}/ToolDAQ/root/bin/thisroot.sh

export LD_LIBRARY_PATH=${ToolDAQapp}/lib:${ToolDAQapp}/ToolDAQ/zeromq-4.0.7/lib:${ToolDAQapp}/ToolDAQ/boost_1_60_0/install/lib:${ToolDAQapp}/ToolDAQ/root/lib/root:$LD_LIBRARY_PATH
