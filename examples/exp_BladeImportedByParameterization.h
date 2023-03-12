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
#include "w_PropertyHolderWidget.h"
#include "draw_TopologyInteractiveEditorWidget.h"
#include "vis_Widget.h"

class curve_Topology;
class curve_Curve;
class curve_Nurbs;
class curve_Polygon;

class T_EXPORT_EXAMPLES exp_BladeImportedByParameterization : public QObject, public TObject
{
  Q_OBJECT;
  T_OBJECT;

public:
  exp_BladeImportedByParameterization(QString object_n = "", TObject *iparent = NULL);
  virtual ~exp_BladeImportedByParameterization() {}

public:
  QVector<QVector<double>> data;
  bool getData(QString filename);
  void exportToFile(QString fileName);

public: // Topology & Curve manage
  curve_Topology* getZRCurveTopology(bool createIfNotAvailable = true);
  curve_Curve* getZRCurve(double span, bool createIfNotAvailable = true);
  curve_Nurbs* getZRNurbsCurve(double span, bool createIfNotAvailable = true);
  curve_Polygon* getZRPolgonCurve(double span, bool createIfNotAvailable = true);

  curve_Topology* getMFracThicknessCurvesTopology(bool createIfNotAvailable = true);
  curve_Curve* getMFracThicknessCurve(double span, bool createIfNotAvailable = true);
  curve_Nurbs* getMFracThicknessNurbsCurve(double span, bool createIfNotAvailable = true);
  curve_Polygon* getMFracThicknessPolgonCurve(double span, bool createIfNotAvailable = true);

  curve_Topology* getMThetaCurveTopology(bool createIfNotAvailable = true);
  curve_Curve* getCamberMThetaCurve(double span, bool createIfNotAvailable = true);
  curve_Nurbs* getCamberMThetaNurbsCurve(double span, bool createIfNotAvailable = true);
  curve_Polygon* getCamberMThetaPolygonCurve(double span, bool createIfNotAvailable = true);

  curve_Topology* getmBetaCurveTopology(bool createIfNotAvailable = true);
  curve_Curve* getCambermBetaCurve(double span, bool createIfNotAvailable = true);
  curve_Polygon* getCambermBetaPolygonCurve(double span, bool createIfNotAvailable = true);
  
  curve_Curve* getCamberMFracMCurve(double span, bool createIfNotAvailable = true);
  curve_Nurbs* getCamberMFracM(double span, bool createIfNotAvailable = true);

  curve_Curve* getProfileMThetaCurves(double span, bool createIfNotAvailable = true);
  curve_Nurbs* getProfileMThetaNurbsCurve(double span, int position, bool createIfNotAvailable = true);
  curve_Polygon* getProfileMThetaPolygonCurve(double span, int position, bool createIfNotAvailable = true);
  QString getCurveNameOfProfile(int position);

public: 
  // core
  int createBlade();
  bool setProfile(double span);

  QVector<Double2> transfromTomTheta(curve_Nurbs* zrNurbs, QVector<Double2> mBeta);

  // create profile
  int createProfile(double span);
  int createCamberCurve(double span);
  int updateCamberCurve(double span);

  int checkMfrac(curve_Polygon* polyChecked, curve_Polygon* zrPoly, curve_Nurbs* zrNurbs);
  // calculate edge
  double getMFracOfLECycle(double span);
  double getMFracOfTEShread(double span);

  double getEquivalentThinkness(double mFrac, double span);
  double pMFracMOfNurbs(double M, double span);
  int getSignOfCamberCurve(double span);
  int getPressureSignAlongCamberCurve(double span);
  int getSuctionSignAlongCamberCurve(double span);

  void createLECurve(double ule, double span);
  void createTECurve(double span);
  void createPressureCurve(double ule, double ute, double span);
  void createSuctionCurve(double ule, double ute, double span);

  int getSignOfCuttedSideAtTE(double span); // Very important
  QString getCuttedSideName(double span);
  QString getExtendSideName(double span);
  void extendSideCurve(double span);
  void cuttedSideCurve(double span);
  void fitSideCurve(double span);


  double getMeridonalLength(double span);
  double getMFromMFrac(double span, double mFrac, double mFrac_start = 0.);
  double get_mfracFromM(double span,double M);
  QVector<Double3> get3Ds(curve_Polygon* pMTheta, double span );

  QVector<Double3> getProfile(double span);
  QVector<QVector<Double3>> getProfile1(double span);
  QVector<Double3> getFlowPath(double span);
public:
  int readSpanwiseData(double span, QString spanwiseDataFile, QString LengthUnitImport);

  QStringList getStringList(QString& s);
  QVector<Double2> getDouble2Data(QStringList& strArray);

  QVector<QVector<QVector<Double2>>> dataSpans;
  QVector<Double2> getSpanData(QString whichLevelData, double span); // 1. (z, r);2. (m, theta);3. (mFrac, thickness)
  double coefficentWithLengthUnit(QString lengthUnit);
  QString getFilePath(QString filename);
public: // tool
  QString getString(double span);
  double coeffienceWithLengthUnitExported();
public: // GUI
  void showDialog(exp_BladeImportedByParameterization* blade);
  void setVTKShow(vis_Widget* w);

  w_PropertyHolderWidget* holder;
  w_PropertyHolderWidget* holder_properties;
  w_PropertyHolderWidget* holder_topoZR;
  draw_TopologyInteractiveEditorWidget* wZRCurves;

  w_PropertyHolderWidget* holder_topoMRTheta;
  draw_TopologyInteractiveEditorWidget* wMRTheta;

  w_PropertyHolderWidget* holder_topoMfracBeta;
  draw_TopologyInteractiveEditorWidget* wMfracBeta;

  w_PropertyHolderWidget* holder_3D;
  vis_Widget* w3D;
  QString lengthUnitExported = "mm";

public:
  void setCamberDataType(int camberDataType);
  int getCamberDataType();
  QStringList getAllReferenceAxisType();
  void setReferenceAxis(QString referenceAxis);
  QString getCurrentReferenceAxis();
  Double3 get3DAtReferenceAxis(Double2 zr, double theta);
  void revolutionSurface(curve_Polygon* zrCurve, QVector<QVector<Double3>>& surface, QVector<QVector<double>>& value);
  double getDoubleDegree(double degree);

public:
  int _bladeNumOfMainBladeRow=10;
  double offsetTheta = 0.;
  int _camberDataType = 0;
  int _referenceAxis = -1;
  int _degreeType = 0;
  enum _referenceAxisTypes
  {
    negtiveZAxis = -1,
    positiveZAxis = 1
  };

  enum camberDataTypes
  {
    ZRTheta = 0,
    ZRBeta = 1,
    XYZ = 2
  };
  enum rotationDirectionTypes
  {
    CCW = -1,
    CW = 1
  };
  int _rotationDirection = 0;

public slots:
  void handleUpdateData();
  void onImportPressed();
  void onExportPressed();


public:
	QString filename;
};
