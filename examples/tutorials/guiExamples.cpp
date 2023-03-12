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

#include "util_TObject.h"

#include "w_PropertyHolderDialog.h"
#include "w_ObjectTreeWidget.h"
#include "pe_TPropertyInputWidget.h"
#include "w_VectorPropertyWidget.h"
#include "w_PropertyDialog.h"

 /* define an object class example
 *
 */

class test_Object1 : public TObject
{
  T_OBJECT;

public:
  test_Object1(const char *object_n = NULL, TObject *iparent = NULL);
  virtual ~test_Object1() {}

public:
  static QStringList nozzleTypeList;

private:
  QVector<double> doubleVector1;
  QVector<double> doubleVector2;
  QVector<double> doubleVector3;
  QVector<int> intVector1;
  QVector<int> intVector2;

  double double1;
  double double2;
  double double3;
  int int1;
  int int2;
  double double4;
  double double5;
  double double6;

  QString aString;
};

QStringList test_Object1::nozzleTypeList = (QStringList() <<
  "1DConvergeDiverge" <<
  "LavalNozzle" <<
  "type3" <<
  "type4" <<
  "channel" <<
  "1DConvergeDivergeNew" <<
  "1DConvergeDivergeStraightEnd" <<
  "fluent");

REGISTER_OBJECT_CLASS(test_Object1, "testObject1", TObject);

test_Object1::test_Object1(const char *object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector1, 0, 0, 0, NULL, TUnit::pressure);
  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector2, 0, 0, 0, NULL, TUnit::density);
  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector3, 0, 0, 0, NULL, NULL);
  DEFINE_QINT_VECTOR_INIT(intVector1, 0, 0, NULL, NULL);
  DEFINE_QINT_VECTOR_INIT(intVector2, 0, 0, NULL, NULL);

  DEFINE_SCALAR_INIT(double, double1, 0, 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, double2, 0, 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, double3, 0, 0, NULL, TUnit::pressure);

  DEFINE_SCALAR_INIT(double, double4, 0, 0, NULL, TUnit::density);
  DEFINE_SCALAR_INIT(double, double5, 0, 0, NULL, TUnit::density);
  DEFINE_SCALAR_INIT(double, double6, 0, 0, NULL, TUnit::density);

  DEFINE_SCALAR_INIT(int, int1, 0, 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, int2, 0, 0, NULL, NULL);

  DEFINE_QSTRING_INIT(aString, "A string example", 0, NULL, NULL);
  setPropertyComboString("aString", &nozzleTypeList);
}

static void showObject(TObject* o)
{
  QStringList L = QStringList() << o->path();
  TPropertyDialog::setProperty(L, 0, "", true);
}

O_METHOD_BEGIN(TObject, Example00CreateObjects, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    /* find the module root object */
    if (TObject* moduleRoot = object->parent("util_ModuleRoot"))
    {
      printMessage("Current module: %s with class type %s",
        moduleRoot->oname(),
        moduleRoot->cname()
      );

      /* create an test_Object object with automatic name */
      if (TObject* holder = TObject::new_object("TObject", 0, moduleRoot))
      {
        printMessage("Object generated: %s with class type %s",
          holder->oname(),
          holder->cname()
        );

        /* make sure it shows up in the tree */
        TObject::refreshTreeWithVisable(holder);

        /* create an object "testObject1" with test_Object type */
        if (TObject* testObject = TObject::new_object("test_Object1", "testObject1", holder))
        {
          printMessage("Object generated: %s with class type %s",
            testObject->oname(),
            testObject->cname()
          );

          /* make sure it shows up in the tree */
          TObject::refreshTreeWithVisable(testObject);

          /* set an int property value */
          if (property_t* p = testObject->property("int1"))
          {
            p->setValue(158);
          }

          /* set an int vector property value */
          if (property_t* p = testObject->property("intVector1"))
          {
            QVector<int>& a = p->asIntVector();
            a.push_back(1);
            a.push_back(2);
            a.push_back(7);
          }

          /* TODO:
          set testObject aDouble = 57.1
          set testObject aString = "helo"
          set testObject aQVectorDouble = {11.2, 23, 45.6, 22}
          */

          /* try to display the object */
          showObject(testObject);
        }
      }
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example00ShowObject, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    showObject(object);
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example01Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample01");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QLabel* label = holder->addImageLabel(":/images/splash-top.png");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    int i = 5;
    holder->addProperty(&i, "An int");

    bool check = false;
    holder->addProperty(&check, "A check");

    QString fileName;
    if (w_Property* w = holder->addProperty(&fileName, "A File", 0, false, false, false, "TCallbackOpenFileBrowser")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L, true, false)) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example02Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample02");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { QGroupBox* g = w->wrapGroupBox(); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example03Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample03");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Holder 1");
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Holder 2");

    QString aString = "string";
    if (w_Property* w = holder1->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { QGroupBox* g = w->wrapGroupBox(); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example04Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample04");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    QLabel* label = holder->addText("Other options", true, false, true);
    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example05Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample05");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    if (w_Property* w = holder->addProperty(object->property("aString"))) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example06Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample06");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Object selection tree");
    QVector<TObject*> selections;
    selections.push_back(object);
    w_ObjectTreeWidget* objectTree = new w_ObjectTreeWidget(&dlg);
    objectTree->getTree()->displayObjectTree(object->parent(), &selections);
    holder1->placeWidget(objectTree, 1, 0, 1, 2);

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder2->addProperty(&selection, "Please choose", &L)) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selected objects");
      foreach(TObject* o, selections)
        eprintf("%s", o->oname());
    }

    return true;
  }
}
O_METHOD_END;


O_METHOD_BEGIN(TObject, Example07Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample07");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Property input");
    TPropertyInputWidget* w = new TPropertyInputWidget(holder1);
    QVector<property_t *> properties = QVector<property_t *>() << object->property("double1") << object->property("double2") << object->property("double3");
    w->setProperties(properties, true);
    holder1->placeWidget(w, 1, 0, 1, 2);

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder2->addProperty(&selection, "Please choose", &L)) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("double1 = %g", object->property("double1")->toDouble());
    }

    return true;
  }
}
O_METHOD_END;

#include "w_VectorPropertyWidgetH.h"
#include "w_VectorPropertyWidgetV.h"
O_METHOD_BEGIN(TObject, Example08Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample08");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Table input");
    QVector<QVector<property_t *> > properties(2);
    //object->property("double1")->setValue(2); 
    properties[0].push_back(object->property("double1"));
    object->property("double2")->setValue(5); properties[0].push_back(object->property("double2"));
    object->property("int1")->setValue(4); properties[1].push_back(object->property("int1"));
    object->property("int2")->setValue(6); properties[1].push_back(object->property("int2"));

    QStringList verticalNames = QStringList() << "row 1" << "row 2";
    w_VectorPropertyWidget* w = new w_VectorPropertyWidgetH(holder1);
    w->setProperties(properties, &verticalNames);
    holder1->placeWidget(w, 1, 0, 1, 2);

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("double1 = %g", object->property("double1")->toDouble());
    }

    return true;
  }
}
O_METHOD_END;


O_METHOD_BEGIN(TObject, Example09Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample09");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Table input");
    w_VectorPropertyWidget* w = new w_VectorPropertyWidgetV(holder1);
    object->property("doubleVector1")->asDoubleVector().clear();
    object->property("doubleVector2")->asDoubleVector().clear();
    object->property("intVector2")->asIntVector().clear();
    for (int i = 0; i < 5; i++)
    {
      object->property("doubleVector1")->asDoubleVector().push_back(i * 5);
      object->property("doubleVector2")->asDoubleVector().push_back(i);
      object->property("intVector2")->asIntVector().push_back(i + 5);
    }
    QVector<property_t*> properties;
    properties.push_back(object->property("doubleVector1"));
    properties.push_back(object->property("doubleVector2"));
    properties.push_back(object->property("intVector1"));
    QStringList verticalNames = QStringList() << "row 1" << "row 2" << "row 3";
    w->setProperties(properties, &verticalNames);
    holder1->placeWidget(w, 1, 0, 1, 2);

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("doubleVector1[0] = %g", object->property("doubleVector1")->asDoubleVector()[0]);
    }

    return true;
  }
}
O_METHOD_END;

static bool widgetChangedCallback(w_TTWidget* w, w_PropertyHolderWidget* holder)
{
  if (w == holder->findMappedWidget("chooseShowSplash"))
  {
    if (QLabel* label = dynamic_cast<QLabel*>(holder->findMappedWidget("splashLabel")))
    {
      if (w_Property* p = dynamic_cast<w_Property*>(w))
      {
        if (p->getProperty()->toString() == "Show splash")label->show();
        else label->hide();
      }
      return true;
    }
  }
  return false;
}
O_METHOD_BEGIN(TObject, Example10Widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample10");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    holder->setCallback(widgetChangedCallback);
    QLabel* label = holder->addImageLabel(":/images/splash-top.png");
    holder->addMappedWidget("splashLabel", label);

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    bool check = false;
    holder->addProperty(&check, "A check");

    QStringList L = QStringList() << "Show splash" << "Hide splash";
    QString selection = L[0];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { holder->addMappedWidget("chooseShowSplash", w); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;


#include "w_BasePropertyHolderDialog.h"
#include "w_VectorPropertyWidgetV.h"
#include "w_VectorPropertyWidgetH.h"
O_METHOD_BEGIN(test_Object1, Example11vectorDialogTest, 0, 0, 0, "")
{
  // argument section starts, should be in proper odering


  if (VARLID_ARG)
  {
    w_BasePropertyHolderDialog dlg;

    w_VectorPropertyWidget* w = new w_VectorPropertyWidgetV(&dlg);

    object->property("doubleVector1")->asDoubleVector().clear();
    object->property("doubleVector2")->asDoubleVector().clear();
    object->property("doubleVector3")->asDoubleVector().clear();
    object->property("intVector2")->asIntVector().clear();

    for (int i = 0; i < 10; i++)
    {
      object->property("doubleVector1")->asDoubleVector().push_back(i * 5);
      object->property("doubleVector2")->asDoubleVector().push_back(i);
      object->property("doubleVector3")->asDoubleVector().push_back(i + 11);

      object->property("intVector2")->asIntVector().push_back(i + 5);
    }

    eprintf("1: doubleVector1[0] = %g", object->property("doubleVector1")->asDoubleVector()[0]);
    eprintf("1: intVector2[0] = %d", object->property("intVector2")->asIntVector()[0]);
    eprintf("1: doubleVector3[0] = %g", object->property("doubleVector3")->asDoubleVector()[0]);

    QVector<property_t*> properties;
    properties.push_back(object->property("doubleVector1"));
    properties.push_back(object->property("doubleVector2"));
    properties.push_back(object->property("doubleVector3"));
    properties.push_back(object->property("intVector1"));
    properties.push_back(object->property("intVector2"));

    QStringList verticalNames = QStringList() << "row 1" << "row 2" << "row 3" << "row 4";

    w->setProperties(properties, &verticalNames);
    dlg.addWidget(w);
    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("2: doubleVector1[0] = %g", object->property("doubleVector1")->asDoubleVector()[0]);
      eprintf("2: intVector2[0] = %d", object->property("intVector2")->asIntVector()[0]);
      eprintf("2: doubleVector3[0] = %g", object->property("doubleVector3")->asDoubleVector()[0]);
    }

#if 0
    w->setProperties(properties, &verticalNames);
    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("2: doubleVector1[0] = %g", object->property("doubleVector1")->asDoubleVector()[0]);
      eprintf("2: intVector2[0] = %d", object->property("intVector2")->asIntVector()[0]);
      eprintf("2: doubleVector3[0] = %g", object->property("doubleVector3")->asDoubleVector()[0]);
    }
#endif
    return true;
  }
}
O_METHOD_END;


#include "w_PropertyHolderDialog.h"
#include "w_ObjectTreeWidget.h"
#include "pe_TPropertyInputWidget.h"
O_METHOD_BEGIN(test_Object1, Example11vector2dDialogTest, 0, 0, 0, "")
{
  // argument section starts, should be in proper odering


  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("A Property Test Dialog");




    /************************************************************/
    /***************************** Any inputs *******************/
    /************************************************************/
    // Any inputs
    double ADouble = 1.3;
    {
      w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
      //holder->addText("Other inputs", false, false, true);      

      w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Holder 1");
      w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Holder 2");

      QLabel* label = holder1->addImageLabel(":/images/splash-top.png");
      QString aString = "string";
      if (w_Property* w = holder1->addProperty(&aString, "A String"))
      {
      }

      if (w_Property* w = holder2->addProperty(&ADouble, "A Double"))
      {
      }


      if (w_PropertyHolderWidget* holder3 = holder->getHolder(-1, 0, 1, 4, "An object property"))
      {
        holder3->addProperty(object->property("nozzleType"));

        int ii = 5;
        holder3->addProperty(&ii, "Another int");

        double dd = 3.4;
        holder3->addProperty(&dd, "Another double");
      }

      int i = 5;
      holder->addProperty(&i, "An int");




      bool check = false;
      holder->addProperty(&check, "A check");



      QString fileName;
      if (w_Property* w = holder->addProperty(&fileName, "A File", 0, false, false, false, "TCallbackOpenFileBrowser"))
      {

      }


      QString selection;
      QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
      selection = L[1];
      if (w_Property* w = holder->addProperty(&selection, "Please choose", &L))
      {
        QGroupBox* g = w->wrapGroupBox();
      }
    }

    /************************************************************/
    /***************************** object tree ******************/
    /************************************************************/
    TObject* parent = object->parent();
    QVector<TObject*> selections;
    selections.push_back(object);
    {
      w_PropertyHolderWidget* holder = dlg.getHolder(0, 2, 2, 1);
      holder->addText("Object selection tree", false, false, false);

      w_ObjectTreeWidget* objectTree = new w_ObjectTreeWidget(&dlg);
      objectTree->getTree()->displayObjectTree(parent, &selections);
      holder->placeWidget(objectTree, 1, 0, 1, 2);
    }


    /************************************************************/
    /********************* property input widget ****************/
    /************************************************************/
    {
      w_PropertyHolderWidget* holder = dlg.getHolder(0, 1);

      TPropertyInputWidget* w = new TPropertyInputWidget(&dlg);

      QVector<property_t *> properties = QVector<property_t *>()
        << object->property("double1")
        << object->property("double2")
        << object->property("double3");
      w->setProperties(properties, true);


      holder->addText("Property input", false, false, false);
      holder->placeWidget(w, 1, 0, 1, 2);
    }


    /************************************************************/
    /********************* Special properties *******************/
    /************************************************************/
    QVector<QVector<property_t *> > properties(3);
    object->property("double1")->setValue(2); properties[0].push_back(object->property("double1"));
    object->property("double2")->setValue(5); properties[0].push_back(object->property("double2"));
    object->property("double3")->setValue(7); properties[0].push_back(object->property("double3"));

    object->property("int1")->setValue(4); properties[1].push_back(object->property("int1"));
    object->property("int2")->setValue(6); properties[1].push_back(object->property("int2"));

    object->property("double4")->setValue(2); properties[2].push_back(object->property("double4"));
    object->property("double5")->setValue(9); properties[2].push_back(object->property("double5"));
    object->property("double6")->setValue(7); properties[2].push_back(object->property("double6"));

    eprintf("1: double1 = %g", object->property("double1")->toDouble());
    QStringList verticalNames = QStringList() << "row 1" << "row 2" << "row 3";
    w_VectorPropertyWidget* left;
    {
      w_PropertyHolderWidget* holder = dlg.getHolder(1, 0);
      holder->addText("Left", false, false, false);

      w_VectorPropertyWidget* w = new w_VectorPropertyWidgetH(&dlg);
      left = w;
      //w->setSizeHint(QSize(600, 600));
      w->setProperties(properties, &verticalNames);

      holder->placeWidget(w, 1, 0, 1, 2);
    }

    QVector<QVector<property_t *> > properties1 = properties;
    {
      w_PropertyHolderWidget* holder = dlg.getHolder(1, 1);
      holder->addText("Right", false, false, false);

      w_VectorPropertyWidget* w = new w_VectorPropertyWidgetH(&dlg);
      w->setProperties(properties1, &verticalNames);
      holder->placeWidget(w, 1, 0, 1, 2);
    }


    /************************************************************/
    /*********************** start dialog ***********************/
    /************************************************************/
    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("2: double1 = %g", object->property("double1")->toDouble());
      eprintf("selected objects");
      foreach(TObject* o, selections)
        eprintf("%s", o->oname());

      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;

#include "plt_Tool.h"
O_METHOD_BEGIN(TObject, Example12Plot, 0, ":/images/plot.png", 0/*FLG_METHOD_SKIP_GUI*/, "")
{
  if (VARLID_ARG)
  {
    QVector<Double2> line1 = QVector<Double2>()
      << Double2(0.0, 1.1)
      << Double2(1.0, 1.5)
      << Double2(2.0, 2.5)
      << Double2(3.0, 1.3)
      ;

    QVector<Double2> line2 = QVector<Double2>()
      << Double2(0.0, 1.4)
      << Double2(1.0, 1.7)
      << Double2(2.0, 1.1)
      << Double2(3.0, 2.3)
      ;

    QVector<Double2> line3 = QVector<Double2>()
      << Double2(0.0, 1.4)
      << Double2(2.0, 1.7)
      << Double2(4.0, 1.1)
      << Double2(5.0, 2.3)
      ;

    QVector<QVector<Double2> > lines;
    lines.push_back(line1);
    lines.push_back(line2);
    lines.push_back(line3);

    QStringList names = QStringList()
      << "Line 1"
      << "Line 2";

    plt_Tool dlg;
    for (int i = 0; i < lines.size(); i++)
    {
      QString name = names.size() > i ? names[i] : QString("Test Line %1").arg(i + 1);
      dlg.addCurveAuto(lines[i], name, 0, 0);
    }
    dlg.setTitle("A Test Plot");
    dlg.setAxisTitle("X Title", "Y Title");

    if (dlg.exec() == QDialog::Accepted)
    {
      return true;
    }

  }
}
O_METHOD_END;

O_METHOD_BEGIN(TObject, Example13MultiplePlot, 0, ":/images/plot.png", 0/*FLG_METHOD_SKIP_GUI*/, "")
{
  if (VARLID_ARG)
  {
    plt_Tool dlg;

    // left plot
    QVector<Double2> line1 = QVector<Double2>()
      << Double2(0.0, 1.1)
      << Double2(1.0, 1.5)
      << Double2(2.0, 2.5)
      << Double2(3.0, 1.3)
      ;
    QVector<Double2> line2 = QVector<Double2>()
      << Double2(0.0, 1.4)
      << Double2(1.0, 1.7)
      << Double2(2.0, 1.1)
      << Double2(3.0, 2.3)
      ;
    QVector<QVector<Double2> > lines = QVector<QVector<Double2> >()
      << line1
      << line2;
    QStringList names = QStringList()
      << "Line 1"
      << "Line 2";
    for (int i = 0; i < lines.size(); i++)
    {
      QString name = names.size() > i ? names[i] : QString("Test Line %1").arg(i + 1);
      dlg.addCurveAuto(lines[i], name, 0, 0);
    }
    dlg.setTitle("A Test Plot", 0, 0);
    dlg.setAxisTitle("X Title", "Y Title", 0, 0);

    // right plot
    line1 = QVector<Double2>()
      << Double2(0.0, 1.7)
      << Double2(1.0, 1.5)
      << Double2(2.0, 2.5)
      << Double2(3.0, 1.3)
      ;
    line2 = QVector<Double2>()
      << Double2(0.0, 1.4)
      << Double2(1.0, 2.9)
      << Double2(2.0, 1.1)
      << Double2(3.0, 2.3)
      ;
    lines = QVector<QVector<Double2> >()
      << line1
      << line2;
    names = QStringList()
      << "Line 3"
      << "Line 4";
    for (int i = 0; i < lines.size(); i++)
    {
      QString name = names.size() > i ? names[i] : QString("Test Line %1").arg(i + 1);
      dlg.addCurveAuto(lines[i], name, 0, 1);
    }
    dlg.setTitle("B Test Plot", 0, 1);
    dlg.setAxisTitle("X", "Y", 0, 1);




    dlg.setWindowTitle("Multiple Test Plots");
    if (dlg.exec() == QDialog::Accepted)
    {
      return true;
    }

  }
}
O_METHOD_END;

#include "w_QMessageBox.h"
O_METHOD_BEGIN(TObject, Example14MessageBoxChoice, 0, 0, 0/*FLG_METHOD_SKIP_GUI*/, "")
{
  if (VARLID_ARG)
  {
    QString title = QObject::tr("Information");
    QString text = QObject::tr("This will override the current case. What would you like to do?");
    QStringList buttons = QStringList()
      << QObject::tr("Stop because I want to save it first")
      << QObject::tr("Continue because I have saved it")
      << QObject::tr("Cancel");

    QString select = w_QMessageBox::warning(0, title, text, buttons, "");
    printMessage("Selected \n\n%s", select.ascii());
    if (select == buttons[0])
    {
      return 0;
    }
    else if (select == buttons[1])
    {
      return 1;
    }
    else if (select == buttons[2])
    {
      return 2;
    }
  }
}
O_METHOD_END;

#include "plt_XYMultiWidget.h"
static bool plotCallback(plt_XYInteractive* plot)
{
  Double2 closest;
  int index = -1;
  QString lineName;
  Double2 p = plot->getClickedPoint(&closest, &index, &lineName);
  eprintf("%s %d: %g %g", lineName.ascii(), index, closest[0], closest[1]);

  return true;
}

O_METHOD_BEGIN(TObject, Example15MultiplePlotWithProperties, 0, ":/images/plot.png", 0/*FLG_METHOD_SKIP_GUI*/, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample15");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 1, "Other inputs");

    // data
    QVector<double> v1 = QVector<double>() << 1.0 << 2.0 << 3.0 << 4.0;
    QVector<double> v2 = QVector<double>() << 2.0 << 3.1 << 2.8 << 2.9;
    QVector<QVector<double> > data = QVector<QVector<double> >() << v1 << v2;


    // right table
    holder2->addText("Table input");

    QStringList names = QStringList() << "X" << "Y";
    QVector<int> units = QVector<int>() << TUnit::mass_flow << TUnit::pressure;
    w_VectorPropertyWidget* w = new w_VectorPropertyWidgetV(holder1);
    w->setProperties(data, names, &units);
    holder2->placeWidget(w, 1, 0, 1, 2);

    // left plot
    holder1->addText("Multiple plots");
    plt_XYMultiWidget*plot = new plt_XYMultiWidget(holder1);
    // add one curve
    plot->addCurve(v1, v2, "Curve 1", "red");
    // add 5 more curves
    for (int i = 0; i < 5; i++)
    {
      for (int j = 0; j < v2.size(); j++)
        v2[j] *= sqrt(j + 1);

      plot->addCurve(v1, v2, QString("Curve %1").arg(i + 2), "blue");
    }

    // add curve to another plot window
    plot->addCurveAuto(v1, v2, "A test plot 2", 0, 1);
    holder1->placeWidget(plot, 1, 0, 1, 2);
    if (plt_XYInteractive* p = plot->getPlot(0, 0))
    {
      p->setCallback("leftClick", plotCallback);
    }


    dlg.setSizeHint(900, 400);

    if (dlg.exec() == QDialog::Accepted)
    {

    }

  }
}
O_METHOD_END;



static void applyButtonCallback(w_PropertyHolderWidget* holder1)
{
  // find the mapped widgets
  w_VectorPropertyWidget* w = (w_VectorPropertyWidget*)holder1->findMappedWidget("data");
  plt_XYMultiWidget* plot = (plt_XYMultiWidget*)holder1->findMappedWidget("plot");

  if (w && plot)
  {
    // find the mapped data
    QVector<QVector<double> >& data = *((QVector<QVector<double> >*)holder1->findMappedData("data"));

    // make the data accept user input
    w->accept();

    // clear the old plot
    plot->clear();

    // add new plot
    plot->addCurveAuto(data[0], data[1], "A test plot 1", 0, 0);
    plot->addCurveAuto(data[0], data[1], "A test plot 2", 0, 1);
  }
}

O_METHOD_BEGIN(TObject, Example16ButtonCallback, 0, ":/images/plot.png", 0/*FLG_METHOD_SKIP_GUI*/, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample16");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 1, 1, 2, "Other inputs");

    // left table
    holder1->addText("Table input");
    QVector<double> v1 = QVector<double>() << 1.0 << 2.0 << 3.0 << 4.0;
    QVector<double> v2 = QVector<double>() << 2.0 << 3.1 << 2.8 << 2.9;
    QVector<QVector<double> > data = QVector<QVector<double> >() << v1 << v2;
    QStringList names = QStringList() << "X" << "Y";
    QVector<int> units = QVector<int>() << TUnit::mass_flow << TUnit::pressure;
    w_VectorPropertyWidget* w = new w_VectorPropertyWidgetV(holder1);

    // add some mapped widget and data
    holder1->addMappedWidget("data", w);
    holder1->addMappedData("data", &data);

    w->setProperties(data, names, &units);
    holder1->placeWidget(w, 1, 0, 1, 2);

    w_QPushButton* button = holder1->addButton("Apply");
    holder1->setButtonCallback(button, applyButtonCallback);

    // right plot
    holder2->addText("Multiple plots");
    plt_XYMultiWidget*plot = new plt_XYMultiWidget(holder1);
    // add some mapped widget
    holder1->addMappedWidget("plot", plot);

    plot->addCurveAuto(v1, v2, "A test plot 1", 0, 0);
    plot->addCurveAuto(v1, v2, "A test plot 2", 0, 1);
    holder2->placeWidget(plot, 1, 0, 1, 2);
    if (plt_XYInteractive* p = plot->getPlot(0, 0))
    {
      p->setCallback("leftClick", plotCallback);
    }

    if (dlg.exec() == QDialog::Accepted)
    {

    }

  }
}
O_METHOD_END;
