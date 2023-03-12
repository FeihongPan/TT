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

#include "exp_QtContainerExampleObject.h"

REGISTER_OBJECT_CLASS(exp_QtContainerExampleObject, "QtContainerExampleObject", TObject);

exp_QtContainerExampleObject::exp_QtContainerExampleObject(QString object_n, TObject *iparent) :
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

  setTag("MethodOrder", "qt01_QString,qt02_QStringList,qt03_QVector,qt04_QMap,qt05_QVariant,$");
}

/* QString example
*
* TODO: to modify b to change 'sunny' to 'cloudy'
*/
O_METHOD_BEGIN(exp_QtContainerExampleObject, qt01_QString, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    // assign a QString
    QString a = "Today, it is sunny, and warm.";
    QString b = a;    

    // to check if a string contains a sub string
    bool c = a.contains("it is"); // should be true
    bool d = a.contains("it-is"); // should be false

    // to check if a string starts with a sub string
    bool e = b.startsWith("Today"); // should be true
    bool f = b.startsWith("Yesterday"); // should be false

    // to check if a string ends with a sub string
    bool g = b.endsWith("warm."); // should be true
    bool h = b.endsWith("Cold"); // should be false

    // split a string to a QStringList
    QStringList L = a.split(","); // should produce a string list of size 3
    auto a1 = L.join(",");
    bool equal = (a == a1); // should be true

    // replace
    QString m = a;
    m.replace("warm", "cold"); // string changed to "Today, it is sunny, and cold."

    printMessage("a = %s\nm = %s", a.ascii(), m.ascii());

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* QStringList example
*
* TODO: to modify a to remove the last two strings
*/
O_METHOD_BEGIN(exp_QtContainerExampleObject, qt02_QStringList, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    // assign a QStringList
    QStringList a = { "compressor", "turbine", "fan", "pump" };
    QStringList b = QStringList () << "compressor" << "turbine" << "fan" << "pump";
    QStringList c = QString("Today, it is sunny, and warm.").split(",");
    QStringList d = a;
    d.push_back("truck");
    d += c;
    
    // reference a string in a string list
    QString e = a[1]; // turbine
    QString f = a.at(1); // turbine
    QString g = a.first(); // compressor
    QString h = a.last(); // pump

    // to check if a string list contains a  string
    bool i = a.contains("fan"); // should be true
    bool j = a.contains("car"); // should be false

    // string list to string by join
    QString k = a.join(","); // compressor,turbine,fan,pump

    // replace "fan" to "car"
    b.replace(b.indexOf("fan"), "car");

    printMessage("d = %s", d.join(", ").ascii());

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* QVector example
*
* TODO: to modify a to remove the last two numbers
*/
O_METHOD_BEGIN(exp_QtContainerExampleObject, qt03_QVector, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    // assign a QVector
    QVector<double> a = { 1.2, 3.4, 5.6, 7.8 };
    QVector<double> b = QVector<double>() << 1.2 << 3.4 << 5.6 << 7.8;
    auto c = a;
    c.push_back(2.3);
    c += b;
    
    // reference
    auto& e = a[1]; // 3.4
    auto f = a.at(1); // 3.4
    auto g = a.first(); // 1.2
    auto h = a.last(); // 7.8    

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* QMap example
*
* TODO: to modify a's "first" value to 2.9
*/
O_METHOD_BEGIN(exp_QtContainerExampleObject, qt04_QMap, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    // assign a QMap
    QMap<QString, double> a = {
      {"first", 1.3},
      {"another", 1.5},
      {"some", 1.7},
    };

    QMap<QString, double> b;
    b["first"] = 1.3;
    b["another"] = 1.5;
    b["some"] = 1.7;

    auto c = a;
    c["any"] = 1.9;

    // reference
    auto d = a.find("some");
    if (d != a.end())
    {
      auto key = d.key();
      auto value = d.value();
    }
    auto e = a.contains("first"); // true
    auto f = a["first"]; // 1.3

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* QVariant example
*
* TODO: to modify a's "first" value to "some names"
*/
O_METHOD_BEGIN(exp_QtContainerExampleObject, qt05_QVariant, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    // assign a QMap
    QMap<QString, QVariant> a = {
      {"first", 1.3},
      {"another", "string"},
      {"some", true},
    };

    QMap<QString, QVariant> b;
    b["first"] = 1.3;
    b["another"] = "string";
    b["some"] = true;

    auto c = a;
    c["any"] = false;

    // reference
    auto d = a.find("some");
    if (d != a.end())
    {
      auto key = d.key();
      auto value = d.value().toString();
    }
    auto e = a.contains("first"); // true
    auto f = a["first"].toDouble(); // 1.3

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;
