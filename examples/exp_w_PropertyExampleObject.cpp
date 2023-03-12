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

#include "exp_w_PropertyExampleObject.h"
#include "w_PropertyHolderDialog.h"
#include "w_TModule.h"

QStringList exp_w_PropertyExampleObject::nozzleTypeList = (QStringList() <<
  "1DConvergeDiverge" <<
  "LavalNozzle" <<
  "type3" <<
  "type4" <<
  "channel" <<
  "1DConvergeDivergeNew" <<
  "1DConvergeDivergeStraightEnd" <<
  "fluent");
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

REGISTER_OBJECT_CLASS(exp_w_PropertyExampleObject, "w_PropertyExampleObject", TObject);

exp_w_PropertyExampleObject::exp_w_PropertyExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;
  DEFINE_QSTRING_INIT(nozzleType, "DnozzleType", 0, NULL, NULL); nozzleType = nozzleTypeList[0];
  setPropertyComboString("nozzleType", &nozzleTypeList);

  DEFINE_SCALAR_INIT(double, xSize, "xSize", 0, NULL, NULL); xSize = 1.0;

  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector1, 0, 0, 0, NULL, TUnit::pressure);
  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector2, 0, 0, 0, NULL, TUnit::density);
  DEFINE_QDOUBLE_VECTOR_INIT(doubleVector3, 0, 0, 0, NULL, NULL);
  DEFINE_QINT_VECTOR_INIT(intVector1, 0, 0, NULL, NULL);
  DEFINE_QINT_VECTOR_INIT(intVector2, 0, 0, NULL, NULL);

  DEFINE_SCALAR_INIT(double, double1, 0, 0, NULL, TUnit::pressure);
  setPropertyComboString("double1", &_doubleSelectors);
  DEFINE_SCALAR_INIT(double, double2, 0, 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, double3, 0, 0, NULL, TUnit::pressure);

  DEFINE_SCALAR_INIT(double, double4, 0, 0, NULL, TUnit::density);
  DEFINE_SCALAR_INIT(double, double5, 0, 0, NULL, TUnit::density);
  DEFINE_SCALAR_INIT(double, double6, 0, 0, NULL, TUnit::density);

  DEFINE_SCALAR_INIT(int, int1, 0, 0, NULL, NULL);
  setPropertyComboString("int1", &_intSelectors);
  DEFINE_SCALAR_INIT(int, int2, 0, 0, NULL, NULL);

  DEFINE_SCALAR_INIT(bool, bool1, 0, 0, NULL, NULL);
  setPropertyComboString("bool1", &_boolSelectors);

  DEFINE_QSTRINGLIST_INIT(stringList1, "stringList1", 0, NULL, NULL);
  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(doubleVectorVector, 0, 0, 0, NULL, TUnit::pressure);
  {
    //doubleVectorVector = { {1, 2}, {1, 3}, {3, 4.5}, {2.3, 3.4} };
    property("doubleVectorVector")->setTag("vectorNames", "length,weight,area,heat");
    QStringList LL = {
      QString("%1").arg(TUnit::length),
      QString("%1").arg(TUnit::mass),
      QString("%1").arg(TUnit::area),
      QString("%1").arg(TUnit::energy)
    };
    property("doubleVectorVector")->setTag("vectorUnitCats", LL.join(","));
  }


  // named data vector define
  DEFINE_QSTRINGLIST_INIT(names, 0, 0, NULL, NULL); names = QStringList() << "RPM" << "mass flow" << "Pr" << "Eff";
  DEFINE_QSTRINGLIST_INIT(units, 0, 0, NULL, NULL); units = QStringList() << "rpm" << "Kg/s" << "" << "";
  DEFINE_QSTRINGLIST_INIT(namedDataVector, 0, 0, NULL, NULL);
  namedDataVector = QStringList() 
    << "names" << "names" // key value
    << "units" << "units" // key value
    << "data" << "dataVectorVector"; // key value
  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(dataVectorVector, 0, 0, 0, NULL, 0);
  QVector<double> a = QVector<double>() << 1234 << 1.3 << 2.1 << 0.9;
  dataVectorVector.push_back(a);
  a = QVector<double>() << 1600 << 1.4 << 2.3 << 0.88;
  dataVectorVector.push_back(a);
  a = QVector<double>() << 1234 << 1.3 << 2.1 << 0.9;
  dataVectorVector.push_back(a);



  DEFINE_QSTRING_INIT(_record, "Record", 0, NULL, NULL);
  setPropertyCallback("_record", "db2_Search::databaseBrowse");

  setTag("MethodOrder", "WidgetExample01,"
    "WidgetExample02,"
    "WidgetExample03,"
    "WidgetExample03RightStretch,"
    "WidgetExample04,"
    "WidgetExample05,"
    "WidgetExample06,"
    "WidgetExample07,"
    "WidgetExample08,"
    "WidgetExample09,"
    "WidgetExample10,"
    "WidgetExample11,"
    "WidgetExample13,"
    "WidgetExample14,"
    "ProgressBarExample,"
    "$,"
    "WidgetExampleTab0,"
    "WidgetExampleTab1,"
    "WidgetExampleTab2,"
    "$,"
    "vectorDialogTest,"
    "vector2dDialogTest,"
    "$,"
    "NamedVectorExample,"
    "NamedVectorPlotExample,"
    "vectorDialogTestWithButton,"
    "TPropertyInputExample,"
    "SelectorExample,"
    "$,"
    "ExampleObjectSelection,"
    "ExampleHierarchySelection,"
    "$,"
    "testTableInput,"
    "$,"
    "HideIfExample,"
    "ModifierExample,"
    "$,"
    "ObjectEditExample,"
    "$,"
    "GenericNonModalDialogExample,"
    "GenericTaskWindowExample,"
    "GenericDocKWidgetExample,"
    "$");
}

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample01, "01: Basic Widgets", 0, 0, "WidgetExample01")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample01");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QLabel* label = holder->addImageLabel(":/images/splash-top.png");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) { w->setToolTip("String tooltip"); }

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) { w->setToolTip("Double tooltip"); }

    int i = 5;
    holder->addProperty(&i, "An int");

    bool check = false;
    holder->addProperty(&check, "A check");

    QString fileName;
    if (w_Property* w = holder->addProperty(&fileName, "A File", 0, false, false, false, "TCallbackOpenFileBrowser")) { w->setToolTip("File tooltip"); }

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { w->setToolTip("Selection tooltip"); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample02, "02: Group Box", 0, 0, "WidgetExample02")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample02 123456 123456 123456 123456 123456 ");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QString aString = "string";
    if (w_Property* w = holder->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double", TUnit::length)) {
      w->setRange(5, 16);
    }

    int AInt = 18;
    if (w_Property* w = holder->addProperty(&AInt, "A Int")) {
      //w->setRange(4, 9);
    }

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { QGroupBox* g = w->wrapGroupBox(); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample03, "03: Hierarchy", 0, 0, "WidgetExample03")
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
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample04, "04: Section Line", 0, 0, "WidgetExample04")
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
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample05, "05: Object Property, different types of selections", 0, 0, "WidgetExample05")
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

    // with display name different than list names, directly set on property
    //QStringList displayList = QStringList() << "display 1" << "display 2" << "display 3";
    if (auto p = object->property("nozzleType"))
    {
      //p->setComboDisplayList(displayList);
      if (w_Property* w = holder->addProperty(p, w_Property::VLayout)) {}
    }

    // with display name different than list names, only use in w_Property
    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QStringList LL = QStringList() << "s1" << "s2" << "s3";
    QString selection = L[1];

    if (w_Property* w = holder->addProperty(&selection, "Button with short display name", &L, true, false, false, "", false, w_Property::HLayout))
    {
      w->setComboDisplayList(LL);
    }

    // with display name the same as list names
    QString selection1 = L[1];
    if (w_Property* w = holder->addProperty(&selection1, "H Layout", &L, false, false, false, "", false, w_Property::HLayout))
    {
    }

    QString selection2 = L[1];
    if (w_Property* w = holder->addProperty(&selection2, "V Layout", &L, false, false, false, "", false, w_Property::VLayout))
    {
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
      eprintf("selection1 = %s", qPrintable(selection1));
      eprintf("nozzleType = %s", qPrintable(object->property("nozzleType")->toString()));
    }

    return true;
  }
}
O_METHOD_END;

#include "w_ObjectTreeWidget.h"
static void handleObjectSelectionChange(w_ObjectTreeWidget* w)
{
  QVector<TObject*> L;
  w->getTree()->getSelectedObjects(L, 0);

  eprintf("\nSelected: %d objects", L.size());
  foreach(TObject*o, L)
  {
    eprintf("%s", o->oname().ascii());
  }
}

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample06, "06: Object Tree, connect with lambda", 0, 0, "WidgetExample06")
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

    QObject::connect(objectTree, &w_TTWidget::valueChanged, objectTree,
      [=]() { QVector<TObject*> L;
              objectTree->getTree()->getSelectedObjects(L, 0);

              eprintf("\nSelected: %d objects", L.size());
              foreach(TObject*o, L)
              {
                eprintf("%s", o->oname().ascii());
              } 
    }
    );

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
        eprintf("%s", qPrintable(o->oname()));
    }

    return true;
  }
}
O_METHOD_END;

#include "pe_TPropertyInputWidget.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample07, "07: Input Table Widget and Range Setting", 0, 0, "WidgetExample07")
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
    if (TPropertyInputWidget* w = new TPropertyInputWidget(holder1))
    {
      //object->property("double1")->setRange(5.5, 7.8);
      object->property("double3")->setRange(5.5, 7.8);
      QVector<property_t *> properties = QVector<property_t *>()
        << object->property("double1")
        << object->property("double2")
        << object->property("double3")
        << object->property("bool1");
      w->setProperties(properties, true);
      holder1->placeWidget(w, 1, 0, 1, 2);
    }

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
      printf("double1 = %g", object->property("double1")->toDouble()); fflush(stdout);
      eprintf("double1 = %g", object->property("double1")->toDouble());
      eprintf("bool1 = %d", (int)object->property("bool1")->toBool());
    }

    return true;
  }
}
O_METHOD_END;

#include "w_VectorPropertyWidgetH.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample08, "08: Table Input and Place Holders, and range control", 0, 0, "WidgetExample08")
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
    QVector<QVector<property_t *> > properties(4);
    //object->property("double1")->setValue(2); 
    object->property("double1")->setRange(5, 8);
    properties[0].push_back(object->property("double1"));

    //object->property("double2")->setValue(5); 
    properties[0].push_back(object->property("double2"));
    object->property("double2")->setRange(9, 18);
    //object->property("int1")->setValue(4); 
    properties[1].push_back(object->property("int1"));
    object->property("int1")->setRange(8, 18);
    //object->property("int2")->setValue(6); 
    properties[1].push_back(object->property("int2"));
    object->property("int2")->setRange(8, 18);
    properties[2].push_back(0);// push back a place holder
    properties[2].push_back(0);// push back a place holder
    properties[2].push_back(object->property("bool1"));
    properties[3].push_back(object->property("nozzleType"));
    properties[3].push_back(0);// push back a place holder
    properties[3].push_back(object->property("_record"));

    QStringList verticalNames = QStringList() << "row 1" << "row 2";
    if (w_VectorPropertyWidget* w = new w_VectorPropertyWidgetH(holder1))
    {
      w->setProperties(properties, &verticalNames);
      w->setCellReadOnly(1, 0, true);
      holder1->placeWidget(w, 1, 0, 1, 2);
    }

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("double1 = %g", object->property("double1")->toDouble());
      eprintf("_record = %s", object->property("_record")->toString().ascii());
      eprintf("nozzleType = %s", object->property("nozzleType")->toString().ascii());
      eprintf("bool1 = %d", object->property("bool1")->toBool());
      eprintf("aString = %s", aString.ascii());

    }

    return true;
  }
}
O_METHOD_END;

#include "w_VectorPropertyWidgetV.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample09, "09: Table Vertical Widget", 0, 0, "WidgetExample09")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample09");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // property table
    holder1->addText("Table input");
    if (w_VectorPropertyWidget* w = new w_VectorPropertyWidgetV(holder1))
    {
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
      properties.push_back(object->property("intVector2"));
      QStringList verticalNames = QStringList() << "row 1" << "row 2" << "row 3";
      w->setProperties(properties, &verticalNames);
      holder1->placeWidget(w, 1, 0, 1, 2);
    }

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

// TODO: Change to use lambda call back
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample10, "10: Hide Show", 0, 0, "WidgetExample10")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample10");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    //holder->setCallback(widgetChangedCallback);
    QLabel* label = holder->addImageLabel(":/images/splash-top.png");
    holder->addMappedWidget("splashLabel", label);

    double ADouble = 4;
    if (w_Property* w = holder->addProperty(&ADouble, "A Double")) {}

    bool check = false;
    holder->addProperty(&check, "A check");

    QStringList L = QStringList() << "Show splash" << "Hide splash";
    QString selection = L[0];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) 
    { 
      holder->addMappedWidget("chooseShowSplash", w);

      QObject::connect(w, &w_TTWidget::valueChanged, label,
        [w, label, L]() {
        label->setHidden(w->getProperty()->getValue().toString() == L[1]);
      }
      );
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;

#include "w_PropertyString.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample11, "11: Object Tree Display with Hierarchy", 0, 0, "WidgetExample11")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample11");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Object selection tree");
    QVector<TObject*> selections;
    if (w_ObjectTreeWidget* objectTree = new w_ObjectTreeWidget(&dlg))
    {
      selections.push_back(object);
      objectTree->getTree()->displayObjectTree(object->parent(), &selections);
      holder1->placeWidget(objectTree, 1, 0, 1, 2);
    }

    // other inputs
    QString aString = "string";
    if (w_Property* w = holder2->addProperty(&aString, "A String")) {}

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList()
      << "selection 1"
      << "selection 2"
      << "some menu/selection 3"
      << "some menu/selection 4"
      ;
    QString selection = "selection 3";
    if (w_Property* w = holder2->addProperty(&selection, "Please choose", &L, false, false, true))
    {
      if (w_PropertyString* s = dynamic_cast<w_PropertyString*>(w))
      {
        s->setUseHierarchyList();
      }
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
      eprintf("selected objects");
      foreach(TObject* o, selections)
        eprintf("%s", qPrintable(o->oname()));
    }

    return true;
  }
}
O_METHOD_END;

static void handleSliderFinished(w_Property* w)
{
  eprintf("Sliding finished at value %s", w->getProperty()->toString().ascii());
}

#include "w_PropertyDouble.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample13, "13: Slider and Dial, lambda callback", 0, 0, "WidgetExample13")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample13");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2);
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Other inputs");

    // object tree
    holder1->addText("Object selection tree");
    QVector<TObject*> selections;
    selections.push_back(object);
    w_ObjectTreeWidget* objectTree = new w_ObjectTreeWidget(&dlg);
    objectTree->getTree()->displayObjectTree(object->parent(), &selections, 0, "TCurve");
    holder1->placeWidget(objectTree, 1, 0, 1, 2);

    // other inputs
    QString aString = "string";
    w_Property* aStringW = nullptr;
    if (w_Property* w = holder2->addProperty(&aString, "A String")) 
    {
      aStringW = w;
    }

    double ADouble = 4;
    if (w_PropertyDouble* w = dynamic_cast<w_PropertyDouble*>(holder2->addProperty(&ADouble, "A Double", TUnit::angle)))
    {
      QAbstractSlider* slider = w->addSlider(1, 7, 1);

      QObject::connect(w, &w_Property::editingFinished, w,
        [=]() { 
        handleSliderFinished(w);

        aStringW->hideAll(w->getProperty()->toDouble() > 2);
      });
    }

    double doubles[4] = { 1.234, 2.324, 1.39739423, 3.1 };
    for (auto i = 0; i < 4; i++)
    {
      QString name = QString("Number %1").arg(i + 1);
      if (w_PropertyDouble* w = dynamic_cast<w_PropertyDouble*>(holder2->addProperty(&doubles[i], name, TUnit::length)))
      {
        QAbstractSlider* slider = w->addSlider(1, 7, 1);
      }
    }

    double BDouble = 5;
    if (w_PropertyDouble* w = dynamic_cast<w_PropertyDouble*>(holder2->addProperty(&BDouble, "B Double")))
    {
      QAbstractSlider* slider = w->addSlider(1, 7, 1, "dial");
    }

    double CDouble = 5;
    if (w_PropertyDouble* w = dynamic_cast<w_PropertyDouble*>(holder2->addProperty(&CDouble, "C Double")))
    {
    }

    QStringList L = QStringList()
      << "selection 1"
      << "selection 2"
      << "some menu/selection 3"
      << "some menu/selection 4"
      ;
    QString selection = "selection 3";
    if (w_Property* w = holder2->addProperty(&selection, "Please choose", &L, false, false, true))
    {
      if (w_PropertyString* s = dynamic_cast<w_PropertyString*>(w))
      {
        s->setUseHierarchyList();
      }
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
      eprintf("selected objects");
      foreach(TObject* o, selections)
        eprintf("%s", qPrintable(o->oname()));
    }

    return true;
  }
}
O_METHOD_END;

#include "w_PropertyRadioSelector.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample14, "14: Radio group, related widgets, holder check box", 0, 0, "WidgetExample14")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample14");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Main options");
    w_PropertyHolderWidget* holder2 = holder->getHolder(1, 0, 1, 2, "Other inputs", true);

    QStringList L = QStringList()
      << "selection 1"
      << "selection 2"
      << "some menu/selection 3"
      << "some menu/selection 4"
      ;
    QString selection = "some menu/selection 3";
    QMap<QString, QVector<property_t*>> m = {
      {"selection 1", {object->property("double1")}},
      {"selection 2", {object->property("double2")}},
      {"some menu/selection 3", {object->property("double3"), object->property("double5")}},
      {"some menu/selection 4", {object->property("double4")}},
    };

    w_PropertyRadioSelector* selector = nullptr;

    if (w_Property* w = holder1->addProperty(&selection, "Please choose", &L, false, false, false, "", false, w_Property::GLayout))
    {
      if (w_PropertyRadioSelector* ss = dynamic_cast<w_PropertyRadioSelector*>(w))
      {
        ss->setMappedProperties(m);
        selector = ss;
      }
    }

    QString selection1 = "some menu/selection 4";
    if (w_Property* w = holder2->addProperty(&selection1, "Please choose", &L, false, false, true))
    {

    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
      eprintf("selected objects");
      auto LL = selector->getChosenMappedProperties();
    }

    return true;
  }
}
O_METHOD_END;

#include "w_ProgressBarDialog.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, ProgressBarExample, "15: Progress bar", 0, 0, "Progress bar")
{
  if (VARLID_ARG)
  {
    w_ProgressBarDialog dlg;
    {
      dlg.show();

      auto max = 100;
      dlg.setRange(0, max);

      for (auto i = 0; i <= max; i++)
      {
        dlg.setValue(i);
        Sleep(100);
      }
    }

    return true;
  }
}
O_METHOD_END;

#include "w_BasePropertyHolderDialog.h"
#include "w_VectorPropertyWidgetV.h"
#include "w_VectorPropertyWidgetH.h"

O_METHOD_BEGIN(exp_w_PropertyExampleObject, vectorDialogTest, "V Vector Table", 0, 0, "vectorDialogTest")
{
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
O_METHOD_BEGIN(exp_w_PropertyExampleObject, vector2dDialogTest, "Vector 2 Widget", 0, 0, "vectorDialogTest")
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
    //{
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
    //}

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

      object->property("double2")->setRange(20, 30);

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
    QVector<QVector<property_t *> > properties(4);
    object->property("double1")->setValue(2); properties[0].push_back(object->property("double1"));
    object->property("double2")->setValue(5); properties[0].push_back(object->property("double2"));
    object->property("double3")->setValue(7); properties[0].push_back(object->property("double3"));

    object->property("int1")->setValue(4); properties[1].push_back(object->property("int1"));
    object->property("int2")->setValue(6); properties[1].push_back(object->property("int2"));

    object->property("double4")->setValue(2); properties[2].push_back(object->property("double4"));
    object->property("double5")->setValue(9); properties[2].push_back(object->property("double5"));
    object->property("double6")->setValue(7); properties[2].push_back(object->property("double6"));
    properties[3].push_back(0);// push back a place holder
    properties[3].push_back(0);// push back a place holder
    properties[3].push_back(object->property("bool1"));

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
        eprintf("%s", qPrintable(o->oname()));

      eprintf("bool1 = %d", (int)object->property("bool1")->toBool());
      eprintf("ADouble = %g", ADouble);
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, NamedVectorExample, "Named Vector Table", 0, 0, "NamedVectorExample")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("NamedVectorExample");

    dlg.setHideLabel(true);
    if (property_t *p = object->property("namedDataVector"))
      if (w_Property* w = dlg.addPropertyNamedVector(p))
      {
        dlg.setStretch(w);
      }

    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, NamedVectorPlotExample, "Named Vector Plot", 0, 0, "NamedVectorPlotExample")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("NamedVectorPlotExample");

    dlg.setHideLabel(true);
    if (property_t *p = object->property("namedDataVector"))
      if (w_Property* w = dlg.addPropertyNamedVectorPlot(p))
      {
        dlg.setStretch(w);
      }

    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;



static void buttonCallback(w_PropertyHolderWidget* holder)
{
  if (w_VectorPropertyWidget* w = dynamic_cast<w_VectorPropertyWidget*>(holder->findMappedWidget("table")))
  {
    QVector<double> a;
    w->getColumn(1, a);
    for (int i = 0; i < a.size(); i++)
      a[i] *= 2;
    w->setColumn(1, a);
  }
}


O_METHOD_BEGIN(exp_w_PropertyExampleObject, vectorDialogTestWithButton, "Vector Property & Button", 0, 0, "vectorDialogTestWithButton")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);

    w_QPushButton* button = holder->addButton("Double value of column 1");
    holder->setButtonCallback(button, buttonCallback);


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
    holder->placeWidget(w, 2, 0, 1, 2);
    holder->addMappedWidget("table", w);

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("2: doubleVector1[0] = %g", object->property("doubleVector1")->asDoubleVector()[0]);
      eprintf("2: intVector2[0] = %d", object->property("intVector2")->asIntVector()[0]);
      eprintf("2: doubleVector3[0] = %g", object->property("doubleVector3")->asDoubleVector()[0]);
    }

    return true;
  }
}
O_METHOD_END;






#include "pe_TPropertyInput.h"
#include "core_ActionHelper.h"
static bool handleCallback(TPropertyInput* t)
{
  TObject* o = (TObject*)VARIANT2POINTER(t->getTag("object"));

  eprintf("value changed");

  return true;
}

O_METHOD_BEGIN(exp_w_PropertyExampleObject, TPropertyInputExample, "Property Input Table", 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("TPropertyInputExample");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
    QVector<property_t *> properties;
    properties.push_back(object->property("bool1"));
    properties.push_back(object->property("nozzleType"));
    properties.push_back(object->property("double1"));
    properties.push_back(object->property("int2"));
    if (TPropertyInput* w = new TPropertyInput(holder))
    {
      w->setTag("object", POINTER2VARIANT(object));
      w->setProperties(properties);
      holder->placeWidget(w, 1, 0, 1, 2);
      QObject::connect(w, &TPropertyInput::valueChanged, w,
        [=]() { handleCallback(w); });
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("int2 = %d", object->property("int2")->toInt());
      eprintf("double1 = %g", object->property("double1")->toDouble());
      eprintf("nozzleType = %s", object->property("nozzleType")->toString().ascii());
      eprintf("bool1 = %d", object->property("bool1")->toBool());
    }

    return true;
  }
}
O_METHOD_END;


O_METHOD_BEGIN(exp_w_PropertyExampleObject, SelectorExample, "Selector", 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("SelectorExample");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);

    object->property("bool1")->setTag("selectorType", "radios");
    if (w_Property* w = dlg.addProperty(object->property("bool1")))
    {
    }

    if (w_Property* w = dlg.addProperty(object->property("int1")))
    {
    }

    if (w_Property* w = dlg.addProperty(object->property("double1")))
    {
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("int1 = %d", object->property("int1")->toInt());
      eprintf("double1 = %g", object->property("double1")->toDouble());
      eprintf("bool1 = %d", object->property("bool1")->toBool());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, ExampleObjectSelection, "Object Selection", 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("ExampleObjectSelection");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);

    // object tree
    holder->addText("Object selection tree");
    QVector<TObject*> selections;
    selections.push_back(object);
    w_ObjectTreeWidget* objectTree = new w_ObjectTreeWidget(&dlg);
    objectTree->getTree()->displayObjectTree(object->parent(), &selections, 0, "TCurve");
    holder->placeWidget(objectTree, 1, 0, 1, 2);

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selected objects");
      foreach(TObject* o, selections)
        eprintf("%s", qPrintable(o->oname()));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, ExampleHierarchySelection, "Object Hierarchy Selection", 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("ExampleHierarchySelection");
    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);

    QStringList L = QStringList()
      << "selection 1"
      << "selection 2"
      << "some menu/selection 3"
      << "some menu/selection 4"
      ;
    QString selection = "selection 3";
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L, false, false, true))
    {
      if (w_PropertyString* s = dynamic_cast<w_PropertyString*>(w))
      {
        s->setUseHierarchyList();
      }
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", selection.ascii());
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExample03RightStretch, "03: Right stretch", 0, 0, "WidgetExample03RightStretch")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample03RightStretch");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Holder 1");
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 8, "Holder 2"); // try to stretch the right hold

    QString aString = "string";
    if (w_Property* w = holder1->addProperty(&aString, "A String")) {
      w->setToolTip("Test of tool tip");
    }

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {
      w->setToolTip("Test of tool tip for double");
    }

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) {
      QGroupBox* g = w->wrapGroupBox();
      w->setToolTip("Test of tool tip for selections");
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExampleTab0, "Tab Widget 0", 0, 0, "WidgetExampleTab0")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExampleTab0");

    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs"))
    {
      QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
      QStringList LL = QStringList() << "selection 1" << "selection 2" << "selection 3";

      QString selection = L[1];
      if (w_Property* w = holder->addProperty(&selection, "Please choose", &L))
      {
        QGroupBox* g = w->wrapGroupBox();
      }

      // get one tab at 1, 0
      if (w_PropertyHolderWidget* holder1 = holder->getHolder(1, 0, 1, 2, "", false, "holder 1")) // tab
      {
        QString aString = "string";
        if (w_Property* w = holder1->addProperty(&aString, "A String"))
        {
        }
      }
      // use the same column and row to get another tab
      if (w_PropertyHolderWidget* holder3 = holder->getHolder(1, 0, 1, 2, "", false, "Tab 2")) // tab
      {
        QString aString3 = LL[0];
        if (w_Property* w = holder3->addProperty(&aString3, "A choice", &LL))
        {
        }
      }

      if (dlg.exec() == QDialog::Accepted)
      {
        eprintf("selection = %s", qPrintable(selection));
      }
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExampleTab1, "Tab Widget 1", 0, 0, "WidgetExampleTab1")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExampleTab1");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "", false, "Holder 1"); // tab
    w_PropertyHolderWidget* holder3 = holder->getHolder(0, 0, 1, 2, "", false, "Holder 3"); // tab
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Holder 2");

    QString aString = "string";
    if (w_Property* w = holder1->addProperty(&aString, "A String")) {}

    QStringList LL = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString aString3 = "string";
    if (w_Property* w = holder3->addProperty(&aString3, "A choice", &LL)) {}

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double")) {}

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L)) { QGroupBox* g = w->wrapGroupBox(); }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

#include "w_QTabWidget.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, WidgetExampleTab2, "Tab 2", 0, 0, "WidgetExampleTab2")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExampleTab2");

    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      QStringList LL = QStringList() << "selection 1" << "selection 2" << "selection 3";
      QString selection = LL[0];
      // get one tab at 1, 0
      if (w_PropertyHolderWidget* holder1 = holder->getHolder(1, 0, 1, 2, "", false, "Holder 1")) // tab
      {
        QString aString = "string";
        if (w_Property* w = holder1->addProperty(&aString, "A String"))
        {
        }

        // set the tab at left
        if (w_QTabWidget* tab = holder1->getTabWidget())
        {
          tab->setTabPosition(QTabWidget::West);
        }
      }

      // use the same column and row to get another tab
      if (w_PropertyHolderWidget* holder3 = holder->getHolder(1, 0, 1, 2, "", false, "Holder 3")) // tab
      {
        if (w_Property* w = holder3->addProperty(&selection, "A choice", &LL))
        {
        }
      }

      if (dlg.exec() == QDialog::Accepted)
      {
        eprintf("selection = %s", qPrintable(selection));
      }
    }

    return true;
  }
}
O_METHOD_END;


#include "w_TableInput.h"
#include "w_PropertyDoubleTable.h"
O_METHOD_BEGIN(exp_w_PropertyExampleObject, testTableInput, "Table Input Test", 0, 0, "Table Input Test")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("Table Input Test");

    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
#if 1
      if (w_PropertyHolderWidget* holder1 = holder->getHolder(1, 0, 1, 2, "Holder 1")) // tab
      {
        if (w_Property* w = w_Property::getPropertyWidgetDoubleVectorTable(object->property("doubleVectorVector"), holder1))
        {
          holder1->placeWidget(w);
        }
      }
#else
      QStringList names = { "Column 1", "Column 2", "Column 3", "Column 4" };
      QStringList units = { "m", "m/s", "kg", "cm" };
      QVector< QVector<double> > data = {
        {1.2, 2.3},
        {2.2, 5.3},
        {3.2, 6.3},
        {5.2, 1.3},
      };

      if (w_PropertyHolderWidget* holder1 = holder->getHolder(1, 0, 1, 2, "Holder 1")) // tab
      {
        if (w_TableInput* w = new w_TableInput(holder1))
        {
          w->setData(names, units, data);

          holder1->placeWidget(w);
        }
      }
#endif
      if (dlg.exec() == QDialog::Accepted)
      {
        eprintf("Data has %d rows", object->property("doubleVectorVector")->asDoubleVectorVector()[0].size());
      }


      return true;
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, HideIfExample, "Hide If Example", 0, 0, 0)
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("HideIfExample");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Holder 1");
    w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 2, "Holder 2");

    QString aString = "string";
    w_Property* stringProperty = 0;
    if (w_Property* w = holder1->addProperty(&aString, "A String"))
    {
      stringProperty = w;
    }

    QString bString = "string";
    if (w_Property* w = holder1->addProperty(&bString, "B String"))
    {
    }

    QString cString = "string";
    if (w_Property* w = holder1->addProperty(&cString, "C String"))
    {
    }

    double ADouble = 4;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double"))
    {
      stringProperty->hideIf(w, w_Property::larger, 8);
    }

    bool ABool = false;
    if (w_Property* w = holder2->addProperty(&ABool, "A Bool"))
    {
      holder1->hideIf(w, w_Property::isTrue);
    }

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L))
    {
      QGroupBox* g = w->wrapGroupBox();
      holder2->hideIf(w, w_Property::equal, L[2]);
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, ModifierExample, "Lambda Call Back Modifier", 0, 0, 0)
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("ModifierExample");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");
    w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "Holder 1");
    w_PropertyHolderWidget* holder2 = holder->getHolder(1, 0, 1, 2, "Holder 2");

    QString aString = "string";
    w_Property* stringProperty = 0;
    if (w_Property* w = holder1->addProperty(&aString, "A String"))
    {
      stringProperty = w;
    }

    QString bString = "string";
    if (w_Property* w = holder1->addProperty(&bString, "B String"))
    {
    }

    QString cString = "string";
    if (w_Property* w = holder1->addProperty(&cString, "C String, hide if \"A String\" has hide inside"))
    {
      w->hideIf(stringProperty, w_Property::contains, "hide");
    }

    double ADouble = 4;
    w_Property* aDouble = 0;
    if (w_Property* w = holder2->addProperty(&ADouble, "A Double, hide \"A String\" when value is out of range [3, 7]"))
    {
      stringProperty->hideIf(w, w_Property::outRange, QPointF(3, 7));
      aDouble = w;
    }

    double BDouble = 4;
    if (w_Property* w = holder2->addProperty(&BDouble, "B Double, double the value of \"A Double\""))
    {
      w->setReadOnly();
      w->modifyIf(aDouble, [](w_Property* wp, w_TTWidget* tp) {
        double a = wp->getProperty()->toDouble();
        if (w_Property* ww = dynamic_cast<w_Property*>(tp))
        {
          ww->setValue(a * 2);
          return true;
        }
        return false;
      }
      );
    }

    bool ABool = false;
    if (w_Property* w = holder2->addProperty(&ABool, "A Bool, check to hide \"Holder 1\""))
    {
      holder1->hideIf(w, w_Property::isTrue);
    }

    QStringList L = QStringList() << "selection 1" << "selection 2" << "selection 3 (choose to hide \"Holder 2\")";
    QString selection = L[1];
    if (w_Property* w = holder->addProperty(&selection, "Please choose", &L))
    {
      QGroupBox* g = w->wrapGroupBox();
      holder2->hideIf(w, w_Property::equal, L[2]);
    }

    if (dlg.exec() == QDialog::Accepted)
    {
      eprintf("selection = %s", qPrintable(selection));
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, GenericNonModalDialogExample, "Generic Non-Modal Dialog Example", 0, 0, 0)
{
  if (VARLID_ARG)
  {
    if (auto m = CURRENT_MODULE)
    {
      auto creator = [&](QMap<QString, QVariant>& args, TModule* md)->QWidget *
      {
        if (w_PropertyHolderWidget* holder = new w_PropertyHolderWidget)
        {
          if (auto p = object->property("double1"))
          {
            if (w_Property* w = holder->addProperty(p))
            {
            }
          }

          return holder;
        }

        return nullptr;
      };

      QMap<QString, QVariant> args =
      {
        {"windowName", "Test"},
        {"purpose", "GenericNonModalDialogExample"}
      };


      if (w_QDialog * dlg = m->findOrCreateNonModalDialog(args, creator))
      {
        dlg->show();

        return true;
      }
    }

    return false;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_w_PropertyExampleObject, GenericDocKWidgetExample, "Generic Dock Widget Example", 0, 0, 0)
{
  if (VARLID_ARG)
  {
    if (auto m = CURRENT_MODULE)
    {
      auto creator = [&](QMap<QString, QVariant>& args, TModule* md)->QWidget *
      {
        if (w_PropertyHolderWidget* holder = new w_PropertyHolderWidget)
        {
          if (auto p = object->property("double2"))
          {
            if (w_Property* w = holder->addProperty(p))
            {
            }
          }

          return holder;
        }

        return nullptr;
      };

      QMap<QString, QVariant> args =
      {
        {"windowName", "Dynamic dock"},
        {"purpose", "GenericDockExample"}
      };


      if (auto * dlg = m->findOrCreateDockWidget(args, creator))
      {

        return true;
      }
    }

    return false;
  }
}
O_METHOD_END;


O_METHOD_BEGIN(exp_w_PropertyExampleObject, GenericTaskWindowExample, "Generic Task Window Example", 0, 0, 0)
{
  if (VARLID_ARG)
  {
    if (auto m = CURRENT_MODULE)
    {
      auto creator = [&](QMap<QString, QVariant>& args, TModule* md)->QWidget *
      {
        if (w_PropertyHolderWidget* holder = new w_PropertyHolderWidget)
        {
          if (auto p = object->property("double2"))
          {
            if (w_Property* w = holder->addProperty(p))
            {
            }
          }

          return holder;
        }

        return nullptr;
      };

      QMap<QString, QVariant> args =
      {
        {"windowName", "Generic Task Window"},
        {"purpose", "GenericTaskWindowExample"}
      };


      if (auto * dlg = m->findOrCreateTaskWindow(args, creator))
      {
        return true;
      }
    }

    return false;
  }
}
O_METHOD_END;

