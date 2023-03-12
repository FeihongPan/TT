/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2021] TurboTides LLC
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TurboTides LLC and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TurboTides LLC and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TurboTides LLC.
 */
#pragma once
#include "util_TObject.h"
#include "w_PropertyHolderWidget.h"
#include "chart_MultiWidget.h"
#include "draw_XYMultiWidget.h"
#include "chart_Config.h"
class exp_DevPlot :public TObject
{
  T_OBJECT;
public:
  exp_DevPlot(QString object_n = "", TObject *iparent = NULL);
  virtual ~exp_DevPlot() {}
public:
  //main GUI
  w_PropertyHolderWidget* holder = nullptr;
  w_PropertyHolderWidget* holderPlot = nullptr;
  w_PropertyHolderWidget* pltholder = nullptr;
  w_QPushButton* apply = nullptr;
  w_QPushButton* clcPlot = nullptr;
  w_Property* wGetFilName = nullptr;
  int holderNum = 0;
  //line plot
  draw_XYMultiWidget* lineMultiplot = nullptr;
  
public:
  QString fileName ="C:\\Users\\guoyh\\Desktop\\Inbox\\plotDev\\configFile.csv";

  QVector<Double2> data;
  
  
  QMap<QString, QVector<QString>> configData;
  QMap<QString, QVector<double>> performanceData;


  
public:

  void getConfigData(QString fileName, QMap<QString,QVector<QString>> &configData);
  void getPerformanceData(QString fileName,QMap<QString, QVector<double>> &data);

  void makelinePlot_base(QString tabName,QString yTitle,draw_XYInteractive* lineplot);
  void makelinePlot_tabbase(QString tabName,w_PropertyHolderWidget* pltHolder);
  void makeDevPlots(QString configFileName,w_PropertyHolderWidget* Mainholder);


  void calculateFigNums(int totalFigs,int nowIdx);
  void showGUI();
  void showDevPlot();
 
private:
  void mapDataTableFromCSV(QStringList txt, QMap<QString, QVector<double>>& data,QString seprator = ",");
  void mapStringTableFromCSV(QStringList txt, QMap<QString,QVector<QString>>&data,QString seprator = ",");

public:

  

private:
  QString seprator = ",";

  int nrow = 1;
  int ncol = 1;

 
};
static QStringList colors =
{
    QObject::tr("red"),
    QObject::tr("seagreen"),
    QObject::tr("tomato"),
    QObject::tr("wheat"),
    QObject::tr("lightskyblue"),
    QObject::tr("indianred"),
    QObject::tr("orchid"),
    QObject::tr("orange"),
    QObject::tr("navy"),
    QObject::tr("mediumslateblue"),
    QObject::tr("coral"),
    QObject::tr("Black"),
    QObject::tr("LightSalmon"),
    QObject::tr("SteelBlue"),
    QObject::tr("Yellow"),
    QObject::tr("Fuchsia"),
    QObject::tr("PaleGreen"),
    QObject::tr("PaleTurquoise"),
    QObject::tr("Cornsilk"),
    QObject::tr("HotPink"),
    QObject::tr("Peru"),
    QObject::tr("Maroon"),
    QObject::tr("LightYellow"),
    QObject::tr("Gray"),
    QObject::tr("LightGray"),
    QObject::tr("White"),
    
  };
static QVector<QColor> RGBcolors =
{
    QColor (255,0,0,255),
    QColor (0,176,80,255),
    QColor (237,125,49,255),
    QColor (0,112,192,255),
    QColor (88,182,192,255),
    QColor (255,182,185,255),
    QColor (187,222,214,255),
    QColor (91,209,215,255),
    QColor (255,80,47,255),
    QColor (81,91,212,255),
    QColor (254,218,119,255),
    QColor (166,150,200,255),
    QColor (36,112,160,255),
    
    
  };

static QVector<Double2> SplineInterpolation(QVector<Double2> points, QVector<double> xnew)
{
	int data_num = points.size();

	QVector<Double2> res;

	if (data_num < 2)
	{
		for (int i = 0; i < xnew.size(); i++)
		{
      res.push_back(points[0]);
		}
		return res;
	}
	else if (data_num == 2)
	{
		double ynew;
		for (int i = 0; i < xnew.size(); i++)
		{
			//ynew = (xnew - xlist[0]) / (xlist[1] - xlist[0]) * (ylist[1] - ylist[0]) + ylist[0];
			ynew = (xnew[i] - points[0][0]) / (points[1][0] - points[0][0]) * (points[1][1] - points[0][1]) + points[0][1];
      res.push_back({ xnew[i], ynew });
		}
		return res;
	}
	//std::vector<std::vector<double>> aa;
 
	//std::vector<double>a, b, gamma, ro, k;
  QVector<QVector<double>> aa;
  QVector<double> a, b, gamma, ro, k;
	double coef1, coef2;

	for (int i = 0; i < data_num; i++)
	{
		for (int j = 0; j < data_num; j++)
		{
			a.push_back(0.0);
		}
		aa.push_back(a);
		b.push_back(0.0);
		ro.push_back(0.0);
		k.push_back(0.0);
		a.clear();
	}

	aa[0][0] = 2.0 / (points[1][0] - points[0][0]);
	aa[0][1] = 1.0 / (points[1][0] - points[0][0]);
	b[0] = 3.0 * (points[1][1] - points[0][1]) / pow(points[1][0] - points[0][0], 2);


	aa[data_num - 1][data_num - 1] = 2.0 / (points[data_num - 1][0] - points[data_num - 2][0]);
	aa[data_num - 1][data_num - 2] = 1.0 / (points[data_num - 1][0] - points[data_num - 2][0]);
	b[data_num - 1] = 3.0 * (points[data_num - 1][1] - points[data_num - 2][1]) / ((points[data_num - 1][0] - points[data_num - 2][0])
		* (points[data_num - 1][0] - points[data_num - 2][0]));

	// the i point

	for (int i = 1; i < data_num - 1; i++)
	{
		aa[i][i - 1] = 1.0 / (points[i][0] - points[i - 1][0]);//a[i-1]
		aa[i][i] = 2.0 / (points[i][0] - points[i - 1][0]) + 2.0 / (points[i + 1][0] - points[i][0]);//a[i]
		aa[i][i + 1] = 1.0 / (points[i + 1][0] - points[i][0]);//a[i+1]
		b[i] = 3.0 * (points[i][1] - points[i - 1][1]) / (pow(points[i][0] - points[i - 1][0], 2))
			+ 3.0 * (points[i + 1][1] - points[i][1]) / pow(points[i + 1][0] - points[i][0], 2);

	}

	gamma.push_back(aa[0][1] / aa[0][0]);
	ro[0] = b[0] / aa[0][0];

	for (int i = 1; i < data_num - 1; i++)
	{
		gamma.push_back(aa[i][i + 1] / (aa[i][i] - aa[i][i - 1] * gamma[i - 1]));
	}


	for (int i = 1; i < data_num; i++)
	{
		ro[i] = (b[i] - aa[i][i - 1] * ro[i - 1]) / (aa[i][i] - aa[i][i - 1] * gamma[i - 1]);
	}

	k[data_num - 1] = ro[data_num - 1];

	for (int i = data_num - 2; i >= 0; i--)
	{
		k[i] = ro[i] - gamma[i] * k[i + 1];
	}

	int klo = 0;
	int khi = data_num - 1;


	int khiX = khi;
	int kloX = klo;
	double result;
	double t;
	int xSize = xnew.size();
	for (int i = 0; i < xSize; i++)
	{
		khiX = khi;
		kloX = klo;
		do
		{
			if (kloX == 0)
			{
				if (points[khiX - kloX - 1][0] > xnew[i])
				{
					khiX = khiX - kloX - 1;
				}
				else
				{
					kloX = khiX - kloX - 1;
				}
			}
			else
			{
				if (points[khiX - kloX][0] > xnew[i])
				{
					khiX = khiX - kloX;
				}
				else
				{
					kloX = khiX - kloX;
				}
			}

		} while (khiX - kloX > 1);


		coef1 = k[kloX] * (points[khiX][0] - points[kloX][0]) - (points[khiX][1] - points[kloX][1]);
		coef2 = -k[khiX] * (points[khiX][0] - points[kloX][0]) + (points[khiX][1] - points[kloX][1]);

		t = (xnew[i] - points[kloX][0]) / (points[khiX][0] - points[kloX][0]);

		result = (1 - t) * points[kloX][1] + t * points[khiX][1] + t * (1 - t) * (coef1 * (1 - t) + coef2 * t);
		
    res.push_back({ xnew[i],result });
	}

	return res;


}



