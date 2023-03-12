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
#include <qobject.h>
class CalculateThoratArea :public QObject, public TObject
{
  Q_OBJECT;
  T_OBJECT;
public:
  CalculateThoratArea();
  //virtual ~CalculateThoratArea() {}

public:
   double getThroatAreaUpdate(int span0,int span100);
   void setprofile(int span,QVector<QVector<Double3>> profile, QVector<Double2> ZRs);
   void getLengthCurve2Curve(QVector<Double3> curve1, QVector<Double3> curve2, Double3& nearstPoint1, Double3& nearstPoint2);

private:
  double getLengthPoint2Point(Double3 point1, Double3 point2);
  double getLengthPoint2Point(Double2 point1, Double2 point2);
  double getLengthPoint2line(Double3 p0, Double3 p1, Double3 p2 );
  Double3 getFootOfPerpendicular(Double3 pt0, Double3 pt1, Double3 pt2);
  Double2 getFootOfPerpendicular(Double2 pt0, Double2 pt1, Double2 pt2);
  void getNearstPointFromLineSegment(Double3 pt0, Double3 pt1, Double3 pt2, Double3 &newPoint);
  void getNearstPointFromLineSegment(Double2 pt0, Double2 pt1, Double2 pt2, Double2 &newPoint);
  

  double getLengthPointLineSegment(Double3 pt0, Double3 pt1, Double3 pt2);
  double getLengthPointLineSegment(Double2 pt0, Double2 pt1, Double2 pt2);
 

  Double2 getPointFromLine2Line(Double2 pt1, Double2 pt2, Double2 pt3, Double2 pt4,bool &found);
  Double3 getXYZFromZ_BasedOn_2Points(double z, Double3 ptStart, Double3 ptEnd);
  double getRFromZ_BasedOn_2Points(double z, Double3 ptStart, Double3 ptEnd);
  double getZFromMinR_Basedon_2Points(Double3 pt1, Double3 pt2);

  Double2 getInsectPointFromline2Curve(Double2 ptStart, Double2 ptEnd, QVector<Double2> zrCurve,bool & found);

  void make_profile(QVector<Double3> LE_profile,QVector<Double3> ps_profile,QVector<Double3> ss_profile,bool HubFlag=true);
  void make_profiles();
  void mreverse(QVector<Double3> &p);
  void mreverse(QVector<Double2> &p);
  void getLengthPoint2Curve(Double3 point, QVector<Double3> curve,Double3 &nearstPoint);
  void getRotatedCurve(QVector<Double3> curve, double rotated_radian,QVector<Double3> &newCurve);
  void getIntersectPoint(Double3 &point1,Double3 &point2,Double3 &point3,Double3 &point4);
  double getAreaFrom4Points_base(Double3 point1, Double3 point2, Double3 point3, Double3 point4);
  double getAreaFrom3Points_base(Double3 point1, Double3 point2, Double3 point3);
  double getAreaFrom4Points(Double3 point1, Double3 point2, Double3 point3, Double3 point4);
  
  void extendEndPoint(Double3 ptStart, Double3 ptEnd,Double3 &newPoint, double factor,bool HubExtend);
  void getPointFromLine2ZRLine(Double3 ptStart, Double3 ptEnd, Double2 ZRpt1, Double2 ZRpt2, Double3& foundpt, bool& found);
  void getPointFromLine2ZRSurface(Double3 point1, Double3 point2, QVector<Double2> zrs, Double3& foundPoint, bool& found);
  double getAreaBetweenZRCurveAndLine(QVector<Double3> line1, QVector<Double3>line2, QVector<Double2> ZRs,bool HubExtendFlag);
  
  double getThroatArea_base(Double3 pt1,Double3 pt2,Double3 pt3,Double3 pt4,int span0,int span100);
  double getThroatArea(int span0,int span100);
 
  
public:
  int nBlade = 10;

  QVector<Double3> LE_profile_hub;
  QVector<Double3> LE_profile_shr;
  QVector<Double3> ps_profile_hub;
  QVector<Double3> ps_profile_shr;
  QVector<Double3> ss_profile_hub;
  QVector<Double3> ss_profile_shr;
  QVector<Double2> shrZRs;
  QVector<Double2> hubZRs;



private:
  QVector<Double3> psWithLE_profile_hub;
  QVector<Double3> psWithLE_profile_shr;
  QVector<Double3> ssWithLE_profile_hub;
  QVector<Double3> ssWithLE_profile_shr;


};


