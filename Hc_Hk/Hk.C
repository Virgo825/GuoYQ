
#include <iostream>
#include <fstream>
#include <dirent.h>

#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"

using namespace std;

bool GetPath(string path, vector<string> &name); // 获取目录下的所有文件名

void Hk(string dataDir)
{
	gStyle->SetOptFit(0111);  // set fit parameter
	gStyle->SetOptStat(0000); // set hist parameter

	const double yset = 0.2; // 设置要选择的电阻值

	TFile *f = new TFile((dataDir + "_Hk_Result.root").c_str(), "recreate"); // 创建目录名命名的ROOT文件
	ofstream result((dataDir + "_Hk_Result.txt").c_str()); // 创建保存结果到txt文件

	vector<string> dataSubDir; // 子目录文件迭代器
	if (GetPath(dataDir, dataSubDir))
	{
		for (Int_t i = 0; i < dataSubDir.size(); i++)
		{
			string subDir = dataSubDir[i].substr(dataSubDir[i].rfind("/") + 1); // 子目录名字
			cout << subDir << endl;
			result << subDir << endl;

			vector<string> filename; // 数据子目录下数据文件迭代器
			if (GetPath(dataSubDir[i], filename))
			{
				f->mkdir((subDir+"_Raw").c_str()); // 在ROOT文件中创建以数据子目录命名的原始目录
				f->mkdir((subDir+"_Nor").c_str()); // 在ROOT文件中创建以数据子目录命名的归一化目录

				for (Int_t j = 0; j < filename.size(); j++) // 遍历每个数据文件
				{
					if ("txt" != filename[j].substr(filename[j].size() - 3, 3))
						continue;

					cout << filename[j] << endl;

					size_t ss = filename[j].find_last_of("/");
					string graphName = filename[j].substr(ss + 1, filename[j].size() - ss - 5); // 图名称（去掉文件后缀）
					result << graphName << "\t";

					ifstream data(filename[j].c_str());
					if (data.fail())
					{
						cout << "Can not find the file \" " << filename[j] << " \" " << endl;
						exit(-1);
					}

					vector<double> x, y;
					while (!data.eof()) // 读出数据到迭代器x与y
					{
						double temp1, temp2;
						data >> temp1 >> temp2;
						if (data.fail()) break;
						x.push_back(temp1/10000.);
						y.push_back(temp2);
					}
					data.close();

					// 获取纵坐标的最大最小值与平均值
					double xmax = (*max_element(x.begin(), x.end()));
					double xmin = (*min_element(x.begin(), x.end()));
					double ymax = (*max_element(y.begin(), y.end()));
					double ymin = (*min_element(y.begin(), y.end()));

					cout << "xmax: " << xmax << "\txmin: " << xmin << "\tymax: " << ymax << "\tymin: " << ymin << endl;
					
					const int n = x.size();
					double xx[n], yyraw[n], yy[n]; // 横坐标，原始纵坐标，归一纵坐标
					for (int i = 0; i < n; i++) // 将数据从迭代器转化成数组，用于Graph
					{
						if(abs(ymax) > abs(ymin))
							yy[i] = (y[i] - abs(ymax + ymin)/2.) / abs((ymax - ymin)/2.);
						else if(abs(ymax) < abs(ymin))
							yy[i] = (y[i] + abs(ymax + ymin)/2.) / abs((ymax - ymin)/2.);
						else
							yy[i] = y[i] / ymax;

						xx[i] = x[i];
						yyraw[i] = y[i];
					}

					// 获取0.2点的横纵坐标
					vector<double> _x;	// 结果迭代器
					for (int i = 0; i < n - 1; i++)
					{
						if (yy[i] >= yset && yy[i + 1] <= yset)
						{
							double tempx = xx[i + 1] + (xx[i] - xx[i + 1]) / (yy[i] - yy[i + 1]) * (yset - yy[i + 1]);
							_x.push_back(tempx);
						}
						if (yy[i] <= yset && yy[i + 1] >= yset)
						{
							double tempx = xx[i] + (xx[i] - xx[i + 1]) / (yy[i] - yy[i + 1]) * (yset - yy[i]);
							_x.push_back(tempx);
						}
					}
					const int nn = _x.size();
					double x_[nn], y_[nn];
					for(int i = 0; i < nn; i++)
					{
						x_[i] = _x[i];
						y_[i] = 0.2;
						cout << x_[i] << "\t";
						result << x_[i] << "\t";
					}
					cout << endl;
					result << endl;

					TGraph *gr1 = new TGraph(n, xx, yy); // 归一结果
					gr1->SetLineColor(kRed);
					gr1->SetMarkerColor(kRed);
					gr1->SetMarkerStyle(20);

					TGraph *gr2 = new TGraph(nn, x_, y_); // 待求结果
					gr2->SetLineColor(kBlue);
					gr2->SetMarkerColor(kBlue);
					gr2->SetMarkerStyle(22);

					f->cd((subDir + "_Nor").c_str()); // 进入该目录
					TMultiGraph *mg = new TMultiGraph(graphName.c_str(), graphName.c_str());
					mg->Add(gr1);
					mg->Add(gr2);
					mg->GetXaxis()->SetTitle("H(T)");
					mg->GetXaxis()->CenterTitle(true);
					mg->GetXaxis()->SetLimits((xmin - 0.2), (xmax + 0.2));
					mg->GetXaxis()->SetMaxDigits(4);
					mg->GetXaxis()->SetTitleOffset(1.2);
					mg->GetYaxis()->SetTitle("Rxy(#Omega)");
					mg->GetYaxis()->CenterTitle(true);
					mg->GetYaxis()->SetRangeUser(-1.2, 1.2);
					mg->GetYaxis()->SetMaxDigits(4);
					TCanvas *cc = new TCanvas();
					mg->Draw("ALP");
					cc->SaveAs(("./"+dataDir+"_Hk_Result/"+graphName+".eps").c_str());
					mg->Write();
					// delete gr1;
					// delete gr2;
					// delete cc;
					// delete mg;
					f->cd(); // 直接返回打开目录

					f->cd((subDir + "_Raw").c_str()); // 进入该目录
					TGraph *gr3 = new TGraph(n, xx, yyraw); // 原始结果
					gr3->SetNameTitle(graphName.c_str(), graphName.c_str());
					gr3->SetLineColor(kRed);
					gr3->SetMarkerColor(kRed);
					gr3->SetMarkerStyle(20);
					gr3->GetXaxis()->SetTitle("H(T)");
					gr3->GetXaxis()->CenterTitle(true);
					gr3->GetXaxis()->SetLimits((xmin - 0.2), (xmax + 0.2));
					gr3->GetXaxis()->SetMaxDigits(4);
					gr3->GetXaxis()->SetTitleOffset(1.2);
					gr3->GetYaxis()->SetTitle("Rxy(#Omega)");
					gr3->GetYaxis()->CenterTitle(true);
					gr3->GetYaxis()->SetRangeUser(ymin - 0.2, ymax + 0.2);
					gr3->GetYaxis()->SetMaxDigits(4);
					gr3->Draw("ALP");
					gr3->Write();
					delete gr3;
					f->cd();
				}
			}
		}
	}
	f->Close();
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
