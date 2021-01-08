/*--------------------------------------------
File Name: dipole.C
Author: Jianjin Zhou
Mail: zhoujianjin825@gmail.com 
Created Time: Fri 08 Jan 2021 09:03:13 AM DST
--------------------------------------------*/
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

void dipole(int t, int n, double a) // 原胞原子数、扩胞倍数、晶格常数
{
    vector<double> x, y, z, m; // 创建存放 x y z m 的迭代器
    vector<double> sum;        // 创建存放累加结果的迭代器

    ifstream data("./xyz.txt"); // 打开 xyz.txt
    if (!data)
        exit(-1);
    else
        for (int i = 0; i < t; i++)
        {
            // 读取数据并保存到相应迭代器
            double temp1, temp2, temp3;
            data >> temp1 >> temp2 >> temp3;
            x.push_back(temp1);
            y.push_back(temp2);
            z.push_back(temp3);
        }
    data.close();

    data.open("./m.txt"); // 打开 m.txt
    if (!data)
        exit(-1);
    else
        for (int i = 0; i < t; i++)
        {
            // 读取数据保存到迭代器 m 中
            double temp;
            data >> temp;
            m.push_back(temp);
        }
    data.close();

    double r[m.size()][m.size()]; // 两两原子相互作用数组

    ofstream result("./sum.txt");

    for (int l = 0; l <= n; l++)
        for (int k = 0; k <= l; k++)
        {
            if (0 == l) // 单个原胞
            {
                for (int i = 0; i < t; i++)
                    for (int j = 0; j < t; j++)
                        if (i != j) // 单个原胞不需要考虑原子自己与自己的相互作用
                            r[i][j] = (m[i] * m[j]) * (2 * pow((z[i] - z[j]), 2) - pow((x[i] - x[j]), 2) - pow((y[i] - y[j]), 2)) / pow(pow((z[i] - z[j]), 2) + pow((x[i] - x[j]), 2) + pow((y[i] - y[j]), 2), 2.5);
            }
            else
                for (int i = 0; i < t; i++)
                    for (int j = 0; j < t; j++)
                        r[i][j] = (m[i] * m[j]) * ((2 * pow((z[i] - z[j]), 2) - pow((x[i] - x[j] + l * a - 0.5 * k * a), 2) - pow((y[i] - y[j] + k * 1.7320508 * 0.5 * a), 2)) / pow(pow((z[i] - z[j]), 2) + pow((x[i] - x[j] + l * a - 0.5 * k * a), 2) + pow((y[i] - y[j] + 1.7320508 * 0.5 * k * a), 2), 2.5));

            double s = 0; // 
            for (int i = 0; i < t; i++)
                for (int j = 0; j < t; j++)
                    if (0 == l)
                        s += r[i][j];
                    else if (0 == k)
                        s += 4 * r[i][j];
                    else if (k == l)
                        s += 4 * r[i][j];
                    else
                        s += 8 * r[i][j];

            sum.push_back(s);    // 保存结果到迭代器 sum
            result << s << endl; // 保存结果到文件中
        }
    result.close();

    result.open("./result.txt");
    for (int i = 0; i < sum.size(); i++)
    {
        double temp = 0;
        for (int j = 0; j <= i; j++) // 计算级数和
            temp += sum[j];

        result << temp << " " << temp / (4 * M_PI) << endl;
    }
    result.close();
}