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


#include "w_TModule.h"
#include "util_TObject.h"
#include "util_Study.h"

class T_EXPORT_EXAMPLES exp_TModule : public TModule
{
  Q_OBJECT

public:
  exp_TModule ( QMainWindow* parent = 0, Qt::WindowFlags wflags = 0);

public:
  TObject * fillDefaultTree(void);
  TTaskWindow * createDefaultWindow(void);

  virtual void updateAfterModuleClick();
};
