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

#ifndef OEND_BLADEEDITOR_H
#define OEND_BLADEEDITOR_H

#include "util_TObject.h"
class curve_Topology;
class curve_Curve;
class curve_Spline;
class curve_Nurbs;
class curve_Line;
class TNurbsCurve;

class T1dVaned;
class T1dBladeSection;
class profileGenerator;

class T_EXPORT_1D T1dBladeEditor : public TObject
{
  T_OBJECT;

public:
  T1dBladeEditor(QString object_n = "", TObject* iparent = NULL);
  double o1tip, o1mean, o1hub, ThroatArea;

public:
  bool isHasError(int errorCode);

public:
  int setVaned(T1dVaned* pVaned);
  int initialBladeEditor(QVector<double> spans);
  int updateBladeEditor();
  int updateBladeMetal();
  int updateBladeConformalMapping();
  int updateStacking();
  int updateMerdional();
public:
  int calculateBladeMetalAngleCurve(double span);
  int calculateBladeConformalMapping(double span);
  double calculateBladeMerdionalTranformalCurve(double span, double uStart = 0., double uEnd = 1.);

  double getUofEdgeOnFlowPath(double span, int edgeType); // todo

  double getBladeMeridionalLength(double span);
  double getTransformalMeridional(double span);


  int getStackingType();
  void setStackingType(int stackingType);
  Double2 getStackingPosition();
  double getThetaFromStackingCurve(double span);
  QVector<double> getStackingParameters(double span); // Now: use this
  double getBaseTheta_Stacking();
  double getTheta_Stacking(double span);
  double getTheta(double span, int bladeEdge);
  double getBladeAngle(double span, int bladeEdge);
  double getWrapAngle(double span);
  double getBladeThickness(double span, int bladeEdge);
  Double2 getMaxThicknessPosition(double span);

  QVector<double> getSpans();
  int getSpanSize();
protected:
  void setSpans(QVector<double> span);

public:
  int setZRCurves();
  int setFlowPathCurves();
  int setBladeEdgeCurves();
  int setBladeAngleCurves();
  int setBladeThicknessCurves();
  int setBladeStackingCurves(); // todo

  int changeBladeStacking(int stackingTypeToChange); // todo

public: // Get : vane curve
  int getSpanIndex(double span);
  int getSpanIndexFromSpans(double span);
  QString getSpanNameFromSpanIndex(int spanIndex);
  T1dBladeSection* getBladeSection(int index);
  TNurbsCurve* getFlowPathCurveFromVane(double span);
  QVector<double> getBladeEdgeVaule(double span);
  TNurbsCurve* getBladeAngleCurveFromVane(double span);
  TNurbsCurve* getBladeThicknessCurveFromVane(double span);
  TNurbsCurve* getConformalMappingAngleCurveFromVane(double span);

public: // Set: blade curve of Vane
  void setBladeFromBladeEditor();
  void setBladeAngleCurvesOfBlade();
  void setBladeStackingCurveOfBlade();
  void setBladeThicknessCurvesOfBlade();
  void setBladeAngleCurveOfBlade(double span);
  void setBladeThicknessCurveOfBlade(double span);
  void setBladeThetaOfBlade(double span);

public:
  // Topology
  curve_Topology* getZRCurvesTopo(bool createIfNotAvailable = true);
  curve_Topology* getBladeAngleCurvesTopo(bool createIfNotAvailable = true);
  curve_Topology* getBladeThicknessCurvesTopo(bool createIfNotAvailable = true);
  curve_Topology* getBladeConformalMappingCurvesTopo(bool createIfNotAvailable = true);

  // curve_Curve
  curve_Curve* getZRCurve(double span);
  curve_Curve* getZRBladeEdge();
  curve_Curve* getBladeAngleCurves();
  curve_Curve* getBladeThicknessCurves();
  curve_Curve* getBladeConformalMappingCurves();
  curve_Curve* getBladeConformalMappingAssistingCurves(double span); 

  // curve: object
  curve_Nurbs* getZRNurbsFlowPath(double span, bool createIfNotAvailable = true);
  curve_Spline* getZRSplineBladeEdge(int bladeEdgeType, bool createIfNotAvailable = true);
  curve_Nurbs* getBladeAngleNurbsCurve(double span, int merdionalType = 0, int bladeAngleType = 0, bool createIfNotAvailable = true);
  curve_Nurbs* getBladeThicknessNurbsCurve(double span, int merdionalType = 0, int bladeThicknessType = 0, bool createIfNotAvailable = true);
  curve_Spline* getStackingSpline(bool createIfNotAvailable = true); // todo
  curve_Nurbs* getMThetaNurbsCurve(double span, bool createIfNotAvailable = true);

public:
  profileGenerator* getProfileGenerator(double span, bool createIfNotAvailable = true);
  int updateProfilesGenerator();
  int updateProfileGenerator(double span);


  QVector<QVector<Double3>> getBladeSurface(QString SurfaceName, int spansInterpolated = 21);
  QVector<QVector<Double3>> getBladeSurface_rotate(QString SurfaceName, int spansInterpolated = 21);
  QVector<QVector<Double3>> getRevolutionSurface(double span);
  
  // curve: asssiting string functions
  QString getMeridionalTypeString(int merdionalType);
  QString getSpanString(double span);
  QString getBladeEdgeString(int bladeEdgeType);
  QString getBladeAngleTypeString(int bladeAngleType);
  QString getBladeThicknessTypeString(int bladeThicknessType);
  QString getBladeAngleCurveNameString(double span, int merdionalType, int bladeAngleType);
  QString getBladeThicknessCurveNameString(double span, int merdionalType, int bladeThicknessType);

public: // tool functions
  void unitization(curve_Nurbs* s, int whichIndex = 0);
  void scaling(curve_Nurbs* s, int whichIndex = 1, double scale = 1.);
  double getRad(double deg);
  double getDegree(double rad);
public: // tool functions 2
  Double2 getZRPostion(double span, double u);
  void updateTopology(curve_Topology* T);
  curve_Topology* updateZRTopology();
  curve_Topology* updateThicknessTopology();
  curve_Topology* updateBladeAngleTopology();
  curve_Topology* updateBladeConformalMappingTopology();

  void getBladeAngleFromMThetaCurves(QVector<QVector<double>>& bladeAngles);
  void setMThetaCurvesFromBladeAngle(QVector<QVector<double>>& bladeAngles);
  int updateMThetaCurvesByMerdionalCurvesChanged();
  int updateMThetaCurveByMerdionalCurvesChanged(double span);

  int updateSelectedCurve(curve_Topology* T, QString selectedCurvePath, curve_Curve* curveSelected);

  QVector<double> getDeltaThetaFromMThetaCurves();
  void setMThetaCurvesFromStackingSetting(QVector<double> deltaTheta);

public: // by Feihong
  QVector<QVector<Double3>> getThroatSurface(QString surfacename);


  // variables
public:
  T1dVaned* _pVaned = nullptr;
  int _bladeEditorType = 0;
  int _numPTS = 51;

  enum bladeEditorType
  {
    bladeMetalAngleAndThickness = 0,
    bladeConformalMapping = 1
  };

  enum meridionalTypes
  {
    meridionalPercent = 0,
    meridionallength = 1,
    meridionalTransformation = 2
  };

  enum bladeAngleTypes
  {
    bladeMetalAngle = 0,
    bladeThetaAngle = 1
  };

  enum bladeThicknessTypes
  {
    metalThickness = 0,
    transformalThickness = 1
  };

  enum bladeEdgeTypes
  {
    LE = 0,
    TE = 1
  };

  enum stackingTypes
  {
    LE_Stacking = 0,
    TE_Stacking = 1
  };

  enum bladeSurfaceTypes
  {
    LE_Surface = 0,
    Pressure_Surface = 1,
    Suction_Surface = 2,
    TE_Surface = 3
  };

private:
  QVector<double> _spans;
  int _stackingType = 0;
};
#endif // OEND_BLADEEDITOR_H
