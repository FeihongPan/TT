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

#pragma once

#include "w_TTaskWindow.h"

class draw_TopologyInteractiveEditorWidget;
class T_EXPORT_EXAMPLES exp_PlotWindow : public TTaskWindow
{
  Q_OBJECT

public:
  exp_PlotWindow(QWidget* parent, Qt::WindowFlags wflags = 0);

  virtual bool displayObject(TObject *o, bool displayEmptyObject = true);
  virtual bool addObject(TObject *o);
  virtual bool removeObject(TObject *o);

private:
  draw_TopologyInteractiveEditorWidget* _plot = nullptr;
};
