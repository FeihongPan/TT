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

#include "exp_ObjectConfigDialog.h"
#include "w_PropertyHolderDialog.h"
#include "w_TModule.h"
#include "exp_w_PropertyExampleObject.h"

REGISTER_OBJECT_CLASS(exp_ObjectConfig1, "exp_ObjectConfig1", TObject);
REGISTER_OBJECT_CLASS(exp_ObjectConfig2, "exp_ObjectConfig2", TObject);

static QStringList _intSelectors = QStringList{
  "Int with value 0",
  "Int with value 1",
  "Int with value 2"
};

static QStringList _boolSelectors = QStringList{
  "Bool false",
  "Bool true"
};

static QStringList _doubleSelectors = QStringList{
  "0.01",
  "0.05",
  "0.9"
};

exp_ObjectConfig1::exp_ObjectConfig1(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;
  
  auto set_page = [&](const QStringList& L, const QString& page_name)
  {
    foreach(auto s, L)
    {
      if (auto p = property(s))
      {
        SET_PROPERTY_PAGE(p, page_name, "");
      }
    }
  };

  DEFINE_SCALAR_INIT(double, double1, 0, 0, NULL, TUnit::pressure);
  setPropertyComboString("double1", &_doubleSelectors);
  DEFINE_SCALAR_INIT(double, double2, 0, 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, double3, 0, 0, NULL, TUnit::pressure);

   

  DEFINE_SCALAR_INIT(int, int1, 0, 0, NULL, NULL);
  setPropertyComboString("int1", &_intSelectors);
  DEFINE_SCALAR_INIT(int, int2, 0, 0, NULL, NULL);

  DEFINE_SCALAR_INIT(bool, bool1, 0, 0, NULL, NULL);
  setPropertyComboString("bool1", &_boolSelectors);

  set_page(QStringList({ "double1", "int2" }), "Other");

  set_page(QStringList({ "bool1", }), "Select");
}

exp_ObjectConfig2::exp_ObjectConfig2(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  auto set_page = [&](const QStringList& L, const QString& page_name)
  {
    foreach(auto s, L)
    {
      if (auto p = property(s))
      {
        SET_PROPERTY_PAGE(p, page_name, "");
      }
    }
  };

  DEFINE_SCALAR_INIT(double, double1, 0, 0, NULL, TUnit::pressure);
  setPropertyComboString("double1", &_doubleSelectors);
  DEFINE_SCALAR_INIT(double, double2, 0, 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, double3, 0, 0, NULL, TUnit::pressure);



  DEFINE_SCALAR_INIT(int, int1, 0, 0, NULL, NULL);
  setPropertyComboString("int1", &_intSelectors);
  DEFINE_SCALAR_INIT(int, int2, 0, 0, NULL, NULL);

  DEFINE_SCALAR_INIT(bool, bool1, 0, 0, NULL, NULL);
  setPropertyComboString("bool1", &_boolSelectors);

  set_page(QStringList({ "double2", "int2" }), "Date");

  set_page(QStringList({ "int1", }), "Some");
}

#include "w_ObjectEditDialog.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, ObjectEditExample, "Object Edit Example", 0, 0, "")
{
  if (VARLID_ARG)
  {
    QStringList L = { "object0__", "exp_ObjectConfig1", "Venice",
      "object1__", "exp_ObjectConfig2", "Florence",
      "object2__", "exp_ObjectConfig2", "Pizza",
      "object3__", "exp_ObjectConfig1", "Rome",
    };

    QVector<TObject*> objects;
    for (int i = 0; i < L.size() / 3; i++)
    {
      auto name = L[i * 3];
      auto c= L[i * 3 + 1];
      auto d = L[i * 3 + 2];
      auto o = object->object(name);
      if (!o)
      {
        if (o = TObject::new_object(c, name, object))
        {
          o->set_display_name(d);
        }
      }

      if (o)
        objects.push_back(o);
    }


    w_ObjectEditDialog dlg(core_Application::core(), 0, &objects);
    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;
