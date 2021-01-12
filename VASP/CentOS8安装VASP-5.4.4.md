# VASP 5.4.4 安装教程

[参考链接1](http://bbs.keinsci.com/thread-11812-1-1.html)    
[参考链接2](https://blog.csdn.net/qq_35548961/article/details/87943407)

## 安装说明
- 操作系统 `CentOS 8.3`，其他 `Linux` 系统步骤类似。
- 以下操作均在 `root` 账户下进行，软件均安装在 `/opt` 目录下。

## Intel编译器安装
- 采用开源 [Intel oneAPI](https://my.oschina.net/u/4303238/blog/4771794) 代替 `Intel Parallel Studio XE` ，下载 [Base Toolkit](https://software.intel.com/content/www/us/en/develop/tools/oneapi/base-toolkit/download.html) 与 [HPC Toolkit](https://software.intel.com/content/www/us/en/develop/tools/oneapi/hpc-toolkit.html) ，依次选择`Operating system` 、`Web & Local` 、`online` 会得到两个shell脚本。
- 以 `root` 权限运行这两个脚本，选择自定义安装，在 `Base` 脚本中选择 `Intel oneAPI Math Kernel Library` 安装，在 `HPC` 脚本中选择 `Intel MPI Library` 、`Intel oneAPI DPC++/C++ Compiler & intel C++ Compiler Classic` 、`Intel Fortran Compiler (Beta) & Intel Fortran Compiler Classic` 安装，默认安装路径为 `/opt/intel/oneapi` 。
- 安装结束后添加环境变量到 `~/.bashrc`
    ```bash
    # intel oneapi
    source /opt/intel/oneapi/compiler/2021.1.1/env/vars.sh
    source /opt/intel/oneapi/mpi/2021.1.1/env/vars.sh
    source /opt/intel/oneapi/mkl/2021.1.1/env/vars.sh
    ```
    
- 检查安装结果，在终端输入，若能正确输出路径，则安装成功，也可以通过运行例子来检验。
    ```bash
    which icc icpc ifort  mpiifort
    ```
    
- `FFTW` 编译，进入 `/opt/intel/oneapi/mkl/2021.1.1/interfaces/fftw3xf` ，运行 `make libintel64` 命令，运行结束会在当前目录下会产生 `libfftw3xf_intel.a` 库文件。

## openmpi 安装
- [openmpi官网](https://www.open-mpi.org/)，本次安装使用的是 `1.8.1` 版本 ([下载地址](https://www.open-mpi.org/software/ompi/v1.8/))，也可下载最新版。
- 解压安装包，并编译安装到 `/opt` 目录，`configure` 命令中需指明编译器类型，不然会使用 `gcc` 编译器。
    ```bash
    tar -xf openmpi-1.8.tar.gz
    cd openmpi-1.8.1
    ./configure --prefix=/opt/openmpi-1.8.1 CC=icc CXX=icpc F77=ifort FC=ifort
    make -j8
    make install
    ```
- 添加环境变量
    ```bash
    # openmpi
    export PATH=/opt/mpi/openmpi-1.8.1-intel/bin:$PATH
    export LD_LIBRARY_PATH=/opt/mpi/openmpi-1.8.1-intel/lib:$LD_LIBRARY_PATH
    ```
- 验证是否安装成功，成功则显示路径。
    ```bash
    which mpirun mpif90
    ```
    也可通过运行例子检验，进入安装包目录中的 `examples`
    ```bash
    cd examples/
    make
    mpirun -np 2 hello_c # 数字 2 为使用双核运行文件hello_c
    ```

## VASP 安装
- 下载安装包并解压，[下载（版本5.4.4）](https://github.com/LiuGaoyong/VASP-Compile)安装包到 `/opt/vasp` 目录，解压 `VASP` 安装包，得到 `vasp.5.4.4` 文件夹，因为 `VASP` 安装后的 `bin` 文件夹在源文件中（是没有执行 `make install` 的原因导致的？），安装后文件夹与源文件是同一文件夹。

- 修改 `makefile.include` ，进入 `vasp` 目录，把 `arch/makefile.include.linux_intel` 拷到上一级目录下改名为 `makefile.include`，里面的配置专门适合 `Intel` 编译器。打开此文件，把其中的 `OFLAG` 参数里加入 `-xhost` ，这样编译器会使得编译出的程序能够利用当前机子 `CPU` 能支持的最高档次的指令集以加速计算，也因此就没必要手动添加其它一些 `VASP` 编译教程里诸如 `-xAVX`、`-mSSE4.2` 之类的选项了。

- 编译安装，在终端输入以下内容，编译一般需要半个小时到一个小时。
    ```bash
    make veryclean # 清理之前的编译文件
    make all
    ```
    
- 检查文件，并添加环境变量。编译完成后，在 `/opt/vasp/vasp.5.4.4/bin` 目录下出现了 `vasp_gam`、`vasp_ncl`、`vasp_std` 三个可执行文件，分别是 `Gamma only版`，`非共线版` 和 `标准版`。为了使用方便，在 `bin` 文件夹中创建 `vasp_std` 软链接为 `vasp`，```ln -s vasp_std vasp```。然后在`~/.bashrc`加入以下内容后，执行 `source ~/.bashrc` 或重新进入终端，`VASP` 就可以用了。
    ```bash
    # VASP
    export PATH=/opt/vasp.5.4.4/bin:$PATH
    ```

- 测试 `VASP`，下载 [测试任务包](http://sobereva.com/attach/455/benchmark.Hg.tar.gz)，这是个含50个Hg原子的标准测试任务。将之解压，会看到 `IN-short` 和 `IN-long` ，分别是一个耗时较短和一个耗时较长任务的 `INCAR` 文件。这里将 `IN-short` 改名为 `INCAR` ，进入此目录，输入 `mpirun -np 4 vasp` 测试调用四个核心执行此任务，然后检查得到的 `OUTCAR` 看是否内容正常，没异常的话就说明完全装好了！

## FFTW 安装 
这一步可以被省略，与`intel`文件夹中编译`FFTW`重复，又与`VASP`安装中`makefile.include`文件中的一些设置有关，需要让`VASP`能找到`FFTW`，若上述没有编译`intel`的`fftw`，可采用这种方案。
- [FFTW官网](http://www.fftw.org/)，下载 [最新版3.3.9](http://www.fftw.org/download.html)
- 解压安装包，并编译安装到 `/opt` 目录
    ```bash
    tar zxvf fftw-3.3.9.tar.gz
    cd fftw-3.3.9/
    ./configure --prefix=/opt/fftw/fftw-3.3.9 --enable-mpi F77=ifort F90=ifort
    make 
    make install
    ```
- 添加环境变量
    ```bash
    # FFTW(DFT)
    export PATH=/opt/fftw/fftw-3.3.9/bin:$PATH
    export LD_LIBRARY_PATH=/opt/fftw/fftw-3.3.9/lib:$LD_LIBRARY_PATH
    ```
- 验证是否成功安装，检查 `/opt/fftw/fftw-3.3.9/lib/libfftw3_mpi.a` 文件是否存在，若存在，则安装成功。

- 在vasp的makefile.include中找到以下三行变量，将路径修改为自己的安装路径。此方案未验证。
    ```makefile
    MKLROOT =/opt/intel/oenapi/mkl
    OBJECTS = fftmpiw.o fftmpi_map.o fftw3d.o fft3dlib.o /opt/fftw/fftw-3.3.9/lib/libfftw3_mpi.a
    INCS =-I/opt/fftw/fftw-3.3.9/include
    ```
