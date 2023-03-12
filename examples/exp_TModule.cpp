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


#include "exp_TModule.h"

#include "exp_Object.h"
#include "core_Application.h"
#include "w_TMainWindow.h"
#include "w_TTaskWindow.h"
#include "exp_Window.h"


#include "util_ModuleDefine.h"

REGISTER_MODULE_CLASS(exp_TModule);
// T1dModule T1dObject "1d" "1D" "oned" "1d" ""
TT_ADD_MODULE(exp_TModule, exp_Object, "examples", "EXAMPLES", "exp", ":/images/task-icon-examples.png", "Examples", "Examples", "geom", "", "GTTS_ALLOW_EXAMPLE_MODULE");

exp_TModule::exp_TModule(QMainWindow* parent, Qt::WindowFlags wflags) :
  TModule(parent, (Qt::WindowFlags)999999)
{
  moduleName = "exp_TModule";
  defaultWindow = "exp_Window";
  //iconName = ":/images/geometry_01.png";
  moduleObjectClass = "exp_Object";
  _moduleOptionClassName = "exp_UserOptions";

  createDefaultWindow();

  const char *sname = TObject::shortName(moduleObjectClass.c_str());
  if (!(tree = CURRENT_STUDY->object(sname)))
    tree = fillDefaultTree();


  if (defaultTaskWindow)
    defaultTaskWindow->enableUndoRedo();	
}

TObject * exp_TModule::fillDefaultTree(void)
{
  TObject * geom = exp_Object::createDefaultTree();

  return geom;
}

TTaskWindow * exp_TModule::createDefaultWindow(void)
{
  return TModule::createDefaultWindow();
}

void exp_TModule::updateAfterModuleClick()
{
	showAllDockWidget(false);

	TModule::updateAfterModuleClick();
}
