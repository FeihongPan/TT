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
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TaiZe Inc.
 */

#include "1d_BladeEditor.h"
#include "curve_Topology.h"
#include "curve_Curve.h"
#include "curve_Nurbs.h"
#include "curve_Spline.h"
#include "curve_Line.h"

#include "1d_Vaned.h"
#include "profileGenerator.h"

REGISTER_OBJECT_CLASS(T1dBladeEditor, "Blade editor", TObject);
T1dBladeEditor::T1dBladeEditor(QString object_n, TObject* iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;
  DEFINE_SCALAR_INIT(double, o1tip, 0, 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, o1mean, 0, 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, o1hub, 0, 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, ThroatArea, 0, 0, NULL, TUnit::area);
  _pVaned = nullptr;
}

bool T1dBladeEditor::isHasError(int errorCode)
{
  if (errorCode == 0)
    return false;
  return true;
}

curve_Topology* T1dBladeEditor::getBladeAngleCurvesTopo(bool createIfNotAvailable)
{
  QString typeName = "BladeAngleCurvesTopo";
  curve_Topology* T = dynamic_cast<curve_Topology*>(child(typeName));
  if (!T && createIfNotAvailable)
    T = (curve_Topology*)TObject::new_object("curve_Topology", typeName, this);
  if (T)
    return T;
  return nullptr;
}

curve_Topology* T1dBladeEditor::getBladeThicknessCurvesTopo(bool createIfNotAvailable)
{
  QString typeName = "BladeThicknessCurvesTopo";
  curve_Topology* T = dynamic_cast<curve_Topology*>(child(typeName));
  if (!T && createIfNotAvailable)
    T = (curve_Topology*)TObject::new_object("curve_Topology", typeName, this);
  if (T)
    return T;
  return nullptr;
}

curve_Topology* T1dBladeEditor::getBladeConformalMappingCurvesTopo(bool createIfNotAvailable)
{
  QString typeName = "ConformalMappingCurvesTopo";
  curve_Topology* T = dynamic_cast<curve_Topology*>(child(typeName));
  if (!T && createIfNotAvailable)
    T = (curve_Topology*)TObject::new_object("curve_Topology", typeName, this);
  if (T)
    return T;
  return nullptr;
}

curve_Topology* T1dBladeEditor::getZRCurvesTopo(bool createIfNotAvailable)
{
  QString typeName = "ZRCurvesTopo";
  curve_Topology* T = dynamic_cast<curve_Topology*>(child(typeName));
  if (!T && createIfNotAvailable)
    T = (curve_Topology*)TObject::new_object("curve_Topology", typeName, this);
  if (T)
    return T;
  return nullptr;
}

curve_Curve* T1dBladeEditor::getZRCurve(double span)
{
  QString name = getSpanString(span);
  curve_Topology* T = getZRCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Curve* T1dBladeEditor::getZRBladeEdge()
{
  QString name = "Blade edge";
  curve_Topology* T = getZRCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Curve* T1dBladeEditor::getBladeAngleCurves()
{
  QString name = "BladeAngleCurve";
  curve_Topology* T = getBladeAngleCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Curve* T1dBladeEditor::getBladeThicknessCurves()
{
  QString name = "BladeThicknessCurve";
  curve_Topology* T = getBladeThicknessCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Curve* T1dBladeEditor::getBladeConformalMappingCurves()
{
  QString name = "MTheta";
  curve_Topology* T = getBladeConformalMappingCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Curve* T1dBladeEditor::getBladeConformalMappingAssistingCurves(double span)
{
  QString name = getSpanString(span)+ "-Assiting";
  curve_Topology* T = getBladeConformalMappingCurvesTopo();
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (c)
    return c;
  c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  return c;
}

curve_Nurbs* T1dBladeEditor::getZRNurbsFlowPath(double span, bool createIfNotAvailable)
{
  QString name = "ZR";

  curve_Curve* c = getZRCurve(span)->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getZRCurve(span)->addSegment(curve_Curve::Nurbs, name);

  if (c)
  {
    curve_Nurbs* s = dynamic_cast<curve_Nurbs*>(c);
    return s;
  }
  return nullptr;
}

curve_Spline* T1dBladeEditor::getZRSplineBladeEdge(int bladeEdgeType, bool createIfNotAvailable)
{
  QString name = getBladeEdgeString(bladeEdgeType);

  curve_Curve* c = getZRBladeEdge()->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getZRBladeEdge()->addSegment(curve_Curve::Spline, name);

  if (c)
  {
    curve_Spline* s = dynamic_cast<curve_Spline*>(c);
    return s;
  }
  return nullptr;
}

curve_Nurbs* T1dBladeEditor::getBladeAngleNurbsCurve(double span, int merdionalType, int bladeAngleType, bool createIfNotAvailable)
{
  QString name = getBladeAngleCurveNameString(span, merdionalType, bladeAngleType);

  curve_Curve* c = getBladeAngleCurves()->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getBladeAngleCurves()->addSegment(curve_Curve::Nurbs, name);

  if (c)
  {
    curve_Nurbs* s = dynamic_cast<curve_Nurbs*>(c);
    return s;
  }
  return nullptr;
}

curve_Nurbs* T1dBladeEditor::getMThetaNurbsCurve(double span, bool createIfNotAvailable)
{
  QString name = "MTheta-"+getSpanString(span);

  curve_Curve* c = getBladeConformalMappingCurves()->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getBladeConformalMappingCurves()->addSegment(curve_Curve::Nurbs, name);

  if (c)
  {
    curve_Nurbs* s = dynamic_cast<curve_Nurbs*>(c);
    return s;
  }
  return nullptr;
}

curve_Nurbs* T1dBladeEditor::getBladeThicknessNurbsCurve(double span, int merdionalType, int bladeThicknessType, bool createIfNotAvailable)
{
  QString name = getBladeThicknessCurveNameString(span, merdionalType, bladeThicknessType);

  curve_Curve* c = getBladeThicknessCurves()->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getBladeThicknessCurves()->addSegment(curve_Curve::Nurbs, name);

  if (c)
  {
    curve_Nurbs* s = dynamic_cast<curve_Nurbs*>(c);
    return s;
  }
  return nullptr;

}

curve_Spline* T1dBladeEditor::getStackingSpline(bool createIfNotAvailable)
{
  QString name = "stacking curve";

  curve_Curve* c = getBladeConformalMappingCurves()->getCurveByName(name);

  if (!c && createIfNotAvailable)
    c = getBladeConformalMappingCurves()->addSegment(curve_Curve::Spline, name);

  if (c)
  {
    curve_Spline* s = dynamic_cast<curve_Spline*>(c);
    return s;
  }
  return nullptr;
}

QString T1dBladeEditor::getBladeAngleCurveNameString(double span, int merdionalType, int bladeAngleType)
{
  QString name;
  name += getMeridionalTypeString(merdionalType);

  name += "-";

  name += getBladeAngleTypeString(merdionalType);

  name += "-";

  name += getSpanString(span);

  return name;
}

QString T1dBladeEditor::getBladeThicknessCurveNameString(double span, int merdionalType, int bladeThicknessType)
{
  QString name;
  name += getMeridionalTypeString(merdionalType);

  name += "-";

  name += getBladeThicknessTypeString(merdionalType);

  name += "-";

  name += getSpanString(span);

  return name;
}


profileGenerator* T1dBladeEditor::getProfileGenerator(double span, bool createIfNotAvailable)
{
  QString name = getSpanString(span);
  profileGenerator* profile = dynamic_cast<profileGenerator*>(child(name));
  if (!profile && createIfNotAvailable)
    profile = (profileGenerator*)TObject::new_object("profileGenerator", name, this);
  if (profile)
    return profile;
  return nullptr;
}

int T1dBladeEditor::updateProfilesGenerator()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];

    errorCode = updateProfileGenerator(span);
    if (isHasError(errorCode))
      return errorCode;
  }

  return 0;
}

int T1dBladeEditor::updateProfileGenerator(double span)
{
  int errorCode = -1;

  // 1.
  profileGenerator* profile = getProfileGenerator(span);
  if (!profile)
    if (isHasError(errorCode)) 
      return errorCode;

  QVector<curve_Nurbs*> parameterCurves;
  // 2.1
  curve_Nurbs* zrCurve = getZRNurbsFlowPath(span, false);
  if (!zrCurve)
    if (isHasError(errorCode)) 
      return errorCode;
  parameterCurves.push_back(zrCurve);

  // 2.2
  curve_Nurbs* metalBladeAngle = getBladeAngleNurbsCurve(span,0,0, false);
  if (!metalBladeAngle)
    if (isHasError(errorCode))
      return errorCode;
  curve_Nurbs* metalBladeAngle_rad = new curve_Nurbs;

  metalBladeAngle_rad->copyFrom(metalBladeAngle);

  double toRad = PI / 180.;
  scaling(metalBladeAngle_rad, 1, toRad);

  parameterCurves.push_back(metalBladeAngle_rad);

  // 2.3
  curve_Nurbs* metalBladeThickness = getBladeThicknessNurbsCurve(span, false);
  if (!metalBladeThickness)
    if (isHasError(errorCode))
      return errorCode;
  parameterCurves.push_back(metalBladeThickness);

  // 2.4
  QVector<double> parameterValues = QVector<double> () << getUofEdgeOnFlowPath(span, 0) << getUofEdgeOnFlowPath(span, 1);

  // 2.5
  QVector<double> parameterValues_Stacking = getStackingParameters(span);

  // 3.
  errorCode = profile->setProfileGenerator2(parameterCurves, parameterValues, parameterValues_Stacking);


  delete metalBladeAngle_rad;

  if (isHasError(errorCode))
    return errorCode;

  // 4.
  errorCode = profile->createProfile();
  if (isHasError(errorCode))
    return errorCode;

  return 0;
}

QVector<QVector<Double3>> T1dBladeEditor::getBladeSurface(QString SurfaceName, int spansInterpolated)
{
  int errorCode = -1;
  QVector<QVector<Double3>> surface_source;

  auto getBladeSurfaceType = [&](QString surfaceName)->int
  {
    int surfaceType = 0;
    if (surfaceName.contains("Camber"))
      surfaceType = profileGenerator::CamberCurve;
    else if (surfaceName.contains("LE"))
      surfaceType = profileGenerator::LECurve;
    else if (surfaceName.contains("Pressure"))
      surfaceType = profileGenerator::LeftCurve;
    else if (surfaceName.contains("Suction"))
      surfaceType = profileGenerator::RightCurve;
    else if (surfaceName.contains("TE"))
      surfaceType = profileGenerator::TECurve;

    return surfaceType;
  };

  // 1.
  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];
    profileGenerator* profile = getProfileGenerator(span);
    if (!profile) isHasError(errorCode);

    int surfaceType = getBladeSurfaceType(SurfaceName);

    QVector<Double3>pts = profile->get3DOfProfileCurve(surfaceType);
    surface_source.push_back(pts);
  }

  int np = (surface_source.first()).size();

  // 2.
  QVector<QVector<Double3>> surface;
  for (int i = 1; i < spansInterpolated - 2; i++)
  {
    QVector<Double3> interpolated;
    int t = i / (spansInterpolated - 2);
    for (int j = 0; j<np; j++)
    {
      Double3 pt = (1-t)* (surface_source.first())[j] + t* (surface_source.last())[j];
      interpolated.push_back(pt);
    }
    surface.push_back(interpolated);
  }

  surface.push_back(surface_source.last());
  surface.push_front(surface_source.at(1));
  surface.push_front(surface_source.first());

  return surface;
}

QVector<QVector<Double3>> T1dBladeEditor::getBladeSurface_rotate(QString SurfaceName, int spansInterpolated)
{
  int errorCode = -1;
  QVector<QVector<Double3>> surface_source;

  auto getBladeSurfaceType = [&](QString surfaceName)->int
  {
    int surfaceType = 0;
    if (surfaceName.contains("Camber"))
      surfaceType = profileGenerator::CamberCurve;
    else if (surfaceName.contains("LE"))
      surfaceType = profileGenerator::LECurve;
    else if (surfaceName.contains("Pressure"))
      surfaceType = profileGenerator::LeftCurve;
    else if (surfaceName.contains("Suction"))
      surfaceType = profileGenerator::RightCurve;
    else if (surfaceName.contains("TE"))
      surfaceType = profileGenerator::TECurve;

    return surfaceType;
  };

  // 1.
  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    int n = 20;//num mainblade
    double rotate = 360/n;
    double span = spans[i];
    profileGenerator* profile = getProfileGenerator(span);
    if (!profile) isHasError(errorCode);

    int surfaceType = getBladeSurfaceType(SurfaceName);

    QVector<Double3>pts = profile->get3DOfProfileCurve(surfaceType);
    QVector<Double3>ptsRotate;ptsRotate.resize(51);
    for (int i = 0;i < pts.size();i++)
    {
      ptsRotate[i][0] = pts[i][0] * cos(rotate) - pts[i][1] * sin(rotate);
      ptsRotate[i][1] = pts[i][0] * sin(rotate) + pts[i][1] * cos(rotate);
      ptsRotate[i][2] = pts[i][2];
    }

    surface_source.push_back(ptsRotate);
  }

  int np = (surface_source.first()).size();

  // 2.
  QVector<QVector<Double3>> surface;
  for (int i = 1; i < spansInterpolated - 2; i++)
  {
    QVector<Double3> interpolated;
    int t = i / (spansInterpolated - 2);
    for (int j = 0; j < np; j++)
    {
      Double3 pt = (1 - t) * (surface_source.first())[j] + t * (surface_source.last())[j];
      interpolated.push_back(pt);
    }
    surface.push_back(interpolated);
  }

  surface.push_back(surface_source.last());
  surface.push_front(surface_source.at(1));
  surface.push_front(surface_source.first());

  return surface;
}


QVector<QVector<Double3>> T1dBladeEditor::getRevolutionSurface(double span)
{
  QVector<QVector<Double3>> revolutionSurface;

  curve_Nurbs* flowPath = getZRNurbsFlowPath(span, false);

  if (!flowPath)
  {
    isHasError(-1);
    return revolutionSurface;
  }

  double ule = 0.; // getUofEdgeOnFlowPath(span, LE)
  double ute = 1.; // getUofEdgeOnFlowPath(span, TE)

  double du = (ute - ule) / (_numPTS - 1.);

  // I. ZR 
  QVector<Double2> ZRs;
  for (int i = 0; i < _numPTS; i++)
  {
    double u = ule + du * i;
    Double2 zr = flowPath->getPoint(u);
    ZRs.push_back(zr);
  }

  int nSliceTheta = 361;
  double dTheta = 2 * PI / (nSliceTheta - 1);

  for (int i = 0; i < nSliceTheta; i++)
  {
    QVector<Double3> data_thetaSlice;
    QVector<double> value_thetaSlice;
    double theta = i * dTheta;
    for (int j = 0; j < _numPTS; j++)
    {
      double r = ZRs[j][1];
      double x = r * cos(theta);
      double y = r * sin(theta);
      double z = ZRs[j][0];
      data_thetaSlice.push_back(Double3(x, y, z));
      value_thetaSlice.push_back(0.);
    }
    revolutionSurface.push_back(data_thetaSlice);
  }

  return revolutionSurface;
}

QString T1dBladeEditor::getMeridionalTypeString(int merdionalType)
{
  QString name;
  if (merdionalType == meridionallength)
  {
    name = "meridionalLength";
  }
  else if(merdionalType == meridionalTransformation)
  {
    name = "meridionalTransformation";
  }
  else
  {
    name = "meridionalPercent";
  }
  return name;
}

QString T1dBladeEditor::getSpanString(double span)
{
  QString name = "span" + QString("-%1").arg((int)((span + 1.e-6)));
  return name;
}

QString T1dBladeEditor::getBladeEdgeString(int bladeEdgeType)
{
  QString name;
  if (bladeEdgeType == TE)
    name = "TE";
  else
    name = "LE";
  return name;
}

QString T1dBladeEditor::getBladeAngleTypeString(int bladeAngleType)
{
  QString name;
  if (bladeAngleType == 0)
    name = "BetaAngle";
  else
    name = "ThetaAngle";
  return name;
}

QString T1dBladeEditor::getBladeThicknessTypeString(int bladeThicknessType)
{
  QString name;
  if (bladeThicknessType == transformalThickness)
    name = "transformalThickness";
  else
    name = "metalThickness";
  return name;
}

void T1dBladeEditor::unitization(curve_Nurbs* s, int whichIndex)
{
  if (!s)
    return;

  QVector<Double2> controlPoints = s->getControlPointsDouble2();

  double start = (controlPoints.first())[0];
  if (whichIndex != 0)
    start = (controlPoints.first())[1];

  double end = (controlPoints.last())[0];
  if (whichIndex != 0)
    end = (controlPoints.last())[1];

  double total = end - start;

  for (int i = 0; i < controlPoints.size(); i++)
  {
    if (whichIndex == 0)
    {
      controlPoints[i][0] -= start;
      controlPoints[i][0] /= total;
    }
    else
    {
      controlPoints[i][1] -= start;
      controlPoints[i][1] /= total;
    }
  }

  s->setControlPoints(controlPoints);
}

void T1dBladeEditor::scaling(curve_Nurbs* s, int whichIndex, double scale)
{
  if (!s)
    return;

  QVector<Double2> controlPoints = s->getControlPointsDouble2();
  for (int i = 0; i < controlPoints.size(); i++)
  {
    if (whichIndex == 0)
      controlPoints[i][0] *= scale;
    else
      controlPoints[i][1] *= scale;
  }

  s->setControlPoints(controlPoints);
}

double T1dBladeEditor::getRad(double deg)
{
  return deg / 180. * PI;
}

double T1dBladeEditor::getDegree(double rad)
{
  return rad * 180. / PI;
}

int T1dBladeEditor::initialBladeEditor(QVector<double> spans)
{
  int errorCode = -1;
  if (!_pVaned)
    return errorCode;

  setSpans(spans);

  // 1.
  errorCode = setZRCurves();
  if(isHasError(errorCode))  return errorCode;

  // 2.
  errorCode = setBladeEdgeCurves();
  if (isHasError(errorCode))  return errorCode;

  // 3.
  errorCode = setBladeAngleCurves();
  if (isHasError(errorCode))  return errorCode;

  // 4.
  errorCode = setBladeThicknessCurves();
  if (isHasError(errorCode))  return errorCode;


  errorCode = updateBladeEditor();
  if (isHasError(errorCode))  return errorCode;

  return errorCode;
}

int T1dBladeEditor::setVaned(T1dVaned* pVaned)
{
  int errorCode = -1;
  if (!pVaned)
  {
    isHasError(-1);
    return errorCode;
  }

  _pVaned = pVaned;
  return 0;
}

int T1dBladeEditor::updateBladeEditor()
{
  int errorCode = -1;

  errorCode = updateMerdional();

  errorCode = updateStacking();

  if (_bladeEditorType == bladeMetalAngleAndThickness)
  {
    errorCode = updateBladeMetal();
  }
  else
  {
    errorCode = updateBladeConformalMapping();
  }

  errorCode = updateProfilesGenerator();

  return errorCode;
}

int T1dBladeEditor::updateMerdional()
{
  int errorCode = -1;

  errorCode = updateMThetaCurvesByMerdionalCurvesChanged();

  return errorCode;
}

int T1dBladeEditor::updateBladeMetal()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];

    errorCode = calculateBladeConformalMapping(span);
    if (isHasError(errorCode))
      return errorCode;
  }

  return errorCode;
}

int T1dBladeEditor::updateBladeConformalMapping()
{
  int errorCode = -1;
  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];

    errorCode = calculateBladeMetalAngleCurve(span);
    if (isHasError(errorCode))
      return errorCode;
  }

  return errorCode;
}

int T1dBladeEditor::calculateBladeMetalAngleCurve(double span)
{
  int errorCode = -1;

  curve_Nurbs* flowPath = getZRNurbsFlowPath(span, false);
  curve_Nurbs* MThetaCurve = getMThetaNurbsCurve(span, false);

  if (!flowPath || !MThetaCurve)
  {
    isHasError(errorCode);
    return errorCode;
  }

  double ule = getUofEdgeOnFlowPath(span, LE);
  double ute = getUofEdgeOnFlowPath(span, TE);

  double du = (ute - ule) / (_numPTS - 1.);

  // I. ZR 
  QVector<Double2> ZRs;
  QVector<double> ms;

  for (int i = 0; i < _numPTS; i++)
  {
    double u = ule + du * i;
    // 1.1
    Double2 zr = flowPath->getPoint(u);
    ZRs.push_back(zr);

    // 1.2
    double m = flowPath->calculateLength(ule, u);
    ms.push_back(m);
  }

  // 3.
  QVector<double> Ms;
  Ms.push_back(0.);
  for (int i = 1; i < _numPTS; i++)
  {
    double R_avg = (ZRs[i - 1][1] + ZRs[i][1]) / 2.;

    double dm = (ms[i] - ms[i - 1]);

    // 3.1
    double dM = dm / R_avg;
    double M = Ms.last() + dM;
    Ms.push_back(M);
  }

  double M_calculed = calculateBladeMerdionalTranformalCurve(span, ule, ute);
  QVector<Double2> CPs = MThetaCurve->getControlPointsDouble2();
  double M_now = (CPs.last() - CPs.first())[0];

  // 4.
  double M_end = 0.;
  Double2 MAxis = {1., 0.};

  QVector<double> betas;
  for (int i = 0; i < _numPTS; i++)
  {
    double M = Ms[i];
    QVector<double> us = MThetaCurve->getUFromX(M);
    if (us.size() == 0)
    {
      if (M > M_end)
        us.push_back(1.);
      else if (M < 0.)
        us.push_back(0.);
      else
        continue;
    }

    Double2 T = MThetaCurve->getTangent(us.first());
    double angle = MAxis.angleSigned(T);

    betas.push_back(angle);
  }

  // 5.
  double flowPathLength = ms.last();

  QVector<Double2> mPerBetas;
  for (int i = 0; i < _numPTS; i++)
  {
    double mPer = ms[i]/ flowPathLength;
    double beta_deg = getDegree(betas[i]);
    mPerBetas.push_back(Double2(mPer, beta_deg));
  }

  // 6.
  int numCP = 6;
  curve_Nurbs* metalBladeAngle = getBladeAngleNurbsCurve(span, false);
  if (metalBladeAngle)
    numCP = metalBladeAngle->getControlPointCount();
  else
    metalBladeAngle = getBladeAngleNurbsCurve(span, true);

  if (numCP < 4)
    numCP = 6;

  metalBladeAngle->fitBezier(mPerBetas, numCP);

  return 0;
}

int T1dBladeEditor::calculateBladeConformalMapping(double span)
{
  int errorCode = -1;

  curve_Nurbs* flowPath = getZRNurbsFlowPath(span, false);
  curve_Nurbs* metalBladeAngle = getBladeAngleNurbsCurve(span, false);

  if (!flowPath || !metalBladeAngle)
  {
    isHasError(errorCode);
    return errorCode;
  }

  double ule = getUofEdgeOnFlowPath(span, LE);
  double ute = getUofEdgeOnFlowPath(span, TE);

  double du = (ute - ule) / (_numPTS - 1.);

  // I. ZR 
  QVector<Double2> ZRs;
  QVector<double> ms;

  for (int i = 0; i< _numPTS; i++)
  {
    double u = ule + du * i;

    // 1.1
    Double2 zr = flowPath->getPoint(u);
    ZRs.push_back(zr);

    // 1.2
    double m = flowPath->calculateLength(ule, u);
    ms.push_back(m);
  }

  // 2. 
  double flowPathLength = ms.last();
  QVector<double> mPerencts;

  QVector<double> Betas;
  for (int i = 0; i < _numPTS; i++)
  {
    // 2.1
    double mPercent = ms[i]/ flowPathLength;
    mPerencts.push_back(mPercent);

    // 2.2
    QVector<double> us = metalBladeAngle->getUFromX(mPercent);
    if (us.size() == 0)
    {
      isHasError(errorCode);
      return errorCode;
    }

    Double2 mBeta = metalBladeAngle->getPoint(us.first());

    double beta = getRad(mBeta[1]);

    Betas.push_back(beta);
  }

  // 3.
  QVector<double> Ms;
  Ms.push_back(0.);

  QVector<double> Thetas;
  Thetas.push_back(0.);

  for (int i = 1; i < _numPTS; i++)
  {
    double R_avg = (ZRs[i - 1][1] + ZRs[i][1]) / 2.;

    double beta_avg = (Betas[i - 1] + Betas[i]) / 2.;

    double dm = (ms[i]- ms[i - 1]);

    // 3.1
    double dM = dm / R_avg;
    double M = Ms.last() + dM;
    Ms.push_back(M);

    // 3.2
    double dTheta = dM * tan(beta_avg);
  //  double theta = Thetas.last() + getDegree(dTheta);
    double theta = Thetas.last() + dTheta;

    Thetas.push_back(theta);
  }

  // 4.
  QVector<Double2> MThetas;
  for (int i = 0; i < _numPTS; i++)
  {
    Double2 MTheta = { Ms[i], Thetas[i]};
    MThetas.push_back(MTheta);
  }

  // 5.
  int numCP = 6;
  curve_Nurbs* MThetaCurve = getMThetaNurbsCurve(span, false);
  if (MThetaCurve)
    numCP = MThetaCurve->getControlPointCount();
  else
    MThetaCurve = getMThetaNurbsCurve(span, true);

  if (numCP < 5)
    numCP = 6;

  MThetaCurve->fitBezier(MThetas, numCP);
 
  return 0;
}

double T1dBladeEditor::calculateBladeMerdionalTranformalCurve(double span, double uStart,double uEnd)
{
  int errorCode = -1;

  double M = 0.;
  curve_Nurbs* zrCurve = getZRNurbsFlowPath(span, false);
  if (!zrCurve)
  {
    isHasError(errorCode);
    return M;
  }

  double du = (uEnd - uStart) / (_numPTS-1.);

  Double2 zr_last = zrCurve->getPoint(uStart);
  Double2 zr_now;
  
  for (int i = 1; i< _numPTS; i++)
  {
    double u = uStart+i * du;
    zr_now = zrCurve->getPoint(u);
    double dm = zrCurve->calculateLength(u-du, u);
    double r_avg = (zr_last[1] + zr_now[1]) / 2.;
    double dM = dm / r_avg;
    M += dM;

    zr_last = zr_now;
  }
 
  return M;
}

double T1dBladeEditor::getUofEdgeOnFlowPath(double span, int edgeType)
{

  curve_Spline* bladeEdge = getZRSplineBladeEdge(edgeType);
  curve_Nurbs* flowPath = getZRNurbsFlowPath(span);
  int index = getSpanIndexFromSpans(span);

  QVector<Double2> CPs = bladeEdge->getControlPointsDouble2();

  double u = 0.;
  if (edgeType == 1)
    u = 1.;
  if (!bladeEdge || !flowPath || (index > CPs.size()-1))
  {
    isHasError(-1);
    return u;
  }

  Double2 pos = CPs[index];

  QVector<double> us = flowPath->getUFromXY(pos);
  if (us.size() > 0)
    u = us.first();

  return u;
}

double T1dBladeEditor::getBladeMeridionalLength(double span)
{
  double ule = getUofEdgeOnFlowPath(span, LE);
  double ute = getUofEdgeOnFlowPath(span, TE);

  curve_Nurbs* flowPath = getZRNurbsFlowPath(span);
  if (!flowPath)
  {
    isHasError(-1);
    return 1.;
  }
  double L = flowPath->calculateLength(ule, ute);

  return L;
}

double T1dBladeEditor::getTransformalMeridional(double span)
{

  return 0.;
}

int T1dBladeEditor::getStackingType()
{
  return _stackingType;
}

void T1dBladeEditor::setStackingType(int stackingType)
{
  if(stackingType == stackingTypes::LE_Stacking)
    _stackingType = stackingTypes::LE_Stacking;

  else if (stackingType == stackingTypes::TE_Stacking)
    _stackingType = stackingTypes::TE_Stacking;

  else
  {
    ; // todo
  }
}

int T1dBladeEditor::updateStacking()
{

  return 0;
}



Double2 T1dBladeEditor::getStackingPosition()
{
  Double2 Pt;

  return Pt;
}

QVector<double> T1dBladeEditor::getStackingParameters(double span)
{
  // 1.
  double stackingType = int(getStackingType());
  // 2.
  double baseTheta_Stacking = getBaseTheta_Stacking();
  // 3.
  // 3.1 
  double theta = getTheta_Stacking(span);
  // 3.2 
  double offsetTheta = theta - baseTheta_Stacking;

  // 4. set
  QVector<double> stackingParameters;

  stackingParameters.push_back(stackingType);

  stackingParameters.push_back(baseTheta_Stacking);

  stackingParameters.push_back(offsetTheta);

  return stackingParameters;
}

double T1dBladeEditor::getBaseTheta_Stacking()
{
  double baseTheta_Stacking = 0.;
  curve_Nurbs* MTheta_span0 = getMThetaNurbsCurve(0., false);
  if (!MTheta_span0)
  {
    return 0.;
  }

  if (getStackingType() == LE)
  {
    baseTheta_Stacking = (MTheta_span0->getControlPointPosition(0))[1];
  }
  else
  {
    int num = MTheta_span0->getControlPointCount();
    baseTheta_Stacking = (MTheta_span0->getControlPointPosition(num-1))[1];
  }
  return baseTheta_Stacking;
}

double T1dBladeEditor::getTheta_Stacking(double span)
{
  double theta_Stacking = 0.;
  curve_Nurbs* MTheta_span = getMThetaNurbsCurve(span, false);
  if (!MTheta_span)
  {
    return 0.;
  }
  if (getStackingType() == LE)
  {
    theta_Stacking = (MTheta_span->getControlPointPosition(0))[1];
  }
  else
  {
    int num = MTheta_span->getControlPointCount();
    theta_Stacking = (MTheta_span->getControlPointPosition(num - 1))[1];
  }
  return theta_Stacking;
}

double T1dBladeEditor::getTheta(double span, int bladeEdge)
{
  double theta = 0.;
  curve_Nurbs* MTheta_span = getMThetaNurbsCurve(span, false);
  if (!MTheta_span)
  {
    return 0.;
  }
  if (bladeEdge == LE)
  {
    theta = (MTheta_span->getControlPointPosition(0))[1];
  }
  else
  {
    int num = MTheta_span->getControlPointCount();
    theta = (MTheta_span->getControlPointPosition(num - 1))[1];
  }
  return theta;
}

double T1dBladeEditor::getBladeAngle(double span, int bladeEdge)
{
  double beta = 0.;
  curve_Nurbs* metaBeta = getBladeAngleNurbsCurve(span);
  if (!metaBeta)
  {
    return 0.;
  }
  if (bladeEdge == LE)
  {
    beta = (metaBeta->getControlPointPosition(0))[1];
  }
  else
  {
    int num = metaBeta->getControlPointCount();
    beta = (metaBeta->getControlPointPosition(num - 1))[1];
  }

  beta = getRad(beta);
  return beta;
}

double T1dBladeEditor::getWrapAngle(double span)
{
  curve_Nurbs* MTheta_span = getMThetaNurbsCurve(span, false);
  if (!MTheta_span)
  {
    return 0.;
  }
  double wrapAngle = (MTheta_span->getPoint(1.) - MTheta_span->getPoint(0.))[1];

  return wrapAngle;
}

double T1dBladeEditor::getBladeThickness(double span, int bladeEdge)
{
  double thickness = 0.;
  curve_Nurbs* metaThickness = getBladeThicknessNurbsCurve(span);
  if (!metaThickness)
  {
    return 0.;
  }
  if (bladeEdge == LE)
  {
    thickness = (metaThickness->getControlPointPosition(0))[1];
  }
  else
  {
    int num = metaThickness->getControlPointCount();
    thickness = (metaThickness->getControlPointPosition(num - 1))[1];
  }
  return thickness;
}

Double2 T1dBladeEditor::getMaxThicknessPosition(double span)
{
  Double2 pos = {0.,0.};
  curve_Nurbs* metaThickness = getBladeThicknessNurbsCurve(span);
  if (!metaThickness)
  {
    return pos;
  }

  pos = metaThickness->getPoint(0.);

  int np = 51;

  double du = 1. / (np-1.);
  for (int i = 1; i< np; i++)
  {
    double u = du * i;
    Double2 pos_now = metaThickness->getPoint(u);
    if (pos_now[1] > pos[1])
      pos = pos_now;
  }

  return pos;
}

double T1dBladeEditor::getThetaFromStackingCurve(double span)
{
  curve_Spline* stacking = getStackingSpline(false);
  
  int size = stacking->getControlPointCount();
  if (size != getSpanSize())
  {
    isHasError(-1);
    return -1.;
  }

  QVector<Double2> pts = stacking->getControlPointsDouble2();
  int spanIndex = getSpanIndex(span);

  double theta = pts[spanIndex][1] - pts[0][1];

  return theta;
}

QVector<double> T1dBladeEditor::getSpans()
{
  return _spans;
}

void T1dBladeEditor::setSpans(QVector<double> span)
{
  if (span.size() >= 2)
    _spans = span;
}

int T1dBladeEditor::getSpanSize()
{
  return _spans.size();
}

int T1dBladeEditor::setZRCurves()
{
  int errorCode = -1;

  errorCode = setFlowPathCurves();
  if (isHasError(errorCode))
    return errorCode;

  errorCode = setBladeEdgeCurves();
  if (isHasError(errorCode))
    return errorCode;

  return errorCode;
}

int T1dBladeEditor::setFlowPathCurves()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    // 1.
    TNurbsCurve* zrCurve = getFlowPathCurveFromVane(spans[i]);
    if (!zrCurve)
      return errorCode;
    // 2.
    curve_Nurbs* flowPath = getZRNurbsFlowPath(spans[i]);
    if (!flowPath)
      return errorCode;
    // 3.
    flowPath->fillFromNurbsCurve(*(zrCurve));
  }

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::setBladeEdgeCurves()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  QVector<QVector<Double2>> bladeEdgePoints;

  // 1.
  for (int i = 0; i < spans.size(); i++)
  {
    QVector<double> uEdges;

    uEdges = getBladeEdgeVaule(spans[i]);

    if (uEdges.size() < 2)
    {
      if(isHasError(errorCode)) return errorCode;
    }
    QVector<Double2> spanEdgePoints;
    for (int j = 0; j< uEdges.size();j++)
    {
      Double2 EdgePoint = getZRPostion(spans[i], uEdges[j]);
      spanEdgePoints.push_back(EdgePoint);
    }

    bladeEdgePoints.push_back(spanEdgePoints);
  }

  //2.
  for (int i = 0; i < 2; i++)
  {
    int bladeEdgeType = i;
    curve_Spline* bldEdgeSpline = getZRSplineBladeEdge(bladeEdgeType);

    if(!bldEdgeSpline)
      if (isHasError(errorCode)) return errorCode;


    QVector<Double2> EdgePoints;
    for (int j = 0; j < spans.size(); j++)
    {
      EdgePoints.push_back(bladeEdgePoints[j][i]);
    }

    bldEdgeSpline->setControlPoints(EdgePoints);
  }

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::setBladeAngleCurves()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    // 1.
    TNurbsCurve* srcCurve = getBladeAngleCurveFromVane(spans[i]);
    if (!srcCurve)
      return errorCode;
    // 2.
    curve_Nurbs* objCurve = getBladeAngleNurbsCurve(spans[i]);
    if (!objCurve)
      return errorCode;
    // 3.
    objCurve->fillFromNurbsCurve(*(srcCurve));
    unitization(objCurve);

    double scale = 180. / PI;
    scaling(objCurve, 1, scale);
  }

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::setBladeThicknessCurves()
{
  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    // 1.
    TNurbsCurve* srcCurve = getBladeThicknessCurveFromVane(spans[i]);
    if (!srcCurve)
      return errorCode;
    // 2.
    curve_Nurbs* objCurve = getBladeThicknessNurbsCurve(spans[i]);
    if (!objCurve)
      return errorCode;
    // 3.
    objCurve->fillFromNurbsCurve(*(srcCurve));
    unitization(objCurve);

  }

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::setBladeStackingCurves()
{
  int errorCode = -1;

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::changeBladeStacking(int stackingTypeToChange)
{
  int errorCode = -1;

  errorCode = 0;
  return errorCode;
}

int T1dBladeEditor::getSpanIndex(double span)
{
  int index = span / 50.;
  return index;
}

int T1dBladeEditor::getSpanIndexFromSpans(double span)
{
  QVector<double> spans = getSpans();

  for (int i = 0; i< spans.size(); i++)
  {
    double delta = (spans[i] - span);
    if (abs(delta) < 0.1)
      return i;
  }

  isHasError(-1);
  return 0;
}

QString T1dBladeEditor::getSpanNameFromSpanIndex(int spanIndex)
{
  QString spanName;
  QVector<double> spans = getSpans();
  double span = spans[spanIndex];

  if (spanIndex >= 0 && spanIndex < spans.size())
  {
    spanName = getSpanString(span);
    return spanName;
  }

  isHasError(-1);
  return spanName;
}

T1dBladeSection* T1dBladeEditor::getBladeSection(int index)
{
  if (!_pVaned->blade->pBldSect[index])
    _pVaned->blade->setBldSect();

  if (_pVaned->blade->pBldSect[index])
  {
    return _pVaned->blade->pBldSect[index];
  }
  return nullptr;
}

TNurbsCurve* T1dBladeEditor::getFlowPathCurveFromVane(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec && bldSec->pZRCurve)
    return bldSec->pZRCurve;
  return nullptr;
}

TNurbsCurve* T1dBladeEditor::getBladeAngleCurveFromVane(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec && bldSec->pBetaCurve)
    return bldSec->pBetaCurve;
  return nullptr;
}

TNurbsCurve* T1dBladeEditor::getBladeThicknessCurveFromVane(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec && bldSec->pThickCurve)
    return bldSec->pThickCurve;
  return nullptr;
}

TNurbsCurve* T1dBladeEditor::getConformalMappingAngleCurveFromVane(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec && bldSec->pThetaCurve)
    return bldSec->pThetaCurve;
  return nullptr;
}

void T1dBladeEditor::setBladeFromBladeEditor()
{
  setBladeAngleCurvesOfBlade();

  setBladeThicknessCurvesOfBlade();

  setBladeStackingCurveOfBlade();
}

void T1dBladeEditor::setBladeAngleCurvesOfBlade()
{
  QVector<double> spans = getSpans();
  for (int i = 0; i < spans.size();i++)
  {
    setBladeAngleCurveOfBlade(spans[i]);
  }
}

void T1dBladeEditor::setBladeThicknessCurvesOfBlade()
{
  QVector<double> spans = getSpans();
  for (int i = 0; i < spans.size(); i++)
  {
    setBladeThicknessCurveOfBlade(spans[i]);
  }
}

void T1dBladeEditor::setBladeAngleCurveOfBlade(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  curve_Nurbs* bladeAngle = getBladeAngleNurbsCurve(span);
  if (bldSec && bldSec->pBetaCurve && bladeAngle)
  {
    curve_Nurbs* bladeAngle_copy = new curve_Nurbs;
    double flowPathLengthOfSection = getBladeMeridionalLength(span);
    bladeAngle_copy->copyFrom(bladeAngle);
    scaling(bladeAngle_copy, 0, flowPathLengthOfSection);
    scaling(bladeAngle_copy, 1, PI/180.);

    // 2.
    bladeAngle_copy->copyTo( bldSec->pBetaCurve );
    // 3.
    bldSec->beta1b = getBladeAngle(span, LE);
    bldSec->beta2b = getBladeAngle(span, TE);

    bldSec->updateBladeAngleCurveOfVane();

    delete bladeAngle_copy;
  }
}

void T1dBladeEditor::setBladeThetaOfBlade(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec)
  {
    bldSec->Theta1 = getTheta(span, LE);
    bldSec->Theta2 = getTheta(span, TE);

    bldSec->wrapAngle = getWrapAngle(span);
  }
}

void T1dBladeEditor::setBladeThicknessCurveOfBlade(double span)
{
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  curve_Nurbs* thicknessCurve = getBladeThicknessNurbsCurve(span);
  if (bldSec && bldSec->pThickCurve && thicknessCurve)
  {
    curve_Nurbs* thicknessCurve_Copy = new curve_Nurbs;
    double flowPathLengthOfSection = getBladeMeridionalLength(span);
    thicknessCurve->copyFrom(thicknessCurve);
    scaling(thicknessCurve_Copy, 0, flowPathLengthOfSection);

    // 2.
    thicknessCurve_Copy->copyTo(bldSec->pThickCurve);
    // 3.
    bldSec->thick1 = getBladeThickness(span, LE);
    bldSec->thick2 = getBladeThickness(span, TE);

    // 4.
    Double2 pos_maxThickness = getMaxThicknessPosition(span);
    bldSec->thickmax_loc_chord = pos_maxThickness[0];
    bldSec->thickMax = pos_maxThickness[1];

    // 5.
    bldSec->updateBladeThicknessCurveOfVane();
    delete thicknessCurve_Copy;
  }
}

void T1dBladeEditor::setBladeStackingCurveOfBlade()
{
  if (!_pVaned || !_pVaned->blade)
    return;

  // 1.
  _pVaned->blade->setStackingTangentialType(getStackingType());

  // 2.
  QVector<double> spans = getSpans();
  for (int i = 0; i < spans.size(); i++)
  {
    setBladeThetaOfBlade(spans[i]);
  }

  _pVaned->blade->updateStackingTangential();
}

QVector<double> T1dBladeEditor::getBladeEdgeVaule(double span)
{
  QVector<double> uEdges;
  int index = getSpanIndex(span);
  T1dBladeSection* bldSec = getBladeSection(index);
  if (bldSec)
  {
    uEdges.push_back(bldSec->u1);
    uEdges.push_back(bldSec->u2);
  }
  return uEdges;
}

Double2 T1dBladeEditor::getZRPostion(double span,double u)
{
  Double2 pos;
  // todo
  curve_Nurbs* s = getZRNurbsFlowPath(span);

  if (s)
  {
    pos = s->getPoint(u);
  }
  else
    isHasError(-1);

  return pos;
}

void T1dBladeEditor::updateTopology(curve_Topology* T)
{
  if (!T)
    return;

  if (T == getZRCurvesTopo(false))
  {
    T = updateZRTopology();
  }
  else if (T == getBladeThicknessCurvesTopo(false))
  {
    T = updateThicknessTopology();
  }
  else if (T == getBladeAngleCurvesTopo(false))
  {
    T = updateBladeAngleTopology();
  }
  else if (T == getBladeConformalMappingCurvesTopo(false))
  {
    T = updateBladeConformalMappingTopology();
  }


}

curve_Topology* T1dBladeEditor::updateZRTopology()
{
  curve_Topology* zrCurveTopo = getZRCurvesTopo(false);
  if (!zrCurveTopo)
    return nullptr;
  // 1. ZR flow path curve



  // 2. Blade edge zr curve
  QVector<double> spans = getSpans();
  int num_spans = spans.size();

  int num_bladeEdge = 2;

  // 1.
  for (int j = 0; j < 2; j++)
  {

    curve_Spline* edge = getZRSplineBladeEdge(j,false);
    if (!edge)
      continue;
    int num_edgePoints = edge->getControlPointCount();
    if (num_edgePoints != num_spans)
      continue;

    for (int i = 0; i < spans.size(); i++)
    {
      double span = spans[i];
      curve_Curve* flowpath = getZRCurve(span);
      if (!flowpath)
        continue;

      if (i == 0)
        zrCurveTopo->setEndPointOnCurve(edge, curve_Curve::StartPoint, flowpath);
      else if (i == num_spans - 1)
        zrCurveTopo->setEndPointOnCurve(edge, curve_Curve::EndPoint, flowpath);
      else
        zrCurveTopo->setEndPointOnCurve(edge, i, flowpath);
    }
  }

  zrCurveTopo->updateDependencies0();

  return zrCurveTopo;
}

curve_Topology* T1dBladeEditor::updateThicknessTopology()
{
  curve_Topology* bladeThicknessCurvesTopo = getBladeThicknessCurvesTopo(false);
  if (!bladeThicknessCurvesTopo)
    return nullptr;

  QVector<double> spans = getSpans();
  // 1.
  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];
    curve_Nurbs* thickness = getBladeThicknessNurbsCurve(span);
    if (!thickness)
      continue;

    bladeThicknessCurvesTopo->setEndPointConstantX(thickness, 0);
    bladeThicknessCurvesTopo->setEndPointConstantX(thickness, thickness->getControlPointCount()-1);
    bladeThicknessCurvesTopo->updateDependencies0();
  }
  return bladeThicknessCurvesTopo;
}



curve_Topology* T1dBladeEditor::updateBladeAngleTopology()
{
  curve_Topology* bladeAngleCurvesTopo = getBladeAngleCurvesTopo(false);
  if (!bladeAngleCurvesTopo)
    return nullptr;

  QVector<double> spans = getSpans();
  // 1.
  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];
    curve_Nurbs* bladeAngleCurve = getBladeAngleNurbsCurve(span);
    if (!bladeAngleCurve)
      continue;

    bladeAngleCurvesTopo->setEndPointConstantX(bladeAngleCurve, 0);
    bladeAngleCurvesTopo->setEndPointConstantX(bladeAngleCurve, bladeAngleCurve->getControlPointCount() - 1);
    bladeAngleCurvesTopo->updateDependencies0();
  }
  return bladeAngleCurvesTopo;
}

curve_Topology* T1dBladeEditor::updateBladeConformalMappingTopology()
{
  curve_Topology* ConformalMappingCurvesTopo = getBladeConformalMappingCurvesTopo(false);
  if (!ConformalMappingCurvesTopo)
    return nullptr;

  QVector<double> spans = getSpans();
  // 1.
  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];
    curve_Nurbs* MThetaCurve = getMThetaNurbsCurve(span);
    if (!MThetaCurve)
      continue;

    ConformalMappingCurvesTopo->setEndPointFixed(MThetaCurve, 0);
    ConformalMappingCurvesTopo->setEndPointFixed(MThetaCurve, -1);
    ConformalMappingCurvesTopo->updateDependencies0();
  }

  // 2.
  for (int i = 0; i < spans.size(); i++)
  {
    double span = spans[i];
    curve_Nurbs* MThetaCurve = getMThetaNurbsCurve(span);
    if (!MThetaCurve)
      continue;

    // 2.2
    for (int j = 0; j < 2; j++)
    {
      int index_start = 0;
      int sign = 1;
      if (j != 0)
      {
        index_start = MThetaCurve->getControlPointCount()-1;
        sign = -1;
      }

      QVector<Double2> Cps;
      for (int k = 0; k<2; k++)
      {
        int index = index_start + sign * k;
        Double2 cp = MThetaCurve->getControlPointPosition(index);
        Cps.push_back(cp);
      }

      int index = index_start + sign;
      Double2 T = Cps[1] - Cps[0];
      //2.
      ConformalMappingCurvesTopo->setEndPointOnVector(MThetaCurve, index, T);

      ConformalMappingCurvesTopo->updateDependencies0();
    }
   
  }

  return ConformalMappingCurvesTopo;
}

void T1dBladeEditor::getBladeAngleFromMThetaCurves(QVector<QVector<double>>& bladeAngles)
{
  bladeAngles.clear();

  QVector<double> spans = getSpans();
  Double2 MAxis = {1., 0.};
  for (int i = 0; i < getSpans().size(); i++)
  {
    QVector<double> bladeAngle_span;
    double span = spans[i];

    curve_Nurbs* camberCurve = getMThetaNurbsCurve(span);

    Double2 T = camberCurve->getTangent(0.);
    double beta1b = MAxis.angleSigned(T);
    bladeAngle_span.push_back(beta1b);

    T = camberCurve->getTangent(1.);

    double beta2b = MAxis.angleSigned(T);
    bladeAngle_span.push_back(beta2b);

   
    double wrapAngle = (camberCurve->getPoint(1.) - camberCurve->getPoint(0.))[1];
    //wrapAngle = getDegree(wrapAngle);

    bladeAngle_span.push_back(wrapAngle);

    bladeAngles.push_back(bladeAngle_span);
  }
}

void T1dBladeEditor::setMThetaCurvesFromBladeAngle(QVector<QVector<double>>& bladeAngles)
{
  // 1.
  QVector<QVector<double>> bladeAnglesSource;
  getBladeAngleFromMThetaCurves(bladeAnglesSource);

  QVector<double> spans = getSpans();
  Double2 MAxis = { 1., 0. };

  // 2.
  for (int i = 0; i < getSpans().size(); i++)
  {
    double span = spans[i];

    curve_Nurbs* camberCurve = getMThetaNurbsCurve(span);

    QVector<Double2> CPs = camberCurve->getControlPointsDouble2();
    int num = CPs.size();

    // #1 control point
    double delta = bladeAngles[i][0] - bladeAnglesSource[i][0];
    if (abs(getDegree(delta)) > 0.1)
    {
      double x = (CPs[1] - CPs[0])[0];
      double y = x * tan(bladeAngles[i][0]) ;

      CPs[1] = Double2(CPs[1][0], y+ CPs[0][1]);
    }

    // latest 
    int last = num - 1;
    delta = bladeAngles[i][1] - bladeAnglesSource[i][1];
    if (abs(getDegree(delta)) > 0.1)
    {
      double x = (CPs[last] - CPs[last -1])[0];
      double y = x * tan(bladeAngles[i][1]);

      CPs[last-1] = Double2(CPs[last -1][0], CPs[last][1]-y);
    }

    // for wrap angle
    delta = bladeAngles[i][2] - bladeAnglesSource[i][2];
    if (abs(getDegree(delta)) > 0.1)
    {
      Double2 Delta = {0., delta };
      CPs[last - 1] += Delta;
      CPs[last] += Delta;
    }

    camberCurve->setControlPoints(CPs);
  }

}

int T1dBladeEditor::updateSelectedCurve(curve_Topology* T, QString selectedCurvePath, curve_Curve* curveSelected)
{
  if (T == getZRCurvesTopo(false))
  {

    curve_Curve* c = T->getCurve(selectedCurvePath);
    if (c && curveSelected)
    {
      c->copyCurve(curveSelected);
    }
  }
  else if (T == getBladeThicknessCurvesTopo(false))
  {
    curve_Curve* c = T->getCurve(selectedCurvePath);
    if (c && curveSelected)
    {
      c->copyCurve(curveSelected);
    }
  }
  else if (T == getBladeAngleCurvesTopo(false))
  {
    curve_Curve* c = T->getCurve(selectedCurvePath);
    if (c && curveSelected)
    {
      c->copyCurve(curveSelected);
    }
  }
  else if (T == getBladeConformalMappingCurvesTopo(false))
  {
    curve_Curve* c = T->getCurve(selectedCurvePath);
    if (c && curveSelected)
    {
      c->copyCurve(curveSelected);
    }
  }
  return 0;
}

QVector<double> T1dBladeEditor::getDeltaThetaFromMThetaCurves()
{
  QVector<double> spans = getSpans();

  QVector<double> deltaTheta;
  for (int i = 0; i< spans.size();i++)
  {
    curve_Nurbs* MTheta = getMThetaNurbsCurve(spans[i], false);

    QVector<Double2> CPs = MTheta->getControlPointsDouble2();
    if (getStackingType() == LE_Stacking)
      deltaTheta.push_back((CPs.first())[1]);
    else
      deltaTheta.push_back((CPs.last())[1]);
  }

  double theta_span_0 = deltaTheta.first();
  for (int i = 0; i < spans.size(); i++)
  {
    deltaTheta[i] -= theta_span_0;
  }

  return deltaTheta;
}

int T1dBladeEditor::updateMThetaCurvesByMerdionalCurvesChanged()
{
  if (_bladeEditorType != bladeConformalMapping)
    return 0;

  int errorCode = -1;

  QVector<double> spans = getSpans();

  for (int i = 0; i< spans.size();i++)
  {
    double span = spans[i];

    errorCode = updateMThetaCurveByMerdionalCurvesChanged(span);

    if(isHasError(errorCode))
      return errorCode;
  }

  return 0;
}

int T1dBladeEditor::updateMThetaCurveByMerdionalCurvesChanged(double span)
{
  int errorCode = -1;
  // 0.
  curve_Nurbs* MTheta = getMThetaNurbsCurve(span, false);
  if (!MTheta)
  {
    isHasError(errorCode);
    return errorCode;
  }

  // 1.
  double ule = getUofEdgeOnFlowPath(span, LE);
  double ute = getUofEdgeOnFlowPath(span, TE);

  double M_now = calculateBladeMerdionalTranformalCurve(span, ule, ute);

  QVector<Double2> CPs = MTheta->getControlPointsDouble2();
  double M_diff = (CPs.last() - CPs.first())[0];
  
  double delta_M = M_now - M_diff;
  if ( abs(delta_M) < 0.01 * M_diff)
    return 0;

  // 2.
  int size = CPs.size();

  // 2.1 Copy
  QVector<Double2> CPs_copy = CPs;

  // 2.2 Translation
  double delta = delta_M / (size - 1.);
  Double2 vec_delta = { delta, 0 };

  for (int i = 1; i < size; i++)
  {
    CPs[i] += ((1. * i) * vec_delta);
  }

  //2.3 1st
  Double2 T = CPs_copy[1] - CPs_copy[0];
  double scaling = 1 + delta / T[0];
  T *= scaling;
  CPs[1] = T + CPs[0];

  //2.4 last
  int last = size - 1;
  T = CPs_copy[last-1] - CPs_copy[last];
  scaling = 1 + delta / abs(T[0]);
  T *= scaling;
  CPs[last - 1] = T + CPs[last];

  // 3.
  MTheta->setControlPoints(CPs);

  return 0;
}

void T1dBladeEditor::setMThetaCurvesFromStackingSetting(QVector<double> deltaTheta)
{
  // 1.
  QVector<double> DeltaTheta_current =  getDeltaThetaFromMThetaCurves();

  // 2.
  QVector<double> spans = getSpans();

  for (int i = 0; i < spans.size(); i++)
  {
    // 2.1 check
    double delta = deltaTheta[i] - DeltaTheta_current[i];
    if (abs(getDegree(delta)) < 0.1)
    {
      continue;
    }

    // 2.2 set
    curve_Nurbs* MTheta = getMThetaNurbsCurve(spans[i], false);

    QVector<Double2> CPs = MTheta->getControlPointsDouble2();

    Double2 vec_deltaTheta = Double2(0., delta);

    for (int j = 0; j< CPs.size();j++)
    {
      CPs[j] += vec_deltaTheta;
    }
    MTheta->setControlPoints(CPs);

  }
}


#include "1d_CalculateThroatArea.h"
// written by Feihong
QVector<QVector<Double3>> T1dBladeEditor::getThroatSurface(QString surfacename)
{
  // 1. getBladesurface/line
  QVector<QVector<Double3>> bs1 = getBladeSurface("Pressure"); 
  QVector<Double3> bhl1 = bs1.first(); QVector<Double3> bml1 = bs1[1]; QVector<Double3> btl1 = bs1.last();
  QVector<QVector<Double3>> bs2 = getBladeSurface_rotate("Suction"); 
  QVector<Double3> bhl2 = bs2.first(); QVector<Double3> bml2 = bs2[1]; QVector<Double3> btl2 = bs2.last();

  // 2.getPoint
  Double3 tp1, tp2, mp1, mp2, hp1, hp2;
  CalculateThoratArea ThroatA;
  ThroatA.getLengthCurve2Curve(btl1, btl2, tp1, tp2);
  ThroatA.getLengthCurve2Curve(bml1, bml2, mp1, mp2);
  ThroatA.getLengthCurve2Curve(bhl1, bhl2, hp1, hp2);

  // 3.getline
  QVector<Double3> tipline = QVector<Double3>() << tp1 << tp2;
  QVector<Double3> meanline = QVector<Double3>() << mp1 << mp2;
  QVector<Double3> hubline = QVector<Double3>() << hp1 << hp2;

  // 4.getsurface
  QVector<QVector<Double3>> throatsurface = QVector<QVector<Double3>>() << tipline << meanline << hubline;

  // 5.getlegth
  o1hub = (hp1 - hp2).length(); o1mean = (mp1 - mp2).length(); o1tip = (tp1 - tp2).length();

  // 6.getarea
  auto getArea = [&](Double3 pt1, Double3 pt2, Double3 pt3, Double3 pt4) -> double
  {
    return ((pt2 - pt3).length() * (pt1 - pt4).length()) / 2.;
  };
  ThroatArea = getArea(tp1, tp2, mp1, mp2) + getArea(mp1, mp2, hp1, hp2);

  return throatsurface;
}