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

#include "util_ModuleRoot.h"

class T_EXPORT_EXAMPLES exp_Object : public util_ModuleRoot
{
  T_OBJECT;
public:
  exp_Object(QString object_n = "", TObject *iparent = NULL);
  ~exp_Object()
  {
  }

  static TObject * createDefaultTree(void);

};
