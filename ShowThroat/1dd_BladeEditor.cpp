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

#include "1dd_BladeEditor.h"
#include "1d_BladeEditor.h"
#include "curve_Topology.h"

//#include "curve_Curve.h"
//#include "curve_Nurbs.h"
//#include "curve_Spline.h"
//#include "curve_Line.h"
#include "w_PropertyHolderDialog.h"
#include "w_PropertyHolderWidget.h"
#include "pe_TPropertyInputWidget.h"
#include "draw_TopologyInteractiveEditor.h"
#include "w_Widgets.h"
#include "vis_Widget.h"
#include "core_Application.h"

T1dBladeEditorDlg::T1dBladeEditorDlg(QWidget* parent, Qt::WindowFlags f) :
  w_QDialog(parent, f)
{
  setWindowTitle(tr("Blade editor"));
}

int T1dBladeEditorDlg::update()
{


  if (exec() == QDialog::Accepted)
  {
    _bladeEditor->setBladeFromBladeEditor();
    return 0;
  }

  return true;
}

int T1dBladeEditorDlg::setBladeEditor(T1dBladeEditor* bladeEditor)
{
  if (!bladeEditor)
    return -1;
  _bladeEditor = bladeEditor;

  if (_bladeConformalMappingCurveWidget)
  {
    //_bladeConformalMappingCurveWidget->_bladeEditor = bladeEditor;

    _bladeConformalMappingCurveWidget->_topology = _bladeEditor->getBladeConformalMappingCurvesTopo(false);
    if (_bladeConformalMappingCurveWidget->_topology)
      _bladeConformalMappingCurveWidget->updateTopology();
  }

  if (_MerdionalCurvesWidget)
  {
    //_MerdionalCurvesWidget->_bladeEditor = bladeEditor;

    _MerdionalCurvesWidget->_topology = _bladeEditor->getZRCurvesTopo(false);
    if (_MerdionalCurvesWidget->_topology)
      _MerdionalCurvesWidget->updateTopology();
  }

  if (_bladeAngleCurvesWidget)
  {
    //_bladeAngleCurvesWidget->_bladeEditor = bladeEditor;

    _bladeAngleCurvesWidget->_topology = _bladeEditor->getBladeAngleCurvesTopo(false);
    if (_bladeAngleCurvesWidget->_topology)
      _bladeAngleCurvesWidget->updateTopology();
  }

  if (_bladeThicknessCurvesWidget)
  {
    //_bladeThicknessCurvesWidget->_bladeEditor = bladeEditor;

    _bladeThicknessCurvesWidget->_topology = _bladeEditor->getBladeThicknessCurvesTopo(false);
    if(_bladeThicknessCurvesWidget->_topology)
     _bladeThicknessCurvesWidget->updateTopology();
  }

  update3DView();


  return 0;
}

int T1dBladeEditorDlg::initializePage()
{
  // W01
  QVBoxLayout* grid = new QVBoxLayout(this);
  // W02
  w_PropertyHolderWidget* holder = new w_PropertyHolderWidget(this);

  // W03
  // 1.
  holder_3DViewWidget = holder->getHolder(0, 0, 2, 1, tr("3D"));
  _3DViewWidget = vis_Widget::newWidget("vis_WidgetVtk", holder_3DViewWidget);
  holder_3DViewWidget->placeWidget(_3DViewWidget);

  // 2
  holder_MerdionalCurvesWidget = holder->getHolder(0, 1, 1, 1, tr("Merdional"));

  _MerdionalCurvesWidget = new T1dBladeMeridionalCurveWidget(this);
  holder_MerdionalCurvesWidget->placeWidget(_MerdionalCurvesWidget);

  // 3.
  holder_bladeAngleCurvesWidget = holder->getHolder(0, 2, 1, 1, tr("Blade beta"));

  _bladeAngleCurvesWidget = new T1dBladeAngleCurveWidget(this);
  holder_bladeAngleCurvesWidget->placeWidget(_bladeAngleCurvesWidget);

  // 4.
  holder_bladeConformalMappingCurveWidget = holder->getHolder(1, 2, 1, 1, tr("Blade conformal mapping"));

  _bladeConformalMappingCurveWidget = new T1dBladeConformalMappingCurveWidget(this);
  holder_bladeConformalMappingCurveWidget->placeWidget(_bladeConformalMappingCurveWidget);


  // 5.
  holder_bladeThicknessCurvesWidget = holder->getHolder(1, 1, 1, 1, tr("Thickness"));

  _bladeThicknessCurvesWidget = new T1dBladeThicknessCurveWidget(this);
  holder_bladeThicknessCurvesWidget->placeWidget(_bladeThicknessCurvesWidget);


  holder->setEvenColumnSize(true);
  // W04
  grid->addWidget(holder);

  QPushButton* applyConfig = new QPushButton(tr("Config"));
  connect(applyConfig, SIGNAL(clicked()), this, SLOT(onConfigButtonPressed()));  
  // W05
  QPushButton* applyShowThroat = new QPushButton(tr("Show Throat"));
  connect(applyShowThroat, SIGNAL(clicked()), this, SLOT(onShowThroatButton()));

  QPushButton* applyButton = new QPushButton(tr("Apply"));
  connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

  QPushButton* okButton = new w_OKButton;
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QPushButton* cancelButton = new w_CancelButton;
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(applyConfig);
  // W06
  buttonsLayout->addWidget(applyShowThroat);
  buttonsLayout->addStretch(1);

  buttonsLayout->addWidget(applyButton);
  buttonsLayout->addWidget(okButton);
  buttonsLayout->addWidget(cancelButton);
  // W07
  grid->addLayout(buttonsLayout);
  // W08
  setLayout(grid);
  setFocusPolicy(Qt::StrongFocus);

  return 0;
}

void T1dBladeEditorDlg::onConfigButtonPressed()
{
  w_PropertyHolderDialog dlg(core_Application::core());
  dlg.setWindowTitle(w_PropertyHolderDialog::tr("Config"));

  w_PropertyHolderWidget* holder = dlg.getHolder();
  w_PropertyHolderWidget* holder1 = holder->addHolder();

  QStringList _allBladeType = QStringList() << tr("Blade angle") << tr("Conformal mapping");
  _bladeEditorType = _allBladeType[_bladeEditor->_bladeEditorType];

  w_Property* wType = nullptr;
  if (wType = w_Property::getPropertyWidget(&_bladeEditorType, QObject::tr("Blade editor type: "), 
    holder1, &_allBladeType, true, false, w_Property::HLayout))
  {
    wType->setAutoSave();
    holder1->placeWidget(wType, 0, 0, 1, 1);
    connect(wType, SIGNAL(valueChanged()), this, SLOT(bladeEditorTypeChanged()));
  }

  if (dlg.exec() == w_QDialog::Accepted)
  {
  }

}

void T1dBladeEditorDlg::bladeEditorTypeChanged()
{
  if (_bladeEditorType == "Blade angle")
    _bladeEditor->_bladeEditorType = 0;
  else
    _bladeEditor->_bladeEditorType = 1;
}

void T1dBladeEditorDlg::update3DView()
{
  if (!holder_bladeThicknessCurvesWidget || !_3DViewWidget)
    return;

  auto getValuess = [&](QVector<QVector<Double3>>& surface, QVector<QVector<double>>& valuess, double value = 0)
  {
    for (int i = 0; i < surface.size(); i++)
    {
      QVector<double> values;
      for (int j = 0; j < surface[i].size(); j++)
      {
        values.push_back(value);
      }
      valuess.push_back(values);
    }
  };

  auto encryption = [&](QVector<QVector<Double3>>& surface, int value = 1)
  {
    if (value < 1)
      return;

    double dt = 1. / (value + 1.);

    for (int i = 0; i < surface.size(); i++)
    {
      int size = surface[i].size();

      for (int j = size - 1; j > 0; j--)
      {
        Double3 pt_start = surface[i][j];
        Double3 pt_end = surface[i][j - 1];

        for (int k = 0; k < value; k++)
        {
          double t = (k + 1) * dt;
          Double3 pt = (1 - t) * pt_start + t * pt_end;
          surface[i].insert(j, pt);
        }
      }
    }
  };

  int errorCode = _bladeEditor->updateProfilesGenerator();
  if (errorCode != 0)
    return;

  // Blade 1
  QStringList surfaceList = QStringList() << "Camber" << "LE" << "Pressure" << "TE" << "Suction";
  QVector<double> transparency_bladeSurface = QVector<double>() << 0. << 4. << 0. << 4. << 0.;
  QVector<int> encryption_value = QVector<int>() << 1 << 1 << 1 << 1 << 1;
  for (int i = 0; i < surfaceList.size(); i++)
  {
    QVector<QVector<Double3>> surface = _bladeEditor->getBladeSurface(surfaceList[i]);

    encryption(surface, encryption_value[i]);

    QVector<QVector<double>> valueSuface;

    getValuess(surface, valueSuface, 1.);

    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 0},
      {"transparency", transparency_bladeSurface[i]}
    };

    _3DViewWidget->displaySurfaceFromProfiles(surfaceList[i], surface, &valueSuface, &args);
  }

  // Blade 2
  QStringList surfaceList1 = QStringList() << "Camber" << "LE1" << "Pressure1" << "TE1" << "Suction1";
  QVector<double> transparency_bladeSurface1 = QVector<double>() << 0. << 4. << 0. << 4. << 0.;
  QVector<int> encryption_value1 = QVector<int>() << 1 << 1 << 1 << 1 << 1;
  for (int i = 0; i < surfaceList1.size(); i++)
  {
    QVector<QVector<Double3>> surface = _bladeEditor->getBladeSurface_rotate(surfaceList1[i]);

    encryption(surface, encryption_value1[i]);

    QVector<QVector<double>> valueSuface;

    getValuess(surface, valueSuface, 1.);

    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 0},
      {"transparency", transparency_bladeSurface1[i]}
    };

    _3DViewWidget->displaySurfaceFromProfiles(surfaceList1[i], surface, &valueSuface, &args);
  }
  
  // revolutionSurface
  QVector<double> spans = { 0., 100. }; // _bladeEditor->getSpans();
  for (int i = 0; i < spans.size(); i++)
  {
    QVector<QVector<Double3>> RevolutionSurface = _bladeEditor->getRevolutionSurface(spans[i]);
    QVector<QVector<double>> valueSuface;
    getValuess(RevolutionSurface, valueSuface);

    double transparency = 0.;
    if (i == spans.size() - 1)
      transparency = 8.;
    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 0},
      {"transparency", transparency}
    };

    QString name = "RevolutionSurface" + _bladeEditor->getSpanString(spans[i]);
    _3DViewWidget->displaySurfaceFromProfiles(name, RevolutionSurface, &valueSuface, &args);
  }

  // ThroatSurface
  QString SurfaceName = "ThroatSurface";
  double transparency_ThroatSurface = 0.;
  // W23
  QVector<QVector<Double3>> Throatsurface = _bladeEditor->getThroatSurface(SurfaceName);

  encryption(Throatsurface, 5);
  QVector<QVector<double>> valueSuface;
  getValuess(Throatsurface, valueSuface, 1.);
  QMap<QString, QVariant> args =
  {
    {"colorName", "lightBlue"},
    {"lineWidth", 0},
    {"transparency", transparency_ThroatSurface}
  };

  // W24
  _3DViewWidget->displaySurfaceFromProfiles(SurfaceName, Throatsurface, &valueSuface, &args);
}

void T1dBladeEditorDlg::resizeEvent(QResizeEvent* e)
{
  //redraw();

  return w_QDialog::resizeEvent(e);
}

void T1dBladeEditorDlg::redraw()
{
  return;
}

void T1dBladeEditorDlg::apply()
{
  if (_bladeEditor)
    _bladeEditor->updateBladeEditor();

  setBladeEditor(_bladeEditor);
}

int T1dBladeEditorDlg::update1()
{
  if (_bladeEditor)
    _bladeEditor->updateBladeEditor();

  setBladeEditor(_bladeEditor);

  return 0;
}

// written by Feihong
void T1dBladeEditorDlg::onShowThroatButton()
{
  // W11
  w_QDialog dlg(core_Application::core());
  dlg.setWindowTitle(w_QDialog::tr("Show Throat"));
  // W12
  QVBoxLayout* grid = new QVBoxLayout(&dlg);
  // W13
  w_PropertyHolderWidget* holder = new w_PropertyHolderWidget();

  // W14-1 vtk-3Dshow
  // 1a. w_PropertyHolderWidget* holder_ThroatSurfaceWidget;
  holder_ThroatSurfaceWidget = holder->getHolder(0, 0, 1, 2, tr("Throat surface"));
  // 1b. vis_Widget* _ThroatSurfaceWidget;
  _ThroatSurfaceWidget = vis_Widget::newWidget("vis_WidgetVtk", holder_ThroatSurfaceWidget);
  updateSurfaceView();
  // 1c. 
  holder_ThroatSurfaceWidget->placeWidget(_ThroatSurfaceWidget);

  // W14-2 PropertyList 
  // 2a. w_PropertyHolderWidget* holder_ThroatPropertyWidget;
  holder_ThroatSurfaceWidget = holder->getHolder(0, 2, 1, 2, tr("Throat propertylist"));
  // 2b. TPropertyInputWidget* _ThroatPropertyWidget;
  _ThroatPropertyWidget = new TPropertyInputWidget(holder_ThroatSurfaceWidget);
  // 2c. 
  QVector<property_t*> properties = QVector<property_t*>()
    << _bladeEditor->property("o1tip") << _bladeEditor->property("o1mean")
    << _bladeEditor->property("o1hub") << _bladeEditor->property("ThroatArea");
  // 2d.
  _ThroatPropertyWidget->setProperties(properties, true);
  // 2e.
  holder_ThroatSurfaceWidget->placeWidget(_ThroatPropertyWidget);

  // W15
  grid->addWidget(holder);
  // W16
  setLayout(grid);
  setFocusPolicy(Qt::StrongFocus);

  if (dlg.exec() == w_QDialog::Accepted) {}
}

void T1dBladeEditorDlg::updateSurfaceView()
{
  if (!holder_ThroatSurfaceWidget || !_ThroatSurfaceWidget)
    return;

  // W21 
  auto getValuess = [&](QVector<QVector<Double3>>& surface, QVector<QVector<double>>& valuess, double value = 0)
  {
    for (int i = 0; i < surface.size(); i++)
    {
      QVector<double> values;
      for (int j = 0; j < surface[i].size(); j++)
      {
        values.push_back(value);
      }
      valuess.push_back(values);
    }
  };
  auto encryption = [&](QVector<QVector<Double3>>& surface, int value = 1)
  {
    if (value < 1)
      return;

    double dt = 1. / (value + 1.);

    for (int i = 0; i < surface.size(); i++)
    {
      int size = surface[i].size();

      for (int j = size - 1; j > 0; j--)
      {
        Double3 pt_start = surface[i][j];
        Double3 pt_end = surface[i][j - 1];

        for (int k = 0; k < value; k++)
        {
          double t = (k + 1) * dt;
          Double3 pt = (1 - t) * pt_start + t * pt_end;
          surface[i].insert(j, pt);
        }
      }
    }
  };

  // W22
  QString SurfaceName = "ThroatSurface";
  double transparency_ThroatSurface = 0.;
  int encryption_value = 1;
  // W23
  QVector<QVector<Double3>> Throatsurface = _bladeEditor->getThroatSurface(SurfaceName);

  encryption(Throatsurface, encryption_value);
  QVector<QVector<double>> valueSuface;
  getValuess(Throatsurface, valueSuface, 1.);
  QMap<QString, QVariant> args =
  {
    {"colorName", "lightBlue"},
    {"lineWidth", 0},
    {"transparency", transparency_ThroatSurface}
  };

  // W24
  _ThroatSurfaceWidget->displaySurfaceFromProfiles(SurfaceName, Throatsurface, &valueSuface, &args);
}

//-------------------------------------------------------
T1dBladeCurveBaseWidget::T1dBladeCurveBaseWidget(QWidget* parent) :
  draw_TopologyInteractiveEditorWidget(parent)
{
  dlg = dynamic_cast<T1dBladeEditorDlg*>(parent);
  initializePage();
}

void T1dBladeCurveBaseWidget::initializePage()
{
  if (w_QAction* a = new w_QAction(QPixmap(":images/set.png"), QObject::tr("Config"), this))
  {
    connect(a, SIGNAL(triggered()), this, SLOT(config()));
    QVector<w_QAction*> LL;
    LL.push_back(a);
    addAdditionalRightClickActions(LL);
  }

}

void T1dBladeCurveBaseWidget::setTopology()
{

}

void T1dBladeCurveBaseWidget::updateTopology()
{
  if (!_topology)
    return;
  if (getBladeEditor())
  {
    getBladeEditor()->updateTopology(_topology);
  }

  draw_TopologyInteractiveEditorWidget::setTopology(_topology);

  replot();
}

T1dBladeEditor* T1dBladeCurveBaseWidget::getBladeEditor()
{
  if (T1dBladeEditor* editor = dynamic_cast<T1dBladeEditor*>(_topology->parent("T1dBladeEditor")))
    if (editor)
      return editor;
  return nullptr;
}

void T1dBladeCurveBaseWidget::redraw()
{
 // updateTopology();

  replot();
  return;
}

void T1dBladeCurveBaseWidget::config()
{

}

void T1dBladeCurveBaseWidget::resizeEvent(QResizeEvent* e)
{
  redraw();

  return w_TTWidget::resizeEvent(e);
}

void T1dBladeCurveBaseWidget::handleValueChangedFinished(QVariant v)
{
  draw_TopologyInteractiveEditorWidget::handleValueChangedFinished(v);

  updateCurve();
}

void T1dBladeCurveBaseWidget::updateCurve()
{
  QString curvepath = getSelectedCurveName();
  if (curvepath.size() < 2)
    return;
  curve_Curve* curveSelected = getSelectedCurve(curvepath);

  getBladeEditor()->updateSelectedCurve(_topology, curvepath, curveSelected);

}

//-------------------------------------------------------
T1dBladeMeridionalCurveWidget::T1dBladeMeridionalCurveWidget(QWidget* parent) :
  T1dBladeCurveBaseWidget(parent)
{

  

}

void T1dBladeMeridionalCurveWidget::initializePage()
{


}

void T1dBladeMeridionalCurveWidget::updateCurve()
{
  // T1dBladeCurveBaseWidget::updateCurve();
}

//-------------------------------------------------------
T1dBladeConformalMappingCurveWidget::T1dBladeConformalMappingCurveWidget(QWidget* parent) :
  T1dBladeCurveBaseWidget(parent)
{

  initializePage();
}

void T1dBladeConformalMappingCurveWidget::initializePage()
{
//  getPlot()->setUnitCatY(TUnit::angle);

  getPlot()->setKeepAspect(false);
}

void T1dBladeConformalMappingCurveWidget::updateCurve()
{
  if (!getBladeEditor())
    return;

  if (getBladeEditor()->_bladeEditorType == T1dBladeEditor::bladeMetalAngleAndThickness)
    return;

  T1dBladeCurveBaseWidget::updateCurve();

}

void T1dBladeConformalMappingCurveWidget::config()
{

  if (!getBladeEditor())
    return;

  w_PropertyHolderDialog dlg(core_Application::core());
  dlg.setWindowTitle(w_PropertyHolderDialog::tr("Config"));

  w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);

  //1. Blade angle input
  w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Blade angle input");

  QVector<QVector<double>> vAngle;
  getBladeEditor()->getBladeAngleFromMThetaCurves(vAngle);

  forLoop(i, vAngle.size())
  {
    if (w_PropertyHolderWidget* h = holder1->addHolder())
    {
      QLabel* label = new QLabel(h);
      if (i == 0)
        label->setText("Hub: ");
      else if (i == 1 && vAngle.size() == 3)
          label->setText("Mean: ");
      else
        label->setText("Tip: ");
      h->placeWidget(label, 0, 0, 1, 1);

      if (w_Property* w = w_Property::getPropertyWidget(&vAngle[i][0], "beta1b", TUnit::angle, h))
      {
        w->setAutoSave();
        h->placeWidget(w, 0, 1, 1, 1);
      }
      if (w_Property* w = w_Property::getPropertyWidget(&vAngle[i][1], "beta2b", TUnit::angle, h))
      {
        w->setAutoSave();
        h->placeWidget(w, 0, 2, 1, 1);
      }
      if (w_Property* w = w_Property::getPropertyWidget(&vAngle[i][2], "wrap angle", TUnit::angle, h))
      {
        w->setAutoSave();
        h->placeWidget(w, 0, 3, 1, 1);
      }

    }
  }


  //2. Stacking
  w_PropertyHolderWidget* holder2 = holder->getHolder(1, 0, 1, 2, "Stacking");
  QStringList _allStackingType = QStringList() << tr("LE") << tr("TE");
  _stackingType = _allStackingType[getBladeEditor()->getStackingType()];

  w_Property* wType = nullptr;
  if (wType = w_Property::getPropertyWidget(&_stackingType, QObject::tr("Stacking: "), holder2, &_allStackingType, true, false, w_Property::HLayout))
  {
    wType->setAutoSave();
    holder2->placeWidget(wType, 0, 0, 1, 1);
    connect(wType, SIGNAL(valueChanged()), this, SLOT(stackingTypeChange()));
  }

  QVector<double> deltaAngle = getBladeEditor()->getDeltaThetaFromMThetaCurves();

//  if (w_PropertyHolderWidget* h = holder2->addHolder())
  if (w_PropertyHolderWidget* h = holder2->getHolder(1, 0, 1, 2, "Delta theta of stacking"))
  {
    QLabel* label = new QLabel(h);

    label->setText("Delta theta: ");
    h->placeWidget(label, 0, 0, 1, 1);

    for (int i = 0; i < deltaAngle.size(); i++)
    {
      QString spanName = getBladeEditor()->getSpanNameFromSpanIndex(i);

      if (w_Property* w = w_Property::getPropertyWidget(&deltaAngle[i], spanName, TUnit::angle, h))
      {
        w->setAutoSave();
        h->placeWidget(w, 0, i + 1, 1, 1);
      }
    }
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    getBladeEditor()->setMThetaCurvesFromBladeAngle(vAngle);

    getBladeEditor()->setMThetaCurvesFromStackingSetting(deltaAngle);

    updateTopology();
  }
}

void T1dBladeConformalMappingCurveWidget::stackingTypeChange()
{
  if (_stackingType == "LE")
    getBladeEditor()->setStackingType(0);
  else
    getBladeEditor()->setStackingType(1);

}

//-------------------------------------------------------
T1dBladeAngleCurveWidget::T1dBladeAngleCurveWidget(QWidget* parent) :
  T1dBladeCurveBaseWidget(parent)
{

  initializePage();
}

void T1dBladeAngleCurveWidget::initializePage()
{
  getPlot()->setKeepAspect(false);

}

void T1dBladeAngleCurveWidget::updateCurve()
{
  if (!getBladeEditor())
    return;

  if (getBladeEditor()->_bladeEditorType == T1dBladeEditor::bladeConformalMapping)
    return;

  T1dBladeCurveBaseWidget::updateCurve();

}

//-------------------------------------------------------
T1dBladeThicknessCurveWidget::T1dBladeThicknessCurveWidget(QWidget* parent) :
  T1dBladeCurveBaseWidget(parent)
{
  initializePage();
}

void T1dBladeThicknessCurveWidget::initializePage()
{
  getPlot()->setKeepAspect(false);
}


