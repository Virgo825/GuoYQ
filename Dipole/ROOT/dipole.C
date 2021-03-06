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

void dipole(string dir, int t, int n, double a, string type = "rect") // 原胞原子数、扩胞倍数、晶格常数、原胞类型
{
    double length = 1, width = 1;
    if (type == "rect")
    {
        cout << "Please input length and width of rectangle cell: ";
        scanf("%lf %lf", &length, &width);
        cout << "------------------------------------------\n"
             << "The cell type is rectangle."
             << "\nLength of the cell            : " << length
             << "\nWidth of the cell             : " << width
             << "\nNumber of the atoms in a cell : " << t
             << "\nExpansion of the cell         : " << n
             << "\nLattice constant of the cell  : " << a
             << "\n------------------------------------------" << endl;
    }
    else if (type == "hexa")
    {
        cout << "------------------------------------------\n"
             << "The cell type is hexagon."
             << "\nNumber of the atoms in a cell : " << t
             << "\nExpansion of the cell         : " << n
             << "\nLattice constant of the cell  : " << a
             << "\n------------------------------------------" << endl;
    }
    else
    {
        cout << "Please input correct cell type." << endl;
        exit(-1);
    }

    vector<double> x, y, z, m; // 创建存放 x y z m 的迭代器
    vector<double> sum;        // 创建存放累加结果的迭代器

    ifstream data((dir + "/xyz.txt").c_str()); // 打开 xyz.txt
    if (!data)
    {
        cout << "Please input correct path of xyz.txt" << endl;
        exit(-1);
    }
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

    data.open((dir + "/m.txt").c_str()); // 打开 m.txt
    if (!data)
    {
        cout << "Please input correct path of m.txt" << endl;
        exit(-1);
    }
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

    ofstream result;
    result.open((dir + "/sum.txt").c_str());

    for (int l = 0; l <= n; l++)
        for (int k = 0; k <= l; k++)
        {
            double s = 0; // 原胞相互作用值
            if (0 == l)   // 单个原胞
            {
                for (int i = 0; i < t; i++)
                    for (int j = 0; j < t; j++)
                    {
                        if (i != j) // 单个原胞不需要考虑原子自己与自己的相互作用
                            r[i][j] = (m[i] * m[j]) * (2 * pow((z[i] - z[j]), 2) - pow((x[i] - x[j]), 2) - pow((y[i] - y[j]), 2)) / pow(pow((z[i] - z[j]), 2) + pow((x[i] - x[j]), 2) + pow((y[i] - y[j]), 2), 2.5);

                        s += r[i][j];
                    }
            }
            else // 扩胞倍数大于1，有多个原胞
                for (int i = 0; i < t; i++)
                    for (int j = 0; j < t; j++)
                    {
                        if (type == "rect")
                            r[i][j] = (m[i] * m[j]) * ((2 * pow((z[i] - z[j]), 2) - pow((x[i] - x[j] + l * a), 2) - pow((y[i] - y[j] + k * width / length * a), 2)) / pow(pow((z[i] - z[j]), 2) + pow((x[i] - x[j] + l * a), 2) + pow((y[i] - y[j] + width / length * k * a), 2), 2.5));
                        else if (type == "hexa")
                            r[i][j] = (m[i] * m[j]) * ((2 * pow((z[i] - z[j]), 2) - pow((x[i] - x[j] + l * a - 0.5 * k * a), 2) - pow((y[i] - y[j] + k * 1.7320508 * 0.5 * a), 2)) / pow(pow((z[i] - z[j]), 2) + pow((x[i] - x[j] + l * a - 0.5 * k * a), 2) + pow((y[i] - y[j] + 1.7320508 * 0.5 * k * a), 2), 2.5));
                        else
                        {
                            cout << "Please input correct type of unit cell." << endl;
                            exit(-1);
                        }

                        if (0 == k)
                            s += 4 * r[i][j];
                        else if (k == l)
                            s += 4 * r[i][j];
                        else
                            s += 8 * r[i][j];
                    }

            sum.push_back(s);    // 保存结果到迭代器 sum
            result << s << endl; // 保存结果到文件中
        }
    result.close();

    result.open((dir + "/result.txt").c_str());
    for (int i = 0; i < sum.size(); i++)
    {
        double temp = 0;
        for (int j = 0; j <= i; j++) // 计算级数和
            temp += sum[j];

        result << temp << " " << temp / (4 * M_PI) << endl; // 保存 级数和、级数和除以4pi到result.txt
    }
    result.close();
}