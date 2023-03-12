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

#include "exp_PlotWindow.h"
#include "draw_TopologyInteractiveEditorWidget.h"
#include "curve_Topology.h"

REGISTER_TASKWINDOW_CLASS(exp_PlotWindow);

exp_PlotWindow::exp_PlotWindow( QWidget* parent, Qt::WindowFlags wflags ) : 
TTaskWindow ( parent, wflags )
{
  if (QVBoxLayout *layout = new QVBoxLayout(this))
  {
    layout->setMargin(0);

    setFrameStyle(QFrame::NoFrame | QFrame::Plain);

    if (_plot = new draw_TopologyInteractiveEditorWidget(this))
    {
      layout->addWidget(_plot);
    }

    setLayout(layout);
  }

  createActions();
  createMenus();
  createToolBars();
}

bool exp_PlotWindow::displayObject(TObject *o, bool displayEmptyObject)
{
  if (curve_Topology* c = dynamic_cast<curve_Topology*>(o))
  {
    if (_plot)
    {
      _plot->setTopology(c);

      return true;
    }    
  }

  return false;
}

bool exp_PlotWindow::addObject(TObject *o)
{
  return displayObject(o);
}

bool exp_PlotWindow::removeObject(TObject *o)
{
  _plot->setTopology(nullptr);

  return o != nullptr;
}
