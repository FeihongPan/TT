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

#include "InscribedCircleCalculator.h"
#include "curve_Curve.h"
#include "curve_Nurbs.h"
#include "curve_Topology.h"
#include "curve_Curve.h"
#include "curve_Circle.h"


REGISTER_OBJECT_CLASS(InscribedCircleCalculator, "Inscribed Circle Calculator", TObject);

InscribedCircleCalculator::InscribedCircleCalculator(QString object_n, TObject* iparent): TObject(object_n, iparent)
{
	INIT_OBJECT;
	l_cc = 0.;
}

// 0.Method 1
QVector<curve_Circle*> InscribedCircleCalculator::getInscribedCircle(curve_Nurbs* c1, curve_Nurbs* c2, int num_tr, double tol)
{
	// QVector<curve_Nurbs*> NCurves -> QVector<curve_Circle*> InscribedCircleCurves
	// 1.QVector
	QVector<curve_Circle*> ICCurves;
	QVector<Double2> QCenter;
	QVector<double> QRadius;

	// 2.getCurve
	curve_Nurbs* hub = c1;
	curve_Nurbs* shroud = c2;
	if (!hub || !shroud)
		return {};

	int num_radius = 10000; // Number of radius iterations
	double dus = 1.0 / (num_tr - 1);
	int num_uh = 1E7 * tol;
	double us = 0;
	double radius = 0.;

	// 3. getCenters and Radius
	for (int k = 0; k < num_tr; k++)
	{
		// 3.1 get Point
		Double2 point_A = shroud->getPoint(us);
		// 3.2 Tangential direction
		Double2 tangential_A = shroud->getTangent(us);
		// 3.3 mag
		double length = tangential_A.length();
		tangential_A /= length;
		// 3.4 normal
		Double2 normal_A = tangential_A.rotate(PI / 2.);
		// 3.5 radius
		double distance = (point_A - hub->getPoint(us)).length();
		radius = distance / 3;

		for (int i = 0; i < num_radius; i++)
		{
			// Number of intersections.
			int num_r = 0;
			double dl = 0.;
			double uh = 0.;
			// Circlecenter
			Double2 Circlecenter = (point_A - radius * normal_A);

			for (int j = 0; j <= num_uh; j++)
			{
				// get hub Point
				uh = 1. * j / num_uh;
				Double2 point_h = hub->getPoint(uh);
				// Distance between two points
				dl = (point_h - Circlecenter).length() - radius;
				if (dl < -tol)
					num_r += 1;
				if (num_r == 1)
					break;
				continue;
			}
			switch (num_r)
			{
			case 1:
				if (i == 0)
				{
					radius = distance / 10;
					continue;
				}
				QRadius.push_back(radius);
				break;
			case 0:
				radius *= 1. + 50 * tol;
				continue;
			}
			// eprintf("%d", i);
			break;
		}

		// 3.6 Circlecenter
		Double2 Circlecenter = (point_A - radius * normal_A);
		QCenter.push_back(Circlecenter);

		if (k < num_tr - 1)
			us += dus;
	}

	// 4. getCircle
	for (int i = 0; i < num_tr; i++)
	{
		// 4.1
		curve_Circle* cc = new curve_Circle;
		// 4.2
		cc->setRadius(QRadius[i]);
		cc->setCenter(QCenter[i]);
		// 4.3
		ICCurves.push_back(cc);
	}
	return ICCurves;
}

int InscribedCircleCalculator::calculateInscribedCircle(curve_Nurbs* c1, curve_Nurbs* c2, int num_tr, double tol)
{
	curve_Nurbs* hub = c1;
	curve_Nurbs* shroud = c2;

	int num_radius = 10000; // Number of radius iterations
	double dus = 1.0 / (num_tr - 1);
	int num_uh = 100;
	double us = 0;
	double radius = 0.;

	for (int k = 0; k < num_tr; k++)
	{
		// 1. get Point
		Double2 point_A = shroud->getPoint(us);
		Points_shroud.push_back(point_A);
		// 2. Tangential direction
		Double2 tangential_A = shroud->getTangent(us);
		// 3. mag
		double length = tangential_A.length();
		tangential_A /= length;
		// 4. normal
		Double2 normal_A = tangential_A.rotate(PI / 2.);
		// 5. radius
		double distance = (point_A - hub->getPoint(us)).length();
		radius = distance / 3;

		for (int i = 0; i < num_radius; i++)
		{
			// Number of intersections.
			int num_r = 0;
			double dl = 0.;
			double uh = 0.;
			// Circlecenter
			Double2 Circlecenter = (point_A - radius * normal_A);

			for (int j = 0; j <= num_uh; j++)
			{
				// get hub Point
				uh = 1. * j / num_uh;
				Double2 point_h = hub->getPoint(uh);
				// Distance between two points
				dl = (point_h - Circlecenter).length() - radius;
				if (dl < -tol)
					num_r += 1;
				if (num_r == 1)
					break;
				continue;
			}
			switch (num_r)
			{
			case 1:
				if (i == 0)
				{
					radius = distance / 10;
					continue;
				}
				Points_hub.push_back(hub->getPoint(uh));
				Qradius.push_back(radius);
				break;
			case 0:
				radius *= 1. + 50 * tol;
				continue;
			}
			// eprintf("%d", i);
			break;
		}

		// 5. Circlecenter
		Double2 Circlecenter = (point_A - radius * normal_A);
		CircleCenters.push_back(Circlecenter);

		if (k < num_tr - 1)
			us += dus;
	}
	return 0;
}

QVector<curve_Circle*> InscribedCircleCalculator::getInscribedCircle(curve_Nurbs& c1, curve_Nurbs& c2, int num_tr, double tol)
{
	// QVector<curve_Nurbs*> NCurves -> QVector<curve_Circle*> InscribedCircleCurves
	// 1.QVector
	QVector<curve_Circle*> ICCurves;
	QVector<Double2> QCenter;
	QVector<double> QRadius;

	// 2.Init
	int num_radius = 10000; // Number of radius iterations
	double dus = 1.0 / (num_tr - 1);
	int num_uh = 1E7 * tol;
	double us = 0;
	double radius = 0.;

	// 3. getCenters and Radius
	for (int k = 0; k < num_tr; k++)
	{
		// 3.1 get Point
		Double2 point_A = c2.getPoint(us);
		// 3.2 Tangential direction
		Double2 tangential_A = c2.getTangent(us);
		// 3.3 mag
		double length = tangential_A.length();
		tangential_A /= length;
		// 3.4 normal
		Double2 normal_A = tangential_A.rotate(PI / 2.);
		// 3.5 radius
		double distance = (point_A - c1.getPoint(us)).length();
		radius = distance / 3;

		for (int i = 0; i < num_radius; i++)
		{
			// Number of intersections.
			int num_r = 0;
			double dl = 0.;
			double uh = 0.;
			// Circlecenter
			Double2 Circlecenter = (point_A - radius * normal_A);

			for (int j = 0; j <= num_uh; j++)
			{
				// get hub Point
				uh = 1. * j / num_uh;
				Double2 point_h = c1.getPoint(uh);
				// Distance between two points
				dl = (point_h - Circlecenter).length() - radius;
				if (dl < -tol)
					num_r += 1;
				if (num_r == 1)
					break;
				continue;
			}
			switch (num_r)
			{
			case 1:
				if (i == 0)
				{
					radius = distance / 10;
					continue;
				}
				QRadius.push_back(radius);
				break;
			case 0:
				radius *= 1. + 50 * tol;
				continue;
			}
			// eprintf("%d", i);
			break;
		}

		// 3.6 Circlecenter
		Double2 Circlecenter = (point_A - radius * normal_A);
		QCenter.push_back(Circlecenter);

		if (k < num_tr - 1)
			us += dus;
	}

	// 4. getCircle
	for (int i = 0; i < num_tr; i++)
	{
		// 4.1
		curve_Circle* cc = new curve_Circle;
		// 4.2
		cc->setRadius(QRadius[i]);
		cc->setCenter(QCenter[i]);
		// 4.3
		ICCurves.push_back(cc);
	}
	return ICCurves;

}

// 1.getTopo/Curve
QStringList InscribedCircleCalculator::getAllTypeNames()
{
	QStringList allTypeNames = QStringList() << "ZR" << "Area";
	return allTypeNames;
}

QString InscribedCircleCalculator::getTypeName(int type)
{
	if (type < ZR)
		type = ZR;
	if (type >= CurveTypeEnd)
		type = CurveTypeEnd - 1;
	return getAllTypeNames()[type];
}

curve_Topology* InscribedCircleCalculator::getTopo(int type)
{
	// 1.
	QString typeName = getTypeName(type);
	curve_Topology* T = dynamic_cast<curve_Topology*>(child(typeName));

	// 2.
	if (!T)
		T = (curve_Topology*)TObject::new_object("curve_Topology", typeName, this);
	if(T)
		return T;
	else
		return nullptr;
}

curve_Curve* InscribedCircleCalculator::getCurve(int type)
{
	// 1.
	QString typeName = getTypeName(type);
	curve_Topology* T = getTopo(type);
	if (!T)
		return nullptr;
	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(typeName));
	// 2.
	if (!c)
		c = (curve_Curve*)TObject::new_object("curve_Curve", typeName, T);
	if (c)
		return c;
	return nullptr;
}

// 2.getCurve
QString InscribedCircleCalculator::getNurbsName(int type, int CurveID)
{
	QString NurbsName = "nurbs" + getTypeName(type) + QString::number(CurveID);
	return NurbsName;
}

curve_Nurbs* InscribedCircleCalculator::getNurbs(int type, int CurveID, bool createIfNotAvaiable)
{
	// 1.
	curve_Curve* Curve = getCurve(type);
	if (!Curve)
		return nullptr;
	QString NurbsName = getNurbsName(type, CurveID);
	curve_Curve* c = Curve->getCurveByName(NurbsName);

	// 2.
	if (!c && createIfNotAvaiable)
		c = Curve->addSegment(curve_Curve::Nurbs, NurbsName);
	if (c)
	{
		curve_Nurbs* s = dynamic_cast<curve_Nurbs*>(c);
		return s;
	}
	return nullptr;
}

QString InscribedCircleCalculator::getCircleName(int type, int CurveID)
{
	QString CircleName = "Circle" + getTypeName(type = 0)
		+ QString::number(CurveID);

	return CircleName;
}

curve_Circle* InscribedCircleCalculator::getCircle(int type, int CurveID, bool createIfNotAvaiable)
{
	// 1.
	curve_Curve* Curve = getCurve(type);
	if (!Curve)
		return nullptr;
	QString CircleName = getCircleName(type, CurveID);
	curve_Curve* c = Curve->getCurveByName(CircleName);
	if (!c && createIfNotAvaiable)
		c = Curve->addSegment(curve_Curve::Circle, CircleName);
	// 2.
	if (c)
	{
		curve_Circle* cc = dynamic_cast<curve_Circle*>(c);
		return cc;
	}
	return nullptr;
}

curve_Nurbs* InscribedCircleCalculator::getCenterCurve(int type, bool createIfNotAvaiable)
{
	// 1. T 
	curve_Curve* Curve = getCurve(type);
	if (!Curve)
		return nullptr;
	QString CenterCurvename = "CircleCenterCurve";
	curve_Curve* CenterCurve = Curve->getCurveByName(CenterCurvename);

	// 2.
	if (!CenterCurve && createIfNotAvaiable)
		CenterCurve = Curve->addSegment(curve_Curve::Nurbs, CenterCurvename);

	// 3.
	if (CenterCurve)
	{
		curve_Nurbs* ccc = dynamic_cast<curve_Nurbs*>(CenterCurve);
		return ccc;
	}
	else
		return nullptr;
}

curve_Nurbs* InscribedCircleCalculator::getAreaCurve(int type, bool createIfNotAvaiable)
{
	// 1. T 
	curve_Curve* Curve = getCurve(type);
	if (!Curve)
		return nullptr;
	QString AreaCurvename = "AreaCurve";
	curve_Curve* AreaCurve = Curve->getCurveByName(AreaCurvename);

	// 2.
	if (!AreaCurve && createIfNotAvaiable)
		AreaCurve = Curve->addSegment(curve_Curve::Nurbs, AreaCurvename);

	// 3.
	if (AreaCurve)
	{
		curve_Nurbs* ac = dynamic_cast<curve_Nurbs*>(AreaCurve);
		return ac;
	}
	else
		return nullptr;
}

// 3.setCurve
int InscribedCircleCalculator::setTNCurve(QVector<TNurbsCurve*> TNCurves)
{
	// TNurbsCurve -> Curve_Nurbs
	int error = -1;

	// 1.QVector
	QVector<curve_Nurbs*> CNCurves;
	for (int i = 0; i < TNCurves.size(); i++)
	{
		if (!TNCurves[i])
			return error;
		curve_Nurbs* c = new curve_Nurbs;

		// 2. transfer
		c->fillFromNurbsCurve(*TNCurves[i]);
		CNCurves.push_back(c);
	}

	error = setCNCurve(CNCurves);

	return error;
}

int InscribedCircleCalculator::setCNCurve(QVector<curve_Nurbs*> CNCurves)
{
	// NurbsCurve -> _ZRcurve
	int error = -1;
	if (CNCurves.size() < 2)
		return error;
	// 1
	for (int i = 0; i < CNCurves.size(); i++)
	{
		if (!CNCurves[i])
			return error;
		if (CNCurves[i]->getControlPointCount() < 2)
			return error;
	}

	// 2. get shroud/hub
	for (int i = 0; i < CNCurves.size(); i++)
	{
		if (curve_Nurbs* zrCurve = getNurbs(0, i))
			zrCurve->copyFrom(CNCurves[i]);
	}
	return 0;
}

int InscribedCircleCalculator::setCircleCurve(QVector<curve_Circle*> ICCurves)
{
	int error = -1;
	for (int i = 0; i < ICCurves.size(); i++)
	{
		curve_Circle* circle = getCircle(0, i);
		if (!circle)
			return error;
		else
		{
			circle->setRadius(ICCurves[i]->getRadius());
			circle->setCenter(ICCurves[i]->getCenter());
		}
	}
	return 0;
}

int InscribedCircleCalculator::setCenterLine(QVector<curve_Circle*> ICCurves)
{
	int error = -1;
	// 1.
	curve_Nurbs* ccc = getCenterCurve(0);
	QVector<Double2>Centers;
	// 2.
	for (int i = 0; i < ICCurves.size(); i++)
	{
		Centers.push_back(ICCurves[i]->getCenter());
	}
	if (!ccc)
		return error;
	else
		ccc->fitBezier(Centers);
	return 0;
}

int InscribedCircleCalculator::calculateArea()
{
	int error = -1;
	if (CircleCenters.size() == 0)
		return error;

	for (int i = 0; i < Qradius.size(); i++)
	{
		// 1. b
		Double2 Point_A = Points_shroud[i];
		Double2 Point_B = Points_hub[i];
		double s = (Point_A - Point_B).length(); // AB Chord length
		double p = Qradius[i]; // radius of the tangent circle
		double b = 2. / 3 * (s + p); // AEB Arc length

		// 2. Rc
		Double2 Point_D = 0.5 * (Point_A + Point_B);
		Double2 Point_C = Point_D + (1. / 3) * (CircleCenters[i] - Point_D);
		double Rc = Point_C[1]; // The radius of the axis of C

		// 3. L/F
		double L = 1. * i / (Qradius.size() - 1) * l_cc; // i-Length of center line
		double F = 2. * PI * Rc * b; // The CrossSectionArea
		Double2 l_area_i = { L,F };
		L_Area.push_back(l_area_i);
	}
	return 0;
}

int InscribedCircleCalculator::createAreaLine()
{
	int error = -1;
	curve_Nurbs* ac = getAreaCurve(1);
	if (!ac)
		return error;
	else
		ac->fitBezier(L_Area);
	return 0;
}

// 4. show
int InscribedCircleCalculator::LoadCurves()
{
	int error = -1;
	curve_Nurbs* hub = (getNurbs(0, 0));
	curve_Nurbs* shroud = (getNurbs(0, 1));
	if (!hub || !shroud)
		return error;

	QVector<curve_Circle*> ICCurves = getInscribedCircle(*hub, *shroud, n_tr);
	error = setCircleCurve(ICCurves);
	error = setCenterLine(ICCurves);
	return 0;
}

int InscribedCircleCalculator::newLoadCurves()
{
	int error = -1;
	curve_Nurbs* c1 = (getNurbs(0, 0));
	curve_Nurbs* c2 = (getNurbs(0, 1));
	if (!c1 || !c2)
		return error;

	QVector<curve_Circle*> ICCurves = getInscribedCircle(c1, c2, n_tr);
	error = setCircleCurve(ICCurves);
	error = setCenterLine(ICCurves);
	return 0;
}

int InscribedCircleCalculator::newCalculateCrossSection()
{
	int error = -1;
	// 1.
	CircleCenters.clear();
	Points_hub.clear();
	Points_shroud.clear();
	Qradius.clear();
	L_Area.clear();
	// 2.
	curve_Nurbs* c1 = (getNurbs(0, 0));
	curve_Nurbs* c2 = (getNurbs(0, 1));
	if (!c1 || !c2)
		return error;
	// 3.
	error = calculateInscribedCircle(c1, c2);
	// 4.
	curve_Nurbs* ccc = getCenterCurve(0);
	ccc->fitBezier(CircleCenters);
	l_cc = ccc->getLength();
	// 5.
	error = calculateArea();
	error = createAreaLine();
	return 0;
}

// 5.create InscribedCircleCalculator
int InscribedCircleCalculator::createInscribedCircle()
{
	int error = 0;

	return error;
}

InscribedCircleCalculator::~InscribedCircleCalculator()
{
}

