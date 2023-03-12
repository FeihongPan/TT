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

#ifndef INSCRIBEDCIRCLECALCULATORWIDGET_H
#define INSCRIBEDCIRCLECALCULATORWIDGET_H

#include "w_TTWidget.h"
#include "InscribedCircleCalculator.h"

class w_PropertyHolderWidget;
class draw_TopologyInteractiveEditorWidget;
class w_QPushButton;

class T_EXPORT_1D InscribedCircleCalculatorWidget: public w_TTWidget
{
	Q_OBJECT;

public:
	InscribedCircleCalculatorWidget(QWidget* parent = 0);
	void setInscribedCircleCalculator(InscribedCircleCalculator* ICC);
	virtual ~InscribedCircleCalculatorWidget();

private:
	w_PropertyHolderWidget* holder;	
	w_PropertyHolderWidget* holder_zrCurveWidget;
	w_PropertyHolderWidget* holder_AreaWidget;
	w_PropertyHolderWidget* holder_ConfiglWidget;

	draw_TopologyInteractiveEditorWidget* _zrCurveWidget;
	draw_TopologyInteractiveEditorWidget* _AreaWidget;

	w_QPushButton* btn_LoadCurves;
	w_QPushButton* btn_CalculateCrossSection;

	InscribedCircleCalculator* _InscribedCircleCalculator;

private:
	void update_zrCurveShow();
	void update_areaCurveShow();

private slots:
	void onLoadCurves();
	void onCalculateCrossSection();
};

#endif
