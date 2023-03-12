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

#include "InscribedCircleCalculatorWidget.h"
#include "draw_TopologyInteractiveEditorWidget.h"
#include "draw_TopologyInteractiveEditor.h"
#include "w_PropertyHolderWidget.h"
#include "w_PropertyHolderDialog.h"


InscribedCircleCalculatorWidget::InscribedCircleCalculatorWidget(QWidget* parent) : w_TTWidget(parent)
{
  // W01
  QGridLayout* grid = new QGridLayout;
  // W02
  w_PropertyHolderWidget* holder = new w_PropertyHolderWidget();

  // W031 Add ZR sections topology show
  // a. w_PropertyHolderWidget* holder_zrCurveWidget;
  holder_zrCurveWidget = holder->getHolder(0, 0, 1, 2, tr("ZR Sections"));
  // b. draw_TopologyInteractiveEditorWidget* _zrCurveWidget;
  _zrCurveWidget = new draw_TopologyInteractiveEditorWidget(holder_zrCurveWidget);
  // c. placeWidget
  holder_zrCurveWidget->placeWidget(_zrCurveWidget);

  // W032 Add CrossSectionsArea Curve
  holder_AreaWidget = holder->getHolder(0, 2, 1, 2, tr("Cross Sections Area"));
  _AreaWidget = new draw_TopologyInteractiveEditorWidget(holder_AreaWidget);
  holder_AreaWidget->placeWidget(_AreaWidget);

  // W033 Button Config
  holder_ConfiglWidget = holder->getHolder(1, 0, 1, 4, tr("Config"));
  // a. 
  btn_LoadCurves = holder_ConfiglWidget->addButton(QObject::tr("Load curves"), 0, 0, 1, 1);
  connect(btn_LoadCurves, SIGNAL(clicked()), this, SLOT(onLoadCurves()));
  // b.
  btn_CalculateCrossSection = holder_ConfiglWidget->addButton(QObject::tr("Calculate crossSection"), 0, 1, 1, 1);
  connect(btn_CalculateCrossSection, SIGNAL(clicked()), this, SLOT(onCalculateCrossSection()));

  // W04
  grid->addWidget(holder);
  // W05
  setLayout(grid);
  setFocusPolicy(Qt::StrongFocus);
}

void InscribedCircleCalculatorWidget::update_zrCurveShow()
{
  if (!_zrCurveWidget)
    return;
  if (_InscribedCircleCalculator->getTopo(0))
  {
    _zrCurveWidget->setSizeHint(QSize(600, 400));
    _zrCurveWidget->setTopology(_InscribedCircleCalculator->getTopo(0));
  }
}

void InscribedCircleCalculatorWidget::update_areaCurveShow()
{
  if (!_AreaWidget)
    return;
  if (_InscribedCircleCalculator->getTopo(1))
  {
    _AreaWidget->setSizeHint(QSize(600, 400));
    _AreaWidget->setTopology(_InscribedCircleCalculator->getTopo(1));
  }
}

void InscribedCircleCalculatorWidget::setInscribedCircleCalculator(InscribedCircleCalculator* ICC)
{
  if (!ICC)
    return;
  else
    _InscribedCircleCalculator = ICC;

  update_zrCurveShow();
  update_areaCurveShow();
}

void InscribedCircleCalculatorWidget::onLoadCurves()
{
  if (!_zrCurveWidget)
    return;
  // W11
  //_InscribedCircleCalculator->newLoadCurves();
  _InscribedCircleCalculator->LoadCurves();
  // W12
  if (curve_Topology* topology = _InscribedCircleCalculator->getTopo(0))
  {
    _zrCurveWidget->setSizeHint(QSize(1000, 1000));
    // W13
    _zrCurveWidget->setTopology(topology);
  }
}

void InscribedCircleCalculatorWidget::onCalculateCrossSection()
{
  if (!_AreaWidget)
    return;
  // W21
  _InscribedCircleCalculator->newCalculateCrossSection();
  // W22
  if (curve_Topology* topology = _InscribedCircleCalculator->getTopo(1))
  {
    _AreaWidget->setSizeHint(QSize(1000, 600));
    // W23
    _AreaWidget->setTopology(topology);
  }
}

InscribedCircleCalculatorWidget::~InscribedCircleCalculatorWidget()
{
  if (_InscribedCircleCalculator) delete _InscribedCircleCalculator;
}

