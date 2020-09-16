#include <iostream>
#include <fstream>
#include <dirent.h>

#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TF1.h"

using namespace std;

bool GetPath(string path, vector<string> &name); // 获取目录下的所有文件名
void Hc(string dataDir, int model = 1)
{
	gStyle->SetOptFit(0111);  // set fit parameter
	gStyle->SetOptStat(0000); // set hist parameter

	TFile *f = new TFile((dataDir + "_Hc_Result.root").c_str(), "recreate"); // 创建目录名命名的ROOT文件
	ofstream result((dataDir + "_Hc_Result.txt").c_str()); // 创建txt文件数据流

	vector<string> dataSubDir; // 子目录文件迭代器
	if (GetPath(dataDir, dataSubDir)) // 遍历一级目录
	{
		for (Int_t i = 0; i < dataSubDir.size(); i++) // 逐个目录查找
		{
			string subDir = dataSubDir[i].substr(dataSubDir[i].rfind("/") + 1); // 去掉目录头
			cout << subDir << endl; // 终端输出一级目录名称
			result << subDir << endl; // 保存一级目录到txt文件

			vector<string> filename; // 数据子目录下数据文件迭代器
			if (GetPath(dataSubDir[i], filename)) // 遍历二级目录
			{
				f->mkdir((subDir+"_Raw").c_str()); // 在ROOT文件中创建以数据子目录命名的原始数据目录
				if(model != 1)
					f->mkdir((subDir+"_Deal").c_str()); // 在ROOT文件中创建以数据子目录命名的扣本底目录

				for (Int_t j = 0; j < filename.size(); j++) // 逐个文件查找
				{
					if ("txt" != filename[j].substr(filename[j].size() - 3, 3)) continue; // 判断是否以txt结尾，否则跳过

					cout << filename[j] << endl; // 终端输出处理文件名称

					size_t ss = filename[j].find_last_of("/");
					string graphName = filename[j].substr(ss + 1, filename[j].size() - ss - 5); // 标题

					ifstream data(filename[j].c_str()); // 打开txt文件数据流
					if (data.fail()) // 文件打开错误退出程序
					{
						cout << "Can not find the file \" " << filename[j] << " \" " << endl;
						exit(-1);
					}

					TGraph *gr1, *gr2, *gr3, *gr4;
					TMultiGraph *mg1, *mg2;

					double xleft = 0., xright = 0; // 左右中值横坐标

					TF1 *f1;
					double k = 0.;

					gr1 = new TGraph(); // 创建数据的点图
					gr2 = new TGraph(); // 创建结果图
					if (model != 1)
					{
						gr3 = new TGraph(); // 创建处理后的数据点图
						gr4 = new TGraph(); // 创建处理后的结果图
					}

					vector<double> x, y; //创建保存数据的迭代器x与y
					int m = 0; // 数据点个数
					while (!data.eof()) // 读出数据到迭代器x与y
					{
						double temp1, temp2;
						data >> temp1 >> temp2; // 读取数据
						if (data.fail()) break; // 读取失败跳出循环
						x.push_back(temp1); // 添加到x迭代器中
						y.push_back(temp2);
						gr1->SetPoint(m, temp1, temp2); // 将点添加到graph中
						m++;
					}
					data.close(); // 关闭数据流

					// 获取纵坐标的最大最小值与平均值
					double xmax = (*max_element(x.begin(), x.end()));
					double xmin = (*min_element(x.begin(), x.end()));
					double ymax = (*max_element(y.begin(), y.end()));
					double ymin = (*min_element(y.begin(), y.end()));
					double yave = (ymax + ymin) / 2.;

					// 获取中间点的横纵坐标
					for (int i = 0; i < m-1; i++)
					{
						if (y[i] >= yave && y[i + 1] <= yave) // x left
						{	
							xleft = x[i + 1] + (x[i] - x[i + 1]) / (y[i] - y[i + 1]) * (yave - y[i + 1]);
							gr2->SetPoint(0, xleft, yave);
						}
						if (y[i] <= yave && y[i + 1] >= yave) // x right
						{	
							xright = x[i] + (x[i] - x[i + 1]) / (y[i] - y[i + 1]) * (yave - y[i]);
							gr2->SetPoint(1, xright, yave);
						}
					}
					
					if (model != 1)
					{
						// 创建拟合函数，一次函数
						f1 = new TF1("f1", "[0]*x+[1]",  xmax * 0.7, xmax); // 创建拟合函数，设置拟合区间
						f1->SetParameter(1,1); // 设置初始参数
						gr1->Fit("f1", "QR+"); // 利用f1对gr1拟合
						k = f1->GetParameter(0); // 拟合结果k值 
					}

					// graph 格式设置
					if(model != 1)
					{
						gr1->SetLineColor(kBlack);
						gr1->SetMarkerColor(kBlack);
					}
					else
					{
						gr1->SetLineColor(kRed);
						gr1->SetMarkerColor(kRed);	
					}
					gr1->SetMarkerStyle(20);
					gr2->SetLineColor(kBlue);
					gr2->SetMarkerColor(kBlue);
					gr2->SetMarkerStyle(22);

					f->cd((subDir + "_Raw").c_str()); // 进入原始数据该目录
					mg1 = new TMultiGraph(graphName.c_str(), graphName.c_str());
					mg1->Add(gr1); // 添加数据图 红色点
					mg1->Add(gr2); // 添加结果图 蓝色点
					mg1->GetXaxis()->SetTitle("H(Oe)"); // 设置x轴标题
					mg1->GetXaxis()->CenterTitle(true); // x轴标题居中显示
					mg1->GetXaxis()->SetLimits(xmin*1.1, xmax*1.1); // 设置x轴范围
					mg1->GetXaxis()->SetMaxDigits(4); // 设置x轴数字精度
					mg1->GetXaxis()->SetTitleOffset(1.2); // 设置标题距x轴的远近
					mg1->GetYaxis()->SetTitle("Rxy(#Omega)"); // 
					mg1->GetYaxis()->CenterTitle(true);
					mg1->GetYaxis()->SetRangeUser(ymin - 0.002, ymax + 0.002); // 设置y轴范围
					mg1->GetYaxis()->SetMaxDigits(4);
					// mg1->Draw("ALP"); // 画图风格：坐标轴、直线、点
					mg1->Write(); // 保存图片到root文件
					f->cd(); // 返回上级目录

					if(model != 1)
					{
						// 去本底
						for(int i = 0; i < m; i++)
						{
							y[i] -= k * x[i];
							gr3->SetPoint(i, x[i], y[i]);
						}
						// 去本底后的纵坐标最大最小平均值
						ymax = (*max_element(y.begin(), y.end()));
						ymin = (*min_element(y.begin(), y.end()));
						yave = (ymax + ymin) / 2.;

						// 获取处理后中间点的横纵坐标
						for (int i = 0; i < m-1; i++)
						{
							if (y[i] >= yave && y[i + 1] <= yave) // x left
							{	
								xleft = x[i + 1] + (x[i] - x[i + 1]) / (y[i] - y[i + 1]) * (yave - y[i + 1]);
								gr4->SetPoint(0, xleft, yave);
							}
							if (y[i] <= yave && y[i + 1] >= yave) // x right
							{	
								xright = x[i] + (x[i] - x[i + 1]) / (y[i] - y[i + 1]) * (yave - y[i]);
								gr4->SetPoint(1, xright, yave);
							}
						}

						gr3->SetLineColor(kMagenta);
						gr3->SetMarkerColor(kMagenta);
						gr3->SetMarkerStyle(20);
						gr4->SetLineColor(kBlue);
						gr4->SetMarkerColor(kBlue);
						gr4->SetMarkerStyle(22);

						f->cd((subDir+"_Deal").c_str()); // 进入处理后数据目录
						mg2 = new TMultiGraph(graphName.c_str(), graphName.c_str());
						mg2->Add(gr3); // 添加数据图 洋红色点
						mg2->Add(gr4); // 添加结果图 蓝色点
						mg2->GetXaxis()->SetTitle("H(Oe)"); // 设置x轴标题
						mg2->GetXaxis()->CenterTitle(true); // x轴标题居中显示
						mg2->GetXaxis()->SetLimits(xmin*1.1, xmax*1.1); // 设置x轴范围
						mg2->GetXaxis()->SetMaxDigits(4); // 设置x轴数字精度
						mg2->GetXaxis()->SetTitleOffset(1.2); // 设置标题距x轴的远近
						mg2->GetYaxis()->SetTitle("Rxy(#Omega)"); // 
						mg2->GetYaxis()->CenterTitle(true);
						mg2->GetYaxis()->SetRangeUser(ymin - 0.002, ymax + 0.002); // 设置y轴范围
						mg2->GetYaxis()->SetMaxDigits(4);
						mg2->Draw("ALP"); // 画图风格：坐标轴、直线、点
						mg2->Write(); // 保存图片到root文件
						f->cd(); // 直接返回打开目录
					}

					// 终端打印结果
					cout << "xmax:" << "\t" << xmax << "\t" << "xmin:" << "\t" << xmin << endl;
					cout << "ymax:" << "\t" << ymax << "\t" << "ymin:" << "\t" << ymin << "\t" << "yave:" << "\t" << yave << endl;
					cout << "x_left:" << "\t" << xleft << "\t" << "x_right:" << "\t" <<xright << "\t" << "result:" << abs((xright - xleft) / 2.) << endl;

					// 结果写入txt文件
					result << graphName << "\t" << ymax << "\t" << ymin << "\t" << yave << "\t" << xleft << "\t" << xright << "\t" << abs((xright - xleft) / 2.) << endl;

					TCanvas *cc = new TCanvas(); // 创建画布
					if (model == 1)
					{
						mg1->Draw("ALP"); // 画图风格：坐标轴、直线、点
						cc->SaveAs(("./" + dataDir + "_Hc_Result/" + graphName + ".eps").c_str()); // 保存图片成eps格式到本地，支持修改为pdf、png、jpg等
					}
					else
					{
						mg2->Draw("ALP");
						cc->SaveAs(("./" + dataDir + "_Hc_Result/" + graphName + ".eps").c_str()); // 保存图片成eps格式到本地
					}
				}
			}
			result << endl; // 换行
		}
	}
	f->Close(); // 关闭root文件
}
bool GetPath(string path, vector<string> &name)
{
	DIR *dir = opendir(path.c_str());
	if (!dir)
	{
		cout << "opendir " << path << " error." << endl;
		return false;
	}
	dirent *p = NULL;
	while ((p = readdir(dir)) != NULL)
		if (p->d_name[0] != '.')
			name.push_back(path + "/" + string(p->d_name));

	closedir(dir);
	return true;
}