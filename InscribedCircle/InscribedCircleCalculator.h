/*************************************************************************
*
* TURBOTIDES
* An Integrated CAE Platform for Turbomachinery Design and Development
* ____________________________________________________________________
*
*  [2016] - [2020] TaiZe Inc
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of TaiZe Inc and its suppliers, if any.
* The intellectual and technical concepts contained herein
* are proprietary to TaiZe Inc and its suppliers and may
* be covered by U.S. and Foreign Patents, patents in process,
* and are protected by trade secret or copyright law.
* Dissemination of this information or reproduction of this
* material is strictly forbidden unless prior written
* permission is obtained from TaiZe Inc.
*/

#ifndef INSCRIBEDCIRCLECALCULATOR_H
#define INSCRIBEDCIRCLECALCULATOR_H

#include "util_TObject.h"

class curve_Topology;
class curve_Nurbs;
class curve_Curve;
class curve_Circle;
class TNurbsCurve;

class T_EXPORT_1D InscribedCircleCalculator : public TObject
{
	T_OBJECT;
	static const int n_tr = 10; // Number of tangent circles, member variables

private:
	double l_cc; // Length of CircleCenter line
	QVector<Double2> CircleCenters = QVector<Double2>(); // Center of all circles
	QVector<Double2> Points_hub = QVector<Double2>(); // all the points on the hub
	QVector<Double2> Points_shroud = QVector<Double2>(); // all the points on the shroud
	QVector<double> Qradius = QVector<double>(); // all the radius of the tangent circle
	QVector<Double2> L_Area = QVector<Double2>(); // all the points on the Areashow
public:
	InscribedCircleCalculator(QString object_n = "", TObject* iparent = NULL);
	virtual ~InscribedCircleCalculator();
	enum { ZR = 0, Area = 1, CurveTypeEnd };
private:
	QVector<curve_Circle*> getInscribedCircle(curve_Nurbs* c1, curve_Nurbs* c2, int num_tr = 10, double tol = tolr1);
	QVector<curve_Circle*> getInscribedCircle(curve_Nurbs& c1, curve_Nurbs& c2, int num_tr = 10, double tol = tolr1);
	int calculateInscribedCircle(curve_Nurbs* c1, curve_Nurbs* c2, int num_tr = n_tr, double tol = tolr1);
public:
	QStringList getAllTypeNames();
	QString getTypeName(int type);
	curve_Topology* getTopo(int type);
	curve_Curve* getCurve(int type);
	QString getNurbsName(int type = 0, int CurveID = 0);
	curve_Nurbs* getNurbs(int type = 0, int CurveID = 0, bool createIfNotAvaiable = true);
	QString getCircleName(int type = 0, int CurveID = 0);
	curve_Circle* getCircle(int type = 0, int CurveID = 0, bool createIfNotAvaiable = true);
	curve_Nurbs* getCenterCurve(int type, bool createIfNotAvaiable = true);
	curve_Nurbs* getAreaCurve(int type, bool createIfNotAvaiable = true);
public:
	int setTNCurve(QVector<TNurbsCurve*> TNCurves);
	int setCNCurve(QVector<curve_Nurbs*> CNCurves);
	int setCircleCurve(QVector<curve_Circle*> ICCurves);
	int setCenterLine(QVector<curve_Circle*> ICCurves);
	int calculateArea();
	int createAreaLine();
public:
	int LoadCurves();
	int newLoadCurves();
	int newCalculateCrossSection();
public:
	int createInscribedCircle();
};
#endif 