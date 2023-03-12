/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2021] TurboTides LLC
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TurboTides LLC and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TurboTides LLC and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TurboTides LLC.
 */

#pragma once

#include "util_TObject.h"

class exp_QtContainerExampleObject : public TObject
{
  T_OBJECT;

public:
  exp_QtContainerExampleObject(QString object_n = "", TObject *iparent = NULL);
  virtual ~exp_QtContainerExampleObject() {}

private:
  int anInt;
  bool aBool;
  double aDouble;
  QString aString;
  QStringList aStringList;
  QVector<int> aQVectorInt;
  QVector<double> aQVectorDouble;
  QVector<Double2> aQVectorDouble2;
  QVector<QVector<double> > aQVectorVectorDouble;
};
