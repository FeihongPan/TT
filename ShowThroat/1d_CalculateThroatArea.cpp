/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2021]  TaiZe Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TaiZe Inc and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TaiZe Inc and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in proces
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TaiZe Inc.
 */


#include "1d_CalculateThroatArea.h"

#include<algorithm>
#if 0



#include <gsl/gsl_interp.h>
#include<gsl/gsl_spline.h>
#define DATA_COUNTS 9
#define DATA_COUNTS_Inter 10
void test()
{
    double x[DATA_COUNTS] = {2,5,10,25,30,35,40,60,70};
    double y[DATA_COUNTS] = { 3,5,7,9,10,12,15,23,28};
    double xi[] = { 6,8,12.5,27.5,32.5,37.5,52,65};
    double yi = 0;
    gsl_interp_accel* acc = gsl_interp_accel_alloc();

    gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, DATA_COUNTS);
	  gsl_spline_init (spline, x, y, DATA_COUNTS);	
    for (int i = 0; i < (sizeof(xi)/sizeof(xi[0])); i++)
    {
        yi = gsl_spline_eval(spline, xi[i],acc);
        std::cout << "xi is " << xi[i] << " and interplot yi is " << yi << std::endl;
    }
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
}

#endif // 0
CalculateThoratArea::CalculateThoratArea()
{
}

double CalculateThoratArea::getLengthPoint2Point(Double3 point1, Double3 point2)
{
  return (point1 - point2).length();
}

double CalculateThoratArea::getLengthPoint2Point(Double2 point1, Double2 point2)
{
  double res = (point1 - point2).length();
  return res;
}

double CalculateThoratArea::getLengthPoint2line(Double3 p0, Double3 p1, Double3 p2)
{
  Double3 line_vector = p1-p2;
  double line_mod = fabs(line_vector);
  Double3 point2point_vector = p0-p1;
  double point2point_mod=fabs(point2point_vector);
  double d = fabs(line_vector.dot(point2point_vector)) / line_mod;
  double h = sqrt(SQR(point2point_mod) - SQR(d));
  return h;
}

Double3 CalculateThoratArea::getFootOfPerpendicular(Double3 pt0, Double3 pt1, Double3 pt2)
{
  int num = 3;
  double x0 = pt0[0];
  double y0 = pt0[1];

  
  double x1 = pt1[0];
  double y1 = pt1[1];
  
  double x2 = pt2[0];
  double y2 = pt2[1];
  double z0 = 0.0;
  double z1 = 0.0;
  double z2 = 0.0;

  if (num == 3)
  {
    z0 = pt0[2];
    z1 = pt1[2];
    z2 = pt2[2];
  }
  
  double k = -1.0 * ((x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1) + (z1 - z0) * (z2 - z1)) / (SQR(x2 - x1) + SQR(y2 - y1) + SQR(z2 - z1));
  Double3 res = {k*(x2-x1)+x1,k*(y2-y1)+y1,k*(z2-z1)+z1};
  return res;
}

Double2 CalculateThoratArea::getFootOfPerpendicular(Double2 pt0, Double2 pt1, Double2 pt2)
{
  int num = 2;
  double x0 = pt0[0];
  double y0 = pt0[1];

  
  double x1 = pt1[0];
  double y1 = pt1[1];
  
  double x2 = pt2[0];
  double y2 = pt2[1];
  double z0 = 0.0;
  double z1 = 0.0;
  double z2 = 0.0;

  if (num == 3)
  {
    z0 = pt0[2];
    z1 = pt1[2];
    z2 = pt2[2];
  }
  
  double k = -1.0 * ((x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1) + (z1 - z0) * (z2 - z1)) / (SQR(x2 - x1) + SQR(y2 - y1) + SQR(z2 - z1));
  Double3 res = {k*(x2-x1)+x1,k*(y2-y1)+y1,k*(z2-z1)+z1};
  return res;
}

void CalculateThoratArea::getNearstPointFromLineSegment(Double3 pt0, Double3 pt1, Double3 pt2, Double3& newPoint)
{
  Double3 ptn = getFootOfPerpendicular(pt0, pt1, pt2);
  newPoint = ptn;
  double res_length = 0.0;
  double l01 = (pt0 - pt1).length();
  double l02=(pt0 - pt2).length();
  double flag = (ptn[0] - pt1[0]) * (ptn[0] - pt2[0]);
  if ((ptn[0] - pt1[0]) * (ptn[0] - pt2[0])>0)
  {
    newPoint = l01 < l02 ? pt1 : pt2;
  }
  
}

void CalculateThoratArea::getNearstPointFromLineSegment(Double2 pt0, Double2 pt1, Double2 pt2, Double2& newPoint)
{
  Double3 ptn = getFootOfPerpendicular(pt0, pt1, pt2);
  newPoint = ptn;
  double res_length = 0.0;
  double l01 = (pt0 - pt1).length();
  double l02=(pt0 - pt2).length();
  double flag = (ptn[0] - pt1[0]) * (ptn[0] - pt2[0]);
  if ((ptn[0] - pt1[0]) * (ptn[0] - pt2[0])>0)
  {
    newPoint = l01 < l02 ? pt1 : pt2;
  }
}

double CalculateThoratArea::getLengthPointLineSegment(Double3 pt0, Double3 pt1, Double3 pt2)
{
  Double3 ptn;
  getNearstPointFromLineSegment(pt0, pt1, pt2,ptn);
  return getLengthPoint2Point(pt0,ptn);
}

double CalculateThoratArea::getLengthPointLineSegment(Double2 pt0, Double2 pt1, Double2 pt2)
{
  Double2 ptn;
  getNearstPointFromLineSegment(pt0, pt1, pt2,ptn);
  return getLengthPoint2Point(pt0,ptn);
 
}


Double2 CalculateThoratArea::getPointFromLine2Line(Double2 pt1, Double2 pt2, Double2 pt3, Double2 pt4,bool &found)
{
  double x1 = pt1[0];
  double y1 = pt1[1];
  double x2 = pt2[0];
  double y2 = pt2[1];
  double x3 = pt3[0];
  double y3 = pt3[1];
  double x4 = pt4[0];
  double y4 = pt4[1];
  Double2 res = {0,0};
  found = false;
  if (x1 == x2)
  {
    if (x3 = x4)
    {
      found = false;
    }
    else
    {
      double k2 = (y4 - y3) / (x4 - x3);
      double b2 = (x4 * y3 - x3 * y4) / (x4 - x3);
      res[0] = x1;
      res[1] = k2 * res[0] + b2;
      found = true;
    }
  }
  else
  {
    if (x3 == x4)
    {
      double k1 = (y2 - y1) / (x2 - x1);
      double b1 = (x2 * y1 - x1 * y2) / (x2 - x1);
      res[0] = x3;
      res[1] = k1 * res[0] + b1;
      found = true;
    }
    else
    {
      double k1 = (y2 - y1) / (x2 - x1);
      double b1 = (x2 * y1 - x1 * y2) / (x2 - x1);
      double k2 = (y4 - y3) / (x4 - x3);
      double b2 = (x4 * y3 - x3 * y4) / (x4 - x3);
      if (k1 == k2)
      {
        found = false;
        res[0] = 0.0;
        res[1] = 0.0;
      }
      else
      {
        found = true;
        res[0] = (b2 - b1) / (k1 - k2);
        res[1] = k1 * res[0] + b1;
      }
    }
  }
  return res;
}

Double3 CalculateThoratArea::getXYZFromZ_BasedOn_2Points(double z, Double3 ptStart, Double3 ptEnd)
{
  double x0 = ptStart[0];
  double y0 = ptStart[1];
  double z0 = ptStart[2];
  double x1 = ptEnd[0];
  double y1 = ptEnd[1];
  double z1 = ptEnd[2];
  //double z0 = ptEnd[2];
  double m = x1 - x0;
  double n = y1 - y0;
  double p = z1 - z0;
  p = p == 0 ? 1e10 : p;
  double t = (z - z0) / p;
  Double3 xyz;
  xyz[0] = x0 + m * t;
  xyz[1] = y0 + n * t;
  xyz[2] = z;
  return xyz;
}

double CalculateThoratArea::getRFromZ_BasedOn_2Points(double z, Double3 ptStart, Double3 ptEnd)
{
  double znew = z;
  double zmax = max(ptStart[2], ptEnd[2]);
  double zmin = min(ptStart[2], ptEnd[2]);
  znew = min(znew, zmax);
  znew = max(znew, zmin);
  Double3 xyz = getXYZFromZ_BasedOn_2Points(znew, ptStart, ptEnd);
  return sqrt(SQR(xyz[0]) + SQR(xyz[1]));
}

double CalculateThoratArea::getZFromMinR_Basedon_2Points(Double3 pt1, Double3 pt2)
{
    double x0 = pt1[0];
    double y0 = pt1[1];
    double z0 = pt1[2];
    double x1 = pt2[0];
    double y1 = pt2[1];
    double z1 = pt2[2];
    double m = x1 - x0;
    double n = y1 - y0;
    double p = z1 - z0;
    double a = (m * m + n * n) / (p * p);
    double b = (m * x1 + n * y1 - (m * m * z1 + n * m * z1) / p) * 2 / p;
    double z = -b / (2 * a);
  return z;
}

Double2 CalculateThoratArea::getInsectPointFromline2Curve(Double2 ptStart, Double2 ptEnd, QVector<Double2> zrCurve,bool & found)
{
  Double2 res;
  found = false;
  if (zrCurve.first()[0] > zrCurve.last()[0])
  {
    //mreverse(zrCurve);
    std::reverse(zrCurve.begin(),zrCurve.end());
  }
  if (ptStart[0] == ptEnd[0]) 
  {
    if (ptStart[0] <= zrCurve.first()[0])
    {
      res[0] = zrCurve.first()[0];
      res[1] = zrCurve.first()[1];
    }
    else if (ptStart[0] >= zrCurve.last()[0])
    {
      res[0] = zrCurve.last()[0];
      res[1] = zrCurve.last()[1];
    }
    else
    {
      //todo
    }
  }

  double k=(ptStart[1] - ptEnd[1]) / (ptStart[0] - ptEnd[0]);
  double b = (ptStart[0] * ptEnd[1] - ptEnd[0] * ptStart[1]) / (ptStart[0] - ptEnd[0]);
  int idx_min = 0;
  int idx_max = zrCurve.size()-1;
  int idx_m = (idx_min + idx_max) / 2;
  double l_min = k * zrCurve[idx_min][0] + b - zrCurve[idx_min][1];
  double l_max = k * zrCurve[idx_max][0] + b - zrCurve[idx_max][1];
  Double2 pt1;
  Double2 pt2;
  if (l_max * l_min > 0)
  {
    found = false;
   
    getNearstPointFromLineSegment(zrCurve[0], ptStart, ptEnd,pt1);
    getNearstPointFromLineSegment(zrCurve.last(), ptStart, ptEnd, pt2);
    double lpt1 = getLengthPoint2Point(zrCurve[0], pt1);
    double lpt2 = getLengthPoint2Point(zrCurve.last(), pt2);
    res = lpt1 > lpt2 ? pt2 : pt1;
   
  }
  else 
  {
    double l_m=k * zrCurve[idx_m][0] + b - zrCurve[idx_m][1];
    while (idx_max - idx_min > 1)
    {
      if (l_m * l_min > 0)idx_min = idx_m;
      else idx_max = idx_m;
      idx_m = (idx_min + idx_max) / 2;
      l_m=k * zrCurve[idx_m][0] + b - zrCurve[idx_m][1];
    }
    bool found = false;
    if (idx_m == 0)
    {
      pt1 = getPointFromLine2Line(ptStart, ptEnd, zrCurve.first(), zrCurve[1], found);
      if (found)res = pt1;
      else (res = zrCurve.first());
    }
    else if (idx_m == zrCurve.size() - 1)
    {
      pt1 = getPointFromLine2Line(ptStart, ptEnd, zrCurve.last(), zrCurve[idx_m-1], found);
      if (found)res = pt1;
      else (res = zrCurve.first());
    }
    else
    {
      //need to make more robust in case there is no intersect
      bool found2 = false;
      pt1 = getPointFromLine2Line(ptStart, ptEnd, zrCurve[idx_m], zrCurve[idx_m-1], found);
      pt2 = getPointFromLine2Line(ptStart, ptEnd, zrCurve[idx_m], zrCurve[idx_m+1], found2);
      if (found) res = pt1;
      else res = pt2;
    }

  }

  return res;
}

void CalculateThoratArea::make_profile(QVector<Double3> LE_profile,QVector<Double3> ps_profile,QVector<Double3> ss_profile,bool HubFlag)
{
  QVector<Double3> ssWithLE_profile;
  QVector<Double3> psWithLE_profile;
  Double3 leStartPoint = LE_profile[0];

  double rMax = 0.0;
  double rMin = 0.0;
  rMin = SQR(ss_profile[0][0]) + SQR(ss_profile[0][1]);
  rMax = SQR(ss_profile.last()[0]) + SQR(ss_profile.last()[1]);
  //if (rMax > rMin)mreverse(ss_profile);//R for ss_profile need to be larget to small 
  if (rMax > rMin)std::reverse(ss_profile.begin(),ss_profile.end());//R for ss_profile need to be larget to small 
  rMin = SQR(ps_profile[0][0]) + SQR(ps_profile[0][1]);
  rMax = SQR(ps_profile.last()[0]) + SQR(ps_profile.last()[1]);
  Double3 ssStartPoint = ss_profile[0];
  Double3 psStartPoint = ps_profile[0];
  //if (rMax < rMin)mreverse(ps_profile);//R for ps_profile need to be small to large
  if (rMax < rMin)std::reverse(ps_profile.begin(),ps_profile.end());//R for ps_profile need to be small to large

  if ((leStartPoint[0] == ps_profile.first()[0]) && (leStartPoint[1] == ps_profile.first()[1]) && (leStartPoint[2] == ps_profile.first()[2]))mreverse(LE_profile);
  
  for (int i = ss_profile.size() / 4; i < ss_profile.size(); i++)ssWithLE_profile.push_back(ss_profile[i]);
  

  for (int i = 0; i < ps_profile.size() / 4; i++)psWithLE_profile.push_back(ps_profile[i]);

  double rotated_angle =360.0 / nBlade * PI / 180.0;
  if (HubFlag)
  {
    psWithLE_profile_hub = psWithLE_profile;
    ssWithLE_profile_hub = ssWithLE_profile;
    for (int i = 0; i < psWithLE_profile_hub.size(); i++)psWithLE_profile_hub[i] = psWithLE_profile_hub[i].rotate2D(rotated_angle);
    
  }
  else
  {
    psWithLE_profile_shr = psWithLE_profile;
    ssWithLE_profile_shr = ssWithLE_profile;
    for (int i = 0; i < psWithLE_profile_shr.size(); i++)psWithLE_profile_shr[i] = psWithLE_profile_shr[i].rotate2D(rotated_angle);
  }
  
}

void CalculateThoratArea::make_profiles()
{
  make_profile(LE_profile_hub, ps_profile_hub, ss_profile_hub,true);
  make_profile(LE_profile_shr, ps_profile_shr, ss_profile_shr,false);
}


void CalculateThoratArea::mreverse(QVector<Double3>& p)
{
  QVector<Double3> pp;
  pp.resize(p.size());
  int j = 0;
  for (int i = p.size()-1; i >= 0; i--)
  {
    pp[j] = p[i];
    j += 1;
  }
  p.clear();
  p = pp;
}

void CalculateThoratArea::mreverse(QVector<Double2>& p)
{
  QVector<Double2> pp;
  pp.resize(p.size());
  int j = 0;
  for (int i = p.size()-1; i >= 0; i--)
  {
    pp[j] = p[i];
    j += 1;
  }
  p.clear();
  p = pp;
}

void CalculateThoratArea::getLengthPoint2Curve(Double3 point, QVector<Double3> curve, Double3& nearstPoint)
{

  auto getLengthPoint2CurveFromidx = [point, curve](int i) {
    return sqrt(SQR(point[0] - curve[i][0])+SQR(point[1] - curve[i][1])+SQR(point[2] - curve[i][2])); };

  
  int fIdx = getLengthPoint2CurveFromidx(0) <getLengthPoint2CurveFromidx(curve.size() - 1) ? 0 : curve.size() - 1;//found idx

  for (int i = 1; i < curve.size()-2; i++)
  {
    if (curve.size() == 2)
    {
      return;
    }
    if ((getLengthPoint2CurveFromidx(i) < getLengthPoint2CurveFromidx(i + 1)) && (getLengthPoint2CurveFromidx(i)<getLengthPoint2CurveFromidx(i-1)))
    {
      fIdx = i;
      break;
    }

  }
  QVector<double>l1, l2;
  l1.resize(curve.size());
  //l2.resize(curve.size());
  for (int i = 0; i < l1.size(); i++)l1[i] = getLengthPoint2CurveFromidx(i);
  l2 = l1;
  qSort(l1.begin(), l1.end());
  for (int i = 0; i < l1.size(); i++)
  {
    if (l1[0] == l2[i])
    {
      fIdx = i;
      break;
    }
  }

  nearstPoint = curve[fIdx];
#if 1



  if (fIdx == 0)
  {
    getNearstPointFromLineSegment(point, curve[fIdx], curve[fIdx + 1], nearstPoint);

  }
  else if (fIdx == curve.size() - 1)
  {
    getNearstPointFromLineSegment(point, curve[fIdx], curve[fIdx - 1], nearstPoint);
  }
  else
  {
    Double3 pt1;
    Double3 pt2;
    getNearstPointFromLineSegment(point, curve[fIdx], curve[fIdx - 1],pt1);
    getNearstPointFromLineSegment(point, curve[fIdx], curve[fIdx +1], pt2); 
    double lpt1 = sqrt(SQR(point[0] - pt1[0])+SQR(point[1]-pt1[1])+SQR(point[2]-pt1[2]));
    double lpt2 = sqrt(SQR(point[0] - pt2[0])+SQR(point[1]-pt2[1])+SQR(point[2]-pt2[2]));
    nearstPoint = lpt1 > lpt2 ? pt2 : pt1;
  }
#endif // 0

  return;

}

void CalculateThoratArea::getLengthCurve2Curve(QVector<Double3> curve1, QVector<Double3> curve2, Double3& nearstPoint1, Double3& nearstPoint2)
{
  int fIdx = 0;
  double tmp_lenght = 0.0;


  Double3 nearstPoint1_tmp;
  Double3 nearstPoint2_tmp;
  nearstPoint1 = curve1[0];
  getLengthPoint2Curve(nearstPoint1, curve2, nearstPoint2);
  double min_length = getLengthPoint2Point(nearstPoint1, nearstPoint2);

  for (int i = 1; i < curve1.size(); i++)
  {
    nearstPoint1_tmp = curve1[i];
    getLengthPoint2Curve(nearstPoint1_tmp, curve2, nearstPoint2_tmp);
    tmp_lenght = getLengthPoint2Point(nearstPoint1_tmp, nearstPoint2_tmp);
    if (min_length > tmp_lenght)
    {
      min_length = tmp_lenght;
      nearstPoint1 = nearstPoint1_tmp;
      nearstPoint2 = nearstPoint2_tmp;
    }

  }
  return;
}

void CalculateThoratArea::getRotatedCurve(QVector<Double3> curve, double rotated_radian, QVector<Double3>& newCurve)
{
  newCurve.clear();
  newCurve.resize(curve.size());

  for (int i = 0; i < curve.size(); i++)newCurve[i] = curve[i].rotate2D(rotated_radian);
  return;
}

void CalculateThoratArea::getIntersectPoint(Double3 &point1,Double3 &point2,Double3 &point3,Double3 &point4)
{
  // pt1:psHub,pt2:ssHub pt3,ssShr,pt4,psShr
  
  make_profiles();
  
  getLengthCurve2Curve(psWithLE_profile_hub,ssWithLE_profile_hub,point1,point2);
  
  getLengthCurve2Curve(psWithLE_profile_shr,ssWithLE_profile_shr,point4,point3);


}

double CalculateThoratArea::getAreaFrom3Points_base(Double3 point1, Double3 point2, Double3 point3)
{
  double s1 = 0.0;
  double p = 0.0;
  double a = getLengthPoint2Point(point1, point3);
  double b = getLengthPoint2Point(point2, point3);
  double c = getLengthPoint2Point(point1, point2);

  if (a * b * c == 0) { s1 = 0.0; }
  else
  {
    p = (a + b + c) / 2;
    s1 = sqrt(p * (p - a) * (p - b) * (p - c));
  }
  return s1;
}
double CalculateThoratArea::getAreaFrom4Points_base(Double3 point1, Double3 point2, Double3 point3, Double3 point4)
{
    /*
  
  point1 ----L---- point2
    |               |
    |               |
    H               H
    |               |
  point4 ----L---- point3
  
  */

  double s1 = getAreaFrom3Points_base(point1, point2, point3);
  double s2 = getAreaFrom3Points_base(point1, point4, point3);
  double s3 = s1 + s2;
  return s3;
}


#if 0
double CalculateThoratArea::getAreaFrom4Points(Double3 point1, Double3 point2, Double3 point3, Double3 point4)
{
  /*

point1 ----L---- point2
  |               |
  |               |
  H               H
  |               |
point4 ----L---- point3

*/


  double s = 0.0;
  Double3 pt01, pt02, pt03, pt04, pt11, pt12, pt13, pt14;
  int nptrow = 20;
  int nptcol = 20;
  pt01 = point1;
  pt02 = point2;

  for (int i = 1; i < nptrow; i++)
  {
    pt03 = point2 + (point3- point2) * i / (nptrow - 1);
    pt04 = point1 + (point4 - point1) * i / (nptrow - 1);
   /* for (int k = 0; k < 3; k++)
    {
      pt03[k] = point2[k] + (point3[k] - point2[k]) * i / (nptrow - 1);
      pt04[k] = point1[k] + (point4[k] - point1[k]) * i / (nptrow - 1);
    }*/
    pt11 = pt01;
    pt14 = pt04;
    for (int j = 1; j < nptcol; j++)
    {
       pt12 = pt01 + (pt02 - pt01)*j/(nptcol-1);
       pt13 = pt04 + (pt04 - pt03) * j / (nptcol - 1);
      
  /*    for (int k = 0; k < 3; k++)
      {
        pt12[k] = pt01[k] + (pt02[k] - pt01[k])*j/(nptcol-1);
        pt13[k] = pt04[k] + (pt04[k] - pt03[k])*j/(nptcol-1);
      }*/
      s += getAreaFrom4Points_base(pt11, pt12, pt13, pt14);
      pt11 = pt12;
      pt14 = pt13;
    }
    
    pt01 = pt03;
    pt02 = pt04;
  }
  return s;
}

#endif

void CalculateThoratArea::getPointFromLine2ZRLine(Double3 ptStart, Double3 ptEnd, Double2 ZRpt1, Double2 ZRpt2, Double3& foundpt, bool& found)
{
  Double3 tmp_point;
  if (ptEnd[2] < ptStart[2])
  {
    tmp_point = ptStart;
    ptStart = ptEnd;
    ptEnd = tmp_point;
  }
  auto getRfromZR = [ZRpt1, ZRpt2](double z) {return (ZRpt2[1] - ZRpt1[1]) / (ZRpt2[0] - ZRpt1[0]) * (z - ZRpt1[0]) + ZRpt1[1]; };
  auto deltR = [getRfromZR, ptStart, ptEnd, this](double z) {return this->getRFromZ_BasedOn_2Points(z, ptStart, ptEnd) - getRfromZR(z); };
  double tol = fabs(ZRpt2[0] - ZRpt1[0]) / 1e8;
  double zmin = min(ptStart[2], ZRpt1[0]);
  double zmax = max(ptEnd[2], ZRpt2[0]);
  double zmid = (zmax + zmin) / 2.0;
  double drMax = deltR(zmax);
  double drMin = deltR(zmin);
  int n = 0;
  if (ZRpt1[0] > ptEnd[2]) { found = false; return; }
  else if (ZRpt2[0] < ptStart[2]) { found = false; return; }
  else if (deltR(zmax) * deltR(zmin) > 0){ found = false; return; }
  else
  {
    while (fabs(zmax - zmin) > tol)
    {
      if (deltR(zmin) * deltR(zmid) > 0) { zmin = zmid; }
      else zmax = zmid;
      zmid = (zmax + zmin) / 2.0;
      n += 1;
      if (n > 100)break;
    }
    foundpt = getXYZFromZ_BasedOn_2Points(zmid, ptStart, ptEnd);

  }
  return;
}


void CalculateThoratArea::getPointFromLine2ZRSurface(Double3 point1, Double3 point2, QVector<Double2> zrs, Double3& foundPoint, bool& found)
{
  auto deltR = [point1, point2, zrs, this](int idx) {return getRFromZ_BasedOn_2Points(zrs[idx][0], point1, point2)-zrs[idx][1]; };
  double dr = fabs(deltR(0));
  double dr0 = 0;
  int idxFlag = 0;
  int idx_m = 0;
  double maxZ = max(point1[2], point2[2]);
  double minZ = min(point1[2], point2[2]);
 
  int idx_offset = 0;
  for(int i = 0;i < zrs.size(); i++)
  {
    dr0 = fabs(deltR(i));
    if (dr0 < dr)
    {
      dr = dr0;
      idx_m = i;
    }
    if (zrs[i][0] > maxZ)idxFlag += 1;
    if (idxFlag == 2)break;
  }
  if (idx_m <= 1) idx_m = 0;
  else if (idx_m > zrs.size() - 4)idx_m = zrs.size() - 4;
  else idx_m = idx_m - 1;
  
  for(int i=0; i<3;i++)
    {
      getPointFromLine2ZRLine(point1, point2, zrs[idx_m+i], zrs[idx_m + i+1], foundPoint, found);
      if (found) return;
    }
}


void CalculateThoratArea::extendEndPoint(Double3 ptStart, Double3 ptEnd,Double3 &newPoint, double factor,bool HubExtend)
{
 
  if (HubExtend)
  {
    newPoint = getXYZFromZ_BasedOn_2Points(getZFromMinR_Basedon_2Points(ptStart, ptEnd), ptStart, ptEnd);
  }
  else
  {
    newPoint= ptStart + (ptEnd - ptStart) * factor;
  }
  

}

double CalculateThoratArea::getAreaBetweenZRCurveAndLine(QVector<Double3> line1, QVector<Double3> line2, QVector<Double2> ZRs,bool HubExtendFlag)
{
  /*
  
  line1StartPoint  ------------- line2StarPoint
        |                              |
        |              Area            |
        |         ------------         |
        |     -                   -    |
        |  -                         - |
        |-             ZRs            -|
  line1EndPoint  --------------- line2EndPoint
  
  */
  int npts = 20;
  Double3 lineStartSideL=line1[0];
  Double3 lineEndSideL=line1[1];
  Double3 lineStartSideR;
  Double3 lineEndSideR;
  Double3 ZRpointL = line1[1];
  Double3 ZRpointR;
  bool found = false;
  double Area=0;
  double factor = 4.0;
  Double3 ptStart, ptEnd;

  for (int i = 1; i < npts; i++)
  {
    lineStartSideR = line1[0] + (line2[0] - line1[0]) * i / (npts - 1);
    lineEndSideR = line1[1] + (line2[1] - line1[1]) * i / (npts - 1);
    ptStart = lineStartSideR;
    extendEndPoint(ptStart, lineEndSideR, ptEnd, factor,HubExtendFlag);

    getPointFromLine2ZRSurface(ptStart, ptEnd, ZRs, ZRpointR, found);
    
    Area += getAreaFrom4Points_base(lineEndSideL, lineEndSideR, ZRpointR, ZRpointL);
    lineEndSideL = lineEndSideR;
    ZRpointL = ZRpointR;
  }
  return Area;
}



double CalculateThoratArea::getThroatArea(int span0,int span100)
{
  Double3 pt1;
  Double3 pt2;
  Double3 pt3;
  Double3 pt4;
  double factor = 1.00;
  
  getIntersectPoint(pt1, pt2,pt3,pt4);

  double A = getThroatArea_base(pt1, pt2, pt3, pt4,span0,span100);
  return A*factor;
}


double CalculateThoratArea::getThroatArea_base(Double3 pt1, Double3 pt2, Double3 pt3, Double3 pt4,int span0,int span100)
{
/*
  line1StartPoint  ------------- line2StarPoint
        |                              |
        |              Area            |
        |         ------------         |
        |     -                   -    |
        |  -                         - |
        |-             ZRs            -|
  line1EndPoint  --------------- line2EndPoint
 
  pt1:psHub,pt2:ssHub pt3,ssShr,pt4,psShr
  */

  double Amain = 0.0;
  int npt = 20;// change from 30
  double unitFactor = 1000000.0;
  Double3 pt12L = pt1;
  Double3 pt34L = pt3;
  Double3 pt12R;
  Double3 pt34R;
  QVector < Double3 > line1 = { pt1, pt4 };
  QVector < Double3 > line2 = { pt2, pt3 };
  for (int i = 1; i < npt + 1; i++)
  {
    pt12R = pt1 + (pt2 - pt1) * i / npt;
    pt34R = pt3 + (pt4- pt3) * i / npt;
    Amain += getAreaFrom4Points_base(pt12L, pt12R, pt34L, pt34R);
    pt12L = pt12R;
    pt34L = pt34R;
  }
  double Ashr = 0;
  double Ahub = 0;
  if (span100 == 100)Ashr = getAreaBetweenZRCurveAndLine(line1, line2, shrZRs, false);
  //else Ashr = 0.0;
  line1[0] = pt4;
  line1[1] = pt1;
  line2[0] = pt3;
  line2[1] = pt2;
  double Atotal = (Amain + Ashr - Ahub)*nBlade;

  return Atotal;
}

void CalculateThoratArea::setprofile(int span, QVector<QVector<Double3>> profile, QVector<Double2> ZRs)
{
  //1.  0.LECurve 1.LeftCurve 2.RightCurve 3.TECurve;
  if (span == 0)
  {
    hubZRs = ZRs;
    LE_profile_hub = profile[0];
    ss_profile_hub = profile[2];
    ps_profile_hub = profile[1];
  }
  else
  {
    shrZRs = ZRs;
    LE_profile_shr = profile[0];
    ss_profile_shr = profile[2];
    ps_profile_shr = profile[1];
  }


}

double CalculateThoratArea::getThroatAreaUpdate(int span0, int span100)
{
  /*pt1:psHub,pt2:ssHub pt3,ssShr,pt4,psShr*/
  Double3 pt1;
  Double3 pt2;
  Double3 pt3;
  Double3 pt4;
  double factor = 1.026;
  double A = 1000000000000.0;
  make_profiles();
  for (int i = 0; i < psWithLE_profile_hub.size()/2; i++)
  {
    pt1 = psWithLE_profile_hub[i];
    for (int j = ssWithLE_profile_hub.size()/5; j < ssWithLE_profile_hub.size(); j++)
    {
      pt2 = ssWithLE_profile_hub[j];
      for (int k = ssWithLE_profile_shr.size()/3; k < ssWithLE_profile_shr.size(); k++)
      {
        pt3 = ssWithLE_profile_shr[k];
        for (int m = 0; m < psWithLE_profile_shr.size()/4; m++)
        {
          pt4 = psWithLE_profile_shr[m];
          A=min(A,getThroatArea_base(pt1, pt2, pt3, pt4,span0,span100));

        }
      }
    }
  }
  return A*factor;
}


