# 偶极相互作用计算

## 计算公式
- **一个原胞**，$i$ 与 $j$ 均从 $1$ 循环到 $t$（原胞原子数），$i$ 与 $j$ 相同时不计算，最后对所有 $r_{i,j}$ 累加得到相互作用。
$$r_{i,j} = m_{i} \times m_{j} \times \frac{2 \times (z_{i} - z_{j})^2 - (x_{i} - x_{j})^2 - (y_{i} - y_{j})^2}{((z_{i} - z_{j})^2 + (x_{i} - x_{j})^2 + (y_{i} - y_{j})^2)^{\frac{5}{2}}} $$
$$\sum_{i=1,j=1}^{i=t,j=t}r_{i,j} (i \neq j)$$

- **多个原胞**，两层循环，$l$ 从 $0$ 循环到 $n$（扩胞倍数）, $k$ 从 $0$ 循环到 $l$，扩胞倍数 $l$ 先循环，循环过程中对 $k$ 再循环，总共循环 $\sum_{i=1}^{n+1}$ 次，也就是 `sum.txt` 中的行数。
每次循环中计算相互作用，$i$ 与 $j$ 均从 $1$ 循环到 $t$（原胞原子数），对所有的 $r_{i,j}$ 乘 $4$ 或 $8$ 后，累加得到相互作用。
    - **长方**
$$r_{i,j} = m_{i} \times m_{j} \times \frac{2 \times(z_{i} - z_{j})^2 - (x_{i} - x_{j} + l \times a)^2 - (y_{i} - y_{j} + k \times \frac{width}{length} \times a)^2}{((z_{i} - z_{j})^2 + (x_{i} - x_{j} + l \times a)^2 + (y_{i} - y_{j} + \frac{width}{length} \times k \times a)^2)^{\frac{5}{2}}}$$

    - **六方**
$$r_{i,j}=m_{i} \times m_{j} \times \frac{2 \times (z_{i} - z_{j})^2 - (x_{i} - x_{j} + l \times a - \frac{1}{2} \times k \times a)^2 - (y_{i} - y_{j} + k \times \frac{\sqrt{3}}{2} \times a)^2}{((z_{i} - z_{j})^2 + (x_{i} - x_{j} + l \times a - \frac{1}{2}\times k \times a)^2 + (y_{i} - y_{j} + \frac{\sqrt{3}}{2} \times k \times a)^2)^{\frac{5}{2}}}$$


## 使用ROOT程序计算
进入 `ROOT` 文件夹中，文件如下：

![](./Figure/fig1.png)

- `dipole.sh` 运行脚本
- `dipole.C` 计算程序
- `xxx` 数据文件夹，包含两个文件：
    - `xyz.txt` 笛卡尔坐标系，三列数
    - `m.txt` 从 `OUTCAR` 中得到相应原子的总磁矩

运行时运行脚本（`dipole.sh`）、计算程序（`dipole.C`）与数据文件夹（`xxx`）放在同一目录下，在终端输入：
```bash
sh dipole.sh par1 par2 par3 par4 par5 
```
- `par1` 表示 数据文件（`xyz.txt` `m.txt`）存放文件夹名称 `xxx`
- `par2` 表示原胞中原子数（number of atoms in a unit cell）
- `par3` 表示扩胞倍数
- `par4` 表示晶格常数（lattice constant）
- `par5` 表示原胞类型（`rect`、`hexa`），若为 `rect`，则会提示输入长和宽

![](./Figure/fig2.png)

程序运行结束会在数据文件夹 `xxx` 中生成两个txt文件：
- `sum.txt` ，两两偶极相互作用结果
- `result.txt` ，两列数据，分别为级数和（积分）、级数和除以4$\pi$。

![](./Figure/fig3.png)


## 使用ifort编译计算
进入 `ifort` 文件夹中，文件如下：

![](./Figure/fig4.png)

- `prog.f90` 计算单个原胞的偶极相互作用
- `sum.f90` 对单个原胞结果积分并除以4$\pi$
- `x5` 笛卡尔坐标系x
- `y5` 笛卡尔坐标系y
- `z5` 笛卡尔坐标系z
- `m5` 原子总磁矩m

运行说明：
- 核对 `prog.f90` 开头数组长度是否与数据文件行数（原子数）一致，修改 `dimension(xx)` 括号内的值 
    ![](./Figure/fig5.png)
- 利用 `ifort` 编译 `prog.f90` 并运行，打开终端输入：
    ```bash
    ifort prog.f90 -o prog
    ./prog
    ```
    - `-o` 后的 `prog` 指生成可执行文件的名称，可自定义
    - 运行过程中会提示输入原胞中原子数（t），随后又会提示输入晶格常数（a）与扩胞倍数（n）
    - 运行结束会生成名为 `sum` 的文件
- 核对 sum.f90 中数组长度以及循环次数是否与`sum`文件行数一致，修改`dimension(xx)` 括号内的值与` do i=1,xx` 的值
    ![](./Figure/fig7.png)

    ![](./Figure/fig8.png)
    
    ![](./Figure/fig9.png)
- 利用 `ifort` 编译 `sum.f90` 并运行，打开终端输入：
    ```bash
    ifort sum.f90 -o sum1
    ./sum1
    ```
    - `-o` 后的 `sum1` 指生成可执行文件的名称，可自定义
    - 运行结束会生成名为 `conv` 的文件，两列

![](./Figure/fig6.png)
