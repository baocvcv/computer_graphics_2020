## 计算机图形学 PA4

计81 包涵 2018011289



本次作业要求使用计图（jittor）框架实现Conditional GAN模型，并生成自己学号的图片。

### 环境配置

由于我想在有GPU的服务器上运行计图，但是没有root权限，服务器这两天还连不上github，所以只能下载好文件`scp`到服务器上手动安装。按照[这个教程](https://cg.cs.tsinghua.edu.cn/jittor/download/)，我下载了计图的源文件。服务器上只有Python3.6，但是计图要3.7，所以我下载了Python3.8的源码来编译，但是缺库，无法编译`_ctypes`模块，运行不了计图。还好有anaconda，用conda安装了Python3.7之后，终于能安装计图了。

服务器上有几块Titan X，也有`cuda-9.0`和`cuda-10.0`，但是设置了`nvcc_path`之后，仍然不能成功运行所有test。第一个错误如下，

```
test_backward_cuda (jittor.test.test_arg_reduce_op.TestArgReduceOp) ... [i 0614 21:38:18.362173 32 cuda_flags.cc:23] CUDA enabled.
nvcc fatal   : Value 'sm_61,sm_52' is not defined for option 'gpu-architecture'
[i 0614 21:38:18.420497 32 executor.cc:384] 
=== display_memory_info ===
 total_cpu_ram: 62.81GB total_cuda_ram: 11.91GB
 hold_vars: 6 lived_vars: 32 lived_ops: 31
 name: sfrl is_cuda: 1 used:   512 B(0.0488%) unused:  1024KB(100%) total:     1MB
 name: sfrl is_cuda: 0 used:     0 B(0%) unused:    64MB(100%) total:    64MB
 name: sfrl is_cuda: 1 used:     0 B(-nan%) unused:     0 B(-nan%) total:     0 B
 name: sfrl is_cuda: 0 used:     0 B(-nan%) unused:     0 B(-nan%) total:     0 B
 name: sfrl is_cuda: 0 used:   1.5KB(0.0366%) unused: 3.999MB(100%) total:     4MB
===========================

[e 0614 21:38:18.420601 32 executor.cc:388] [Error] source file location: /home/***/.cache/jittor/default/g++/jit/curand_random_T:float__alloc_o01__JIT:1__JIT_cuda:1__index_t:int32__hash:58b501a69c5d74b7_op.cc
[i 0614 21:38:18.420967 32 cuda_flags.cc:25] CUDA disabled.
ERROR
```

所以只好用cpu来训练。



### 训练与测试

训练就非常直接，下载了[教程](https://cg.cs.tsinghua.edu.cn/jittor/tutorial/2020-5-13-22-47-cgan/)中的cgan代码之后，直接用默认参数训练。其中一些输入如下：

- 0.jpg

  ![0](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/0.png)

- 1000.png

  ![1000-Copy1](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/1000-Copy1.png)

- 5000.png

  ![5000-Copy1](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/5000-Copy1.png)

- 10000.png

  ![10000-Copy1](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/10000-Copy1.png)

- 50000.png

  ![50000-Copy1](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/50000-Copy1.png)

- 93000.png

  ![93000-Copy1](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/93000-Copy1.png)

可以看到随着训练的进行，所生成的图片噪声越来越小，数字也越来越清晰。

最后使用测试的代码，将目标改为自己的学号，得到结果：

![result](/home/fred/External/Codes/computer_graphics/PA4.Jittor/out/result.png)