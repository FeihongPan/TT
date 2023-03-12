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

class curve_Topology;
class curve_Curve;

class T_EXPORT_EXAMPLES exp_CurveToplogyShroudMultiCurves : public QObject, public TObject
{
  Q_OBJECT;
  T_OBJECT;

public:
  exp_CurveToplogyShroudMultiCurves(QString object_n = "", TObject *iparent = NULL);
  virtual ~exp_CurveToplogyShroudMultiCurves() {}

public:
  curve_Topology* createShroudMultiCurves(bool createIfNotAvailable = true);
  curve_Curve* getShroudMultiCurve(bool createIfNotAvailable = true);
  curve_Curve* getSubCurve(int index, bool createIfNotAvailable = true);
  int getCurveType(int index);
  QString getCurveTypeName(int curveType);
  void showTopologyPropertiesInDialog(exp_CurveToplogyShroudMultiCurves* shroud);
  w_PropertyHolderWidget* holder_topo;
  draw_TopologyInteractiveEditorWidget *w;
  // assiting annotations
  void setAssiting();
  void setAssiting_RearTangentDirection();
  void setAssiting_HeightOfRearCircle();
  void setAssiting_RadiusOfRearCircle();
  void setAssiting_RadiusOfFrontCircle();
  void setAssiting_FrontThickness();
  void setAssiting_RearThickness();

  // core
  int makeShorudCurve();
  Double2 getPedal(Double2 pt, Double2 pt_line, Double2 T_line);
  double angleCycle(Double2 pt_center, Double2 pt);
  Double2 getPosition(QString postion);
  Double2 getTangetial(QString postion);

public slots:
  void handleUpdateData();

public:
  double thickness_front = 0.001;
  double thickness_rear = 0.001;
  double RC_front = 0.;
  double RC_rear = 0.;
  double L_center_rear = 0.;
  double H_center_rear = 0.;
  double phi_rear = PI / 2.;
  Double2 shroud_front;
  Double2 shroud_rear;
  Double2 hub_rear;
};
