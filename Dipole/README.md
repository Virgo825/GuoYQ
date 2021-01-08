# 偶极相互作用计算

所需文件如下：
- `dipole.C`，计算程序
- `xyz.txt`，笛卡尔坐标系，三列数
- `m.txt`，从 `OUTCAR` 中得到相应原子的总磁矩

运行时两个数据文件与程序放在同一目录下，在终端输入：
```bash
root -l -q 'dipole.C(par1, par2, par3, par4, par5, par6)'
```
- `par1` 表示原胞中原子数（number of atoms in a unit cell）
- `par2` 表示扩胞倍数（）
- `par3` 表示晶格常数（lattice constant）
- `par4` 表示原胞类型（rect、hexa）
- `par5` 表示长方的长度
- `par6` 表示长方的宽度

程序运行结束会生成两个txt文件：
- `sum.txt` ，两两偶极相互作用结果
- `result.txt` ，两列数据，分别为级数和（积分）、级数和除以4$\pi$。
