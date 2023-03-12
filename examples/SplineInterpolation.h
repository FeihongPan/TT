#pragma once
#include "util_TObject.h"
class TSpline : public TObject
{
  //T_OBJECT;

public:
  //TSpline(QVector<Double2> data)
  //{
  //  this->points = data;
  //};
  TSpline()
  {
  };
  void setpoints(QVector<Double2> data)
  {
    points = data;
    int data_num = points.size();
    if (data_num < 2)
    {
      error_code = None;// data_num is too small
      return;
    }
    else if (data_num == 2)
    {
      
      if (points[1][0] == points[0][0])// x1=x2
      {
        error_code = None;//2 points with the same x, can not interplotaion with new x
        return;
      }
      else
      {
        error_code = lineInterplotion;// only 2 points, means use line interplotation
        double x1 = points[0][0];
        double x2 = points[1][0];
        double y1 = points[0][1];
        double y2 = points[1][1];
        A = (y2 - y1) / (x2 - x1);
        B = (x2 * y1 - x1 * y2) / (x2 - x1);
         return;
      }

     
    }
    QVector<QVector<double>> aa;
    QVector<double> a, b, gamma, ro;

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
    return;
  };
  
  double getYFromX_base(double x)
  {
    double result=0;
    if (error_code == None)return result;
    else if (error_code == lineInterplotion) return A * x + B;
    double coef1, coef2;
    int klo = 0;
	  int khi = points.size() - 1;
	  int khiX = khi;
	  int kloX = klo;
	  
	  double t; 
		do
		{
			if (kloX == 0)
			{
				if (points[khiX - kloX - 1][0] > x)
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
				if (points[khiX - kloX][0] > x)
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

		t = (x - points[kloX][0]) / (points[khiX][0] - points[kloX][0]);

		result = (1 - t) * points[kloX][1] + t * points[khiX][1] + t * (1 - t) * (coef1 * (1 - t) + coef2 * t);
    return result;
  };
  
  QVector<double> getYsFromXs(QVector<double> x)
  {
    QVector<double> result;
    for (int i = 0; i < x.size(); i++)result.append(getYFromX_base(x[i]));
    return result;
  };
 
  QVector<Double2> getXYsFromXs(QVector<double>x) 
  {
    QVector<Double2> result;
    Double2 xy;
    for (int i = 0; i < x.size(); i++)
    {
      xy[0] = x[i];
      xy[1] = getYFromX_base(x[i]);
      result.append(xy);
    }
    return result;
  };

  Double2 getIntersectPoint(QVector<Double2> newPoints)
  {
    double c1min = min(points[0][0], points.last()[0]);
    double c1max = max(points[0][0], points.last()[0]);
    double c2min = min(newPoints[0][0], newPoints.last()[0]);
    double c2max = max(newPoints[0][0], newPoints.last()[0]);
    double xmin = max(c1min, c1min);
    double xmax = min(c1max, c2max);
    double xmid = (xmin + xmax) / 2;
    Double2 res;
    TSpline newCur;
    newCur.setpoints(newPoints);
    double tol = (xmax - xmin) / 100000;
    auto deltL = [this, &newCur](double x) {return this->getYFromX_base(x) - newCur.getYFromX_base(x); };
    while (xmax - xmin > tol)
    {
      if (deltL(xmid) * deltL(xmin) > 0)
      {
        xmin = xmid;
      }
      else
      {
        xmax = xmid;
      }
      xmid = (xmax + xmin) / 2;
    }
    res[0] = xmid;
    res[1] = this->getYFromX_base(xmid);
    return res;
  }

  /*Double2 getclosetPoint(Double2 point,bool &found)
  {//there is a problem when find the closet point
    double x1 = points.first()[0];
    double x2 = points.last()[0];
    double x0 = x1 + 0.5 * (x2 - x1);
    double tol = fabs(x2 - x1) / 10000;
    Double2 res = { 0.0,0.0 };
    auto getL = [this, point](double x) {Double2 xy = { x,this->getYFromX_base(x) }; return (xy - point).length(); };
    found = false;
    if (getL(x1) * getL(x2) > 0)return res;
    while (fabs(x1 - x0) > tol)
    {
      if (getL(x0) * getL(x1) > 0) x1 = x0;
      else x2 = x0;
      x0 = x1 + 0.5 * (x2 - x1);
    }
    found = true;
    res[0] = x0;
    res[1] = getYFromX_base(x0);
    return res;
  }*/

public:
  enum error  
  { Normal=0,lineInterplotion,None};
  error error_code = Normal;
  
private:

  QVector<Double2> points;
  QVector<double> k;
  //y=Ax+B;
  double A=0;
  double B=0;
	

};