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
#include "w_PropertyDialog.h"

 /* define an object class example
 *
 */

class test_Object : public TObject
{
  T_OBJECT;

public:
  test_Object(const char *object_n = NULL, TObject *iparent = NULL);
  virtual ~test_Object() {}

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

REGISTER_OBJECT_CLASS(test_Object, "testObject", TObject);

test_Object::test_Object(const char *object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;
  DEFINE_SCALAR_INIT(int, anInt, NULL, 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, aBool, NULL, 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, aDouble, "A double number", 0, NULL, TUnit::pressure); aDouble = 72.9;

  DEFINE_QSTRING_INIT(aString, "A string example", 0, NULL, NULL);
  DEFINE_QSTRINGLIST_INIT(aStringList, "A string list", 0, NULL, NULL);

  DEFINE_QINT_VECTOR_INIT(aQVectorInt, "A Qvector<int> example", 0, NULL, NULL);

  DEFINE_QDOUBLE_VECTOR_INIT(aQVectorDouble, "A Qvector<double> example", 0, 0, NULL, NULL);
  DEFINE_QDOUBLE2_VECTOR_INIT(aQVectorDouble2, "A Qvector<Double2> example", 0, 0, NULL, NULL);
  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(aQVectorVectorDouble, "A Qvector<Qvector<double>> example", 0, 0, NULL, NULL);
}

static void showObject(TObject* o)
{
  QStringList L = QStringList() << o->path();
  TPropertyDialog::setProperty(L, 0, "", true);
}

O_METHOD_BEGIN(TObject, objExample01, 0, 0, 0, "")
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
        if (TObject* testObject = TObject::new_object("test_Object", "testObject1", holder))
        {
          printMessage("Object generated: %s with class type %s",
            testObject->oname(),
            testObject->cname()
          );

          /* make sure it shows up in the tree */
          TObject::refreshTreeWithVisable(testObject);

          /* set an int property value */
          if (property_t* p = testObject->property("anInt"))
          {
            p->setValue(158);
          }

          /* set an int vector property value */
          if (property_t* p = testObject->property("aQVectorInt"))
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


/* create multiple objects
*
*/
O_METHOD_BEGIN(TObject, objExample02, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    /* find the module root object */
    if (TObject* moduleRoot = object->parent("util_ModuleRoot"))
    {
      /* create an test_Object object with automatic name */
      if (TObject* holder = TObject::new_object("TObject", 0, moduleRoot))
      {
        QStringList L = QStringList()
          << "test_Object"
          << "test_Object"
          << "TObject";
        /* TODO:
        create another object of any class type you want
        */
        for (int i = 0; i < L.size(); i++)
        {
          if (TObject* testObject = TObject::new_object(L[i].ascii(), "", holder))
          {
            printMessage("Object generated: %s with class type %s",
              testObject->oname(),
              testObject->cname()
            );

            /* make sure it shows up in the tree */
            TObject::refreshTreeWithVisable(testObject);

            /* set an int property value */
            if (property_t* p = testObject->property("anInt"))
            {
              p->setValue(i + 1);
            }
            else
            {
              printError("I cannot find property %s", "anInt");
            }

            /* try to display the object */
            showObject(testObject);
          }
        }
      }
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/*
TODO:
save out and read back the current case after run the objExample01
then run objExample03
*
*/

O_METHOD_BEGIN(TObject, objExample03, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    /* find the module root object */
    if (TObject* moduleRoot = object->parent("util_ModuleRoot"))
    {
      /* find an object with class name test_Object */
      if (TObject* testObject = moduleRoot->firstChild("test_Object", true))
      {
        showObject(testObject);
      }
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* delete an object
*
*/
O_METHOD_BEGIN(TObject, objExample04, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    /* find the module root object */
    if (TObject* moduleRoot = object->parent("util_ModuleRoot"))
    {
      /* create an test_Object object with automatic name */
      if (TObject* holder = TObject::new_object("TObject", 0, moduleRoot))
      {
        QStringList L = QStringList()
          << "test_Object"
          << "test_Object"
          << "TObject";
        /* TODO:
        create another object of any class type you want
        */
        for (int i = 0; i < L.size(); i++)
        {
          if (TObject* testObject = TObject::new_object(L[i].ascii(), "", holder))
          {
            /* make sure it shows up in the tree */
            TObject::refreshTreeWithVisable(testObject);

            /* set an int property value */
            if (property_t* p = testObject->property("anInt"))
            {
              p->setValue(i + 1);
            }

          }
        }

        TObject::refreshTreeWithVisable(holder);


        /* delete the first test_Object */
        if (TObject* testObject = holder->firstChild("test_Object", true))
        {
          printMessage("I am going to delete %s", testObject->oname());
          TObject::delete_object(testObject);
        }
      }
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;


/* find property from another object
* run example01, example02
* from testObject2 of object2 find first test_Object of object1,
* then set its aQVectorDouble2, then show it
*/
O_METHOD_BEGIN(TObject, objExample05, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    if (TObject* moduleRoot = object->parent("util_ModuleRoot"))
    {
      /* find an object with class name test_Object */
      if (TObject* object1 = moduleRoot->child("object1"))
      {
        if (TObject* testObject = object1->firstChild("test_Object"))
        {
          /* set an int vector property value */
          if (property_t* p = testObject->property("aQVectorDouble2"))
          {
            QVector<Double2>& a = p->asDouble2Vector();
            a.clear();
            a.push_back(Double2(1.2, 1.3));
          }

          showObject(testObject);
        }
      }
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;


/* add a simple method to show the object
*/
O_METHOD_BEGIN(TObject, objExample06, 0, 0, 0, "")
{
  DEFINE_ARG(bool, showAMessage, true, 0, "Show a message", "Show a message");

  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    if (showAMessage)
      printMessage("This is object  %s with class type %s",
        object->oname(),
        object->cname());
    showObject(object);
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

#include "io_XML.h"
/* save out the object
*/
O_METHOD_BEGIN(test_Object, objExample07Save, 0, 0, 0, "")
{
  DEFINE_ARG(QString, fileName, "", 0, "File name", "File name");

  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    if (!fileName.size())
      fileName = QString(object->oname()) + ".xml";

    TXML::o2f(object, fileName, 0, 0);
    printMessage("Object %s saved to file %s", object->oname(), fileName.ascii());
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;


/* read  the object
*/
O_METHOD_BEGIN(test_Object, objExample08Read, 0, 0, 0, "")
{
  DEFINE_ARG(QString, fileName, "", 0, "File name", "File name");

  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    if (!fileName.size())
      fileName = QString(object->oname()) + ".xml";

    if (QFile(fileName).exists())
    {
      if (TObject* o = TXML::f2o(fileName))
      {
        if (QString(o->cname()) == object->cname())
        {
          *(dynamic_cast<TObject*>(object)) = *o;
          //TObject::copyObject(object, o);
        }
        printMessage("Read Object %s from file %s", object->oname(), fileName.ascii());
        TObject::refreshTreeWithVisable(object);

        showObject(object);
      }
    }
    else
    {
      printError("File %s not available", fileName.ascii());
    }

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;
