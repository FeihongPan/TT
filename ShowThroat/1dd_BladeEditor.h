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

#ifndef OENDD_BLADEEDITOR_H
#define OENDD_BLADEEDITOR_H
#include "w_QDialog.h"
#include "draw_TopologyInteractiveEditorWidget.h"

class T1dBladeEditor;
class T1dBladeEditorDlg;

class w_PropertyHolderWidget;
class vis_Widget;
class draw_XYInteractive;
class w_QPushButton;
class TPropertyInputWidget;

// T1dBladeMeridionalCurveWidget
class T_EXPORT_1D T1dBladeCurveBaseWidget : public draw_TopologyInteractiveEditorWidget
{
  Q_OBJECT

public:
  T1dBladeCurveBaseWidget(QWidget* parent = 0);

public:
  void initializePage();
public:
  void setTopology();
  void updateTopology();

public:
  curve_Topology* _topology = 0;
  T1dBladeEditorDlg* dlg;
public:
  T1dBladeEditor* getBladeEditor();
  virtual void updateCurve();
  
protected:
  virtual void resizeEvent(QResizeEvent* e);
  virtual void redraw();

public slots:
  virtual void handleValueChangedFinished(QVariant v);
  virtual void config();
};

// T1dBladeMeridionalCurveWidget
class T_EXPORT_1D T1dBladeMeridionalCurveWidget : public T1dBladeCurveBaseWidget
{
  Q_OBJECT

public:
  T1dBladeMeridionalCurveWidget(QWidget* parent = 0);

public:
  void initializePage();
  virtual void updateCurve();

};

// T1dBladeAngleCurveWidget
class T_EXPORT_1D T1dBladeAngleCurveWidget : public T1dBladeCurveBaseWidget
{
  Q_OBJECT

public:
  T1dBladeAngleCurveWidget(QWidget* parent = 0);

public:
  void initializePage();
  virtual void updateCurve();

};

// T1dBladeConformalMappingCurveWidget
class T_EXPORT_1D T1dBladeConformalMappingCurveWidget : public T1dBladeCurveBaseWidget
{
  Q_OBJECT

public:
  T1dBladeConformalMappingCurveWidget(QWidget* parent = 0);
public:
  QString _stackingType;
  void initializePage();
  virtual void updateCurve();

public slots:
  virtual void config();
  void stackingTypeChange();
};

// T1dBladeThicknessCurveWidget
class T_EXPORT_1D T1dBladeThicknessCurveWidget : public T1dBladeCurveBaseWidget
{
  Q_OBJECT

public:
  T1dBladeThicknessCurveWidget(QWidget* parent = 0);
public:
  void initializePage();
};

//----------------------------------------
class T_EXPORT_1D T1dBladeEditorDlg : public w_QDialog
{
  Q_OBJECT

public:
  T1dBladeEditorDlg(QWidget* parent = 0, Qt::WindowFlags f = 0);

public:
  int update();
  int update1();

public:
  int initializePage();
  void update3DView();
  void updateSurfaceView();

public:
  int setBladeEditor(T1dBladeEditor* bladeEditor);

public:
  T1dBladeMeridionalCurveWidget* _MerdionalCurvesWidget;
  T1dBladeAngleCurveWidget* _bladeAngleCurvesWidget;
  T1dBladeConformalMappingCurveWidget* _bladeConformalMappingCurveWidget;
  T1dBladeThicknessCurveWidget* _bladeThicknessCurvesWidget;
  vis_Widget* _3DViewWidget;
  w_PropertyHolderWidget* holder_3DViewWidget;
  w_PropertyHolderWidget* holder_MerdionalCurvesWidget;
  w_PropertyHolderWidget* holder_bladeAngleCurvesWidget;
  w_PropertyHolderWidget* holder_bladeConformalMappingCurveWidget;
  w_PropertyHolderWidget* holder_bladeThicknessCurvesWidget;
  // W14
  w_PropertyHolderWidget* holder_ThroatSurfaceWidget;
  w_PropertyHolderWidget* holder_ThroatPropertyWidget;
  vis_Widget* _ThroatSurfaceWidget;
  TPropertyInputWidget* _ThroatPropertyWidget;

  T1dBladeEditor* _bladeEditor;
  QString _bladeEditorType;
  
protected:
  virtual void resizeEvent(QResizeEvent* e);
  virtual void redraw();
private slots:
  void apply();
  void onConfigButtonPressed();
  void onShowThroatButton();
  void bladeEditorTypeChanged();
};
#endif // OENDD_BLADEEDITOR_H
