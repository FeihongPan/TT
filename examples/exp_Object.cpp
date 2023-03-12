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

#include "exp_Object.h"
#include "exp_Window.h"

#include "util_Study.h"
#include "w_TModule.h"
#include "util_Version.h"

REGISTER_OBJECT_CLASS(exp_Object, "examples", util_ModuleRoot);

exp_Object::exp_Object(QString object_n, TObject *iparent) :
  util_ModuleRoot(object_n, iparent)
{
  INIT_OBJECT;
}

TObject* exp_Object::createDefaultTree(void)
{
  // get the parent first
  auto sname = TObject::shortName("exp_Object");
  auto o = CURRENT_STUDY->object(sname);

  if (!o)
  {
    if (o = TObject::new_object("exp_Object", sname, CURRENT_STUDY))
    {
      QStringList objs = {
        "exp_CppExampleObject", "01 - C++",
        "exp_QtContainerExampleObject", "02 - QT Container",
        "exp_ObjExampleObject", "03 - TObject",
        "exp_ObjMethodExampleObject", "04 - TObject Method",
        "exp_w_PropertyExampleObject", "05 - wProperty Widgets",     
        "exp_PlotExampleObject", "06 - Plot",
        "exp_CurveToplogyExampleObject", "07 - Topology & Task Window",
        "exp_VtkExampleObject", "08 - VTK & other 3D Plots",
        "exp_OptimizationExampleObject", "09 - Optimization with parameters",
        "exp_CurveToplogyShroudMultiCurves", "10 - Shroud Multi Curves",
        "exp_BladeImport", "11 - BladeInput",
        "exp_BladeImportedByParameterization", "12 - Blade Imported By Parameterization",
        "exp_DevPlot", "13 - performance plot for development",
        "exp_VtkBaseExampleObject", "14 - VTK Base Examples"
      };

      for (int i = 0; i < objs.size(); i += 2)
      {
        auto c = objs[i];
        auto d = objs[i + 1];
        if (auto obj = TObject::new_object(c, TObject::shortName(c), o))
        {
          obj->set_display_name(d);
        }
      }
    }
  }
  
  return o;
}
