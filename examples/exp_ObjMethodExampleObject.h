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

class T_EXPORT_EXAMPLES exp_ObjMethodExampleObject : public TObject
{
  T_OBJECT;

public:
  exp_ObjMethodExampleObject(QString object_n = "", TObject *iparent = NULL);
  virtual ~exp_ObjMethodExampleObject() {}
  void setAnInt(const int& i) { anInt = i; }
  int getAnInt()const { return anInt; }

  void setABool(const bool& i) { aBool = i; }
  bool getABool()const { return aBool; }

  void setADouble(const double& i) { aDouble = i; }
  double getADouble()const { return aDouble; }

  void setAString(const QString& i) { aString = i; }
  QString getAString()const { return aString; }

  void setAStringList(const QStringList& i) { aStringList = i; }
  QStringList getAStringList()const { return aStringList; }

  void setAQVectorInt(const QVector<int>& i) { aQVectorInt = i; }
  QVector<int> getAQVectorInt()const { return aQVectorInt; }

  void setAQVectorDouble(const QVector<double>& i) { aQVectorDouble = i; }
  QVector<double> getAQVectorDouble()const { return aQVectorDouble; }

  void setAQVectorDouble2(const QVector<Double2>& i) { aQVectorDouble2 = i; }
  QVector<Double2> getAQVectorDouble2()const { return aQVectorDouble2; }

  void setAQVectorVectorDouble(const QVector<QVector<double>>& i) { aQVectorVectorDouble = i; }
  QVector<QVector<double>> getAQVectorVectorDouble()const { return aQVectorVectorDouble; }

private:
  int anInt;
  bool aBool;
  double aDouble;
  QString aString;
  QStringList aStringList;
  QVector<int> aQVectorInt;
  QVector<double> aQVectorDouble;
  QVector<Double2> aQVectorDouble2;
  QVector<QVector<double>> aQVectorVectorDouble;
};
