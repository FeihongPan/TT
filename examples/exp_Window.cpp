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

#include "exp_Window.h"

#include "exp_Object.h"

#include "core_Application.h"
#include "util_Study.h"
#include "util_ObjectMethod.h"
#include "util_OUtil.h"
#include "util_TT.h"
#include "util_Report.h"
#include "util_CurveNurbs.h"
#include "w_TMainWindow.h"
#include "w_TModule.h"
#include "w_PropertyHolderDialog.h"
#include "w_QFile.h"
#include "w_QFileDialog.h"
#include "mshTopo_TMMesh.h"

REGISTER_TASKWINDOW_CLASS(exp_Window);

exp_Window::exp_Window(QWidget* parent, Qt::WindowFlags wflags)
  : TTaskWindow(parent, wflags)
{
  setWindowTitle(tr("Example Window"), "Example Window");
}

exp_Window::~exp_Window()
{
}
