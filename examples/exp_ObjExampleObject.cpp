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
 * be covered by U.S. and Foreign Patents, patents in proces
 * and are protected by trade secret or copyright law.      
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written      
 * permission is obtained from TaiZe Inc.                   
 */                                                         

#include "exp_ObjExampleObject.h"
#include "core_Application.h"
#include "w_QFile.h"
#include "w_PropertyHolderDialog.h"

REGISTER_OBJECT_CLASS(exp_ObjExampleObject, "ObjExampleObject", TObject);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_ObjExampleObject, "Object Example Class");

exp_ObjExampleObject::exp_ObjExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  DEFINE_SCALAR_INIT(int, anInt, "An Int", 0, NULL, NULL); 
  anInt = 5;
  DEFINE_SCALAR_INIT(bool, aBool, "A Bool", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, aDouble, "A Double", 0, NULL, TUnit::length); 
  aDouble = 12.5;
  DEFINE_QSTRING_INIT(aString, "AString", 0, NULL, NULL); aString = "A string";
  DEFINE_QSTRING_INIT(sDateTime, "Date Time", 0, NULL, NULL); 
  property("sDateTime")->setEditorCallback("TCallbackDateTimeEditor");
  DEFINE_QSTRINGLIST_INIT(aStringList, "AStringList", 0, NULL, NULL); 
  aStringList = QStringList() << "string 1" << "string 2";
  DEFINE_QDOUBLE_VECTOR_INIT(aQVectorDouble, "AQVectorDouble", 0, 0, NULL, TUnit::temperature);
  aQVectorDouble = {1.2, 3.3, 4.5};
  DEFINE_QDOUBLE2_VECTOR_INIT(aQVectorDouble2, "AQVectorDouble2", 0, 0, NULL, NULL);
  aQVectorDouble2 = { {1.2, 3.4}, {2.3, 6.7}, {4.5, 8.9} };
  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(aQVectorVectorDouble, "AQVectorVectorDouble", 0, 0, NULL, NULL);
  aQVectorVectorDouble = { {1.2, 3.4, 5.6, 7.8}, {2.3, 6.7}, {4.5, 8.9, 9.9} };

  // SET_OBJ_HIDE(this);

  setTag("MethodOrder", "XMLExportTest,flatXMLExportTest,$,JSONExportTest,flatJSONExportTest,$,createObjectClassSourceFile,createMethodArgs,$");
}

#include "io_XMLUtil.h"

O_METHOD_BEGIN(exp_ObjExampleObject, XMLExportTest, "Export and Import XML of an object", 0, 0, "XML test")
{
  // argument section starts, should be in proper odering
  DEFINE_ARG(QString, fileName, "a.xml", 0, 0, "file name");
  SET_ARG_CALLBACK_FILE_SAVE(fileName);

  if (VARLID_ARG)
  {
    if (auto p = object->property("aDouble"))
    {
      printMessage("Before write: %s = %g", p->dname().ascii(), p->toDouble());
    }

    // write
    {
      io_XMLUtil xml(fileName);
      xml.addObject(object);
      int compressitionLevel = 0;
      xml.write(compressitionLevel);
    }

    //read
    {
      io_XMLUtil xml(fileName);
      xml.read();
      QString pName = "aDouble";
      auto v = xml.findProperty(object->oname(), pName);
      if (v.isValid())
      {
        printMessage("After read: %s = %g", pName.ascii(), v.toDouble());
      }
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjExampleObject, flatXMLExportTest, "Export Multiple Object XML Format ", 0, 0, "XML test")
{
  // argument section starts, should be in proper odering
  DEFINE_ARG(QString, fileName, "b.xml", 0, 0, "file name");
  SET_ARG_CALLBACK_FILE_SAVE(fileName);

  if (VARLID_ARG)
  {
    // write
    {
      io_XMLUtil xml(fileName);
      loop_o_child(object->parent(), c)
        xml.addObject(c);
      int compressitionLevel = 0;
      xml.write(compressitionLevel);
    }

    //read
    {
      io_XMLUtil xml(fileName);
      xml.read();
      QString pName = "aDouble";
      auto v = xml.findProperty(object->oname(), pName);
      if (v.isValid())
      {
        printMessage("After read: %s = %g", pName.ascii(), v.toDouble());
      }
    }

    return true;
  }
}
O_METHOD_END;


#include "io_JSONUtil.h"

O_METHOD_BEGIN(exp_ObjExampleObject, JSONExportTest, "Export and Import JSON of an object", 0, 0, "JSON test")
{
  // argument section starts, should be in proper odering
  DEFINE_ARG(QString, fileName, "a.json", 0, 0, "file name");
  SET_ARG_CALLBACK_FILE_SAVE(fileName);

  if (VARLID_ARG)
  {
    // write
    {
      io::JSONUtil json(fileName);
      json.addObject(object);
      int compressitionLevel = 0;
      json.write(compressitionLevel);
    }

    //read
    {
      io::JSONUtil json(fileName);
      json.read();
      QString pName = "aDouble";
      auto v = json.findProperty(object->oname(), pName);
      if (v.isValid())
      {
        printMessage("After read: %s = %g", pName.ascii(), v.toDouble());
      }
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjExampleObject, flatJSONExportTest, "Export Multiple Object JSON Format", 0, 0, "JSON test")
{
  // argument section starts, should be in proper odering
  DEFINE_ARG(QString, fileName, "b.json", 0, 0, "file name");
  SET_ARG_CALLBACK_FILE_SAVE(fileName);

  if (VARLID_ARG)
  {
    // write
    {
      io::JSONUtil json(fileName);
      loop_o_child(object->parent(), c)
        json.addObject(c);
      int compressitionLevel = 0;
      json.write(compressitionLevel);
    }

    //read
    {
      io::JSONUtil json(fileName);
      json.read();
      QString pName = "aDouble";
      auto v = json.findProperty(object->oname(), pName);
      if (v.isValid())
      {
        printMessage("After read: %s = %g", pName.ascii(), v.toDouble());
      }
    }

    return true;
  }
}
O_METHOD_END;















static QMap<QString, QString> getVariableInfo(const QString& line)
{
  QMap<QString, QString> info;

  QStringList L = line.split(" ");
  if (L.size() >= 2)
  {
    QString type = L[0];
    QString name = L[1];

    info["type"] = L[0];

    if (name.endsWith(";"))
      name = name.left(name.size() - 1);
    info["varName"] = name;

    QString baseName = name;

    if (name.startsWith("_"))
      name = name.right(name.size() - 1);

    QString upper = name.toUpper();
    name[0] = upper[0];

    info["baseName"] = name;

    if (L.size() > 3)
    {
      QString value = L[3];
      if (value.endsWith(";"))
        value = value.left(value.size() - 1);
      info["value"] = value;
    }
  }

  return info;
}

static QMap<QString, QString> getSourceInfo(const QString& name, QVector< QMap<QString, QString>>& vars)
{
  QMap<QString, QString> info;
  if (name.size())
  {
    QFile file(name);
    if (file.open(w_QFile::ReadOnly))
    {
      QTextStream in(&file);
      bool started = false;
      while (!in.atEnd())
      {
        QString line = in.readLine().simplified();

        if (!started)
        {
          if (line.startsWith("class T"))
          {
            QStringList L = line.split(" ");
            if (L.size() > 2)
              info["class"] = L[2];
            if (L.size() > 5)
              info["classBase"] = L[5];
          }
        }

        if (line.size())
        {
          if (!started)
          {
            if (line.indexOf("private:") >= 0)
            {
              started = true;
            }
          }
          else if (line.indexOf("(") < 0 && line.indexOf(")") < 0)
          {
            QStringList L = line.split(" ");
            if (L.size() >= 2)
            {
              auto a = getVariableInfo(line);
              if (a.size())
              {
                vars.push_back(a);
              }
            }
          }
        }
      }
    }
  }

  return info;
}

static void createSourceFileFromHeader(const QString& name, bool writeCpp = true)
{
  QVector< QMap<QString, QString>> vars;
  QMap<QString, QString> info = getSourceInfo(name, vars);

  eprintf("\n=============\n%s\n\n", name.ascii());

  foreach(auto v, vars)
  {
    auto varName = v["varName"];
    auto type = v["type"];
    auto  baseName = v["baseName"];
    // setter
    QString s = QString("void set%1(const %2& i){%3 = i;}")
      .arg(baseName)
      .arg(type)
      .arg(varName);
    eprintf("%s\n", s.ascii());

    // getter
    s = QString("%1 get%2()const{return %3;}")
      .arg(type)
      .arg(baseName)
      .arg(varName);
    eprintf("%s\n", s.ascii());

    eprintf("\n");
  }

  // source file with constructor
  QStringList L;
  QString className = info["class"];
  if (className.size())
  {
    QString s = QString("#include \"%1.h\"").arg(className);
    L.push_back(s);
    L.push_back("\n");

    QString shortName = className;
    {
      QStringList LL = className.split("_");
      if (LL.size() > 1)
        shortName = LL[1];
    }
    s = QString("REGISTER_OBJECT_CLASS(%1, \"%2\", %3);")
      .arg(className)
      .arg(shortName)
      .arg(info["classBase"]);
    L.push_back(s);

    L.push_back("\n");

    s = QString("%1::%1(QString object_n, TObject *iparent) :\n  %2(object_n, iparent)")
      .arg(className)
      .arg(info["classBase"]);
    L.push_back(s);

    L.push_back("{");

    foreach(auto v, vars)
    {
      auto varName = v["varName"];
      auto type = v["type"];
      auto value = v["value"];
      auto  baseName = v["baseName"];

      s.clear();
      if (type == "int" || type == "double" || type == "bool")
      {
        s = QString("  DEFINE_SCALAR_INIT(%1, %2, \"%3\", 0, NULL, NULL);")
          .arg(type)
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QString")
      {
        s = QString("  DEFINE_QSTRING_INIT(%1, \"%2\", 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QStringList")
      {
        s = QString("  DEFINE_QSTRINGLIST_INIT(%1, \"%2\", 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QVector<double>")
      {
        s = QString("  DEFINE_QDOUBLE_VECTOR_INIT(%1, \"%2\", 0, 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QVector<Double2>")
      {
        s = QString("  DEFINE_QDOUBLE2_VECTOR_INIT(%1, \"%2\", 0, 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "Double2")
      {
        s = QString("  DEFINE_SCALAR_INIT(double, %1.x[0], \"%2.x\", 0, NULL, NULL);\n")
          .arg(varName)
          .arg(baseName);
        s += QString("  DEFINE_SCALAR_INIT(double, %1.x[1], \"%2.y\", 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QVector<QVector<double>>")
      {
        s = QString("  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(%1, \"%2\", 0, 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }
      else if (type == "QVector<QVector<QVector<double>>>")
      {
        s = QString("  DEFINE_QDOUBLE_VECTOR_VECTOR_VECTOR_INIT(%1, \"%2\", 0, 0, NULL, NULL);")
          .arg(varName)
          .arg(baseName);
      }

      if (s.size())
      {
        if (value.size())
        {
          s += QString(" %1 = %2;")
            .arg(varName)
            .arg(value);
        }

        L.push_back(s);
      }
    }


    L.push_back("}");
  }

  // print
  eprintf("\n\n");
  foreach(auto s, L)
  {
    eprintf("%s", s.ascii());
  }

  if (L.size() && writeCpp)
  {
    QString cpp = name;
    cpp = cpp.left(cpp.size() - 2);
    cpp += ".cpp";

    QFile file(cpp);
    if (file.open(w_QFile::WriteOnly))
    {
      QTextStream out(&file);

      out << "/*************************************************************************" << endl;
      out << " *                                                                     " << endl;
      out << " * TURBOTIDES                                                          " << endl;
      out << " * An Integrated CAE Platform for Turbomachinery Design and Development" << endl;
      out << " * ____________________________________________________________________" << endl;
      out << " *                                                          " << endl;
      out << " *  [2016] - [2021]  TaiZe Inc                              " << endl;
      out << " *  All Rights Reserved.                                    " << endl;
      out << " *                                                          " << endl;
      out << " * NOTICE:  All information contained herein is, and remains" << endl;
      out << " * the property of TaiZe Inc and its suppliers, if any.     " << endl;
      out << " * The intellectual and technical concepts contained herein " << endl;
      out << " * are proprietary to TaiZe Inc and its suppliers and may   " << endl;
      out << " * be covered by U.S. and Foreign Patents, patents in proces" << endl;
      out << " * and are protected by trade secret or copyright law.      " << endl;
      out << " * Dissemination of this information or reproduction of this" << endl;
      out << " * material is strictly forbidden unless prior written      " << endl;
      out << " * permission is obtained from TaiZe Inc.                   " << endl;
      out << " */                                                         " << endl;

      out << endl;

      foreach(auto s, L)
      {
        out << s << endl;
      }
    }
  }
}

#include "w_QFileDialog.h"
O_METHOD_BEGIN(exp_ObjExampleObject, createObjectClassSourceFile, "Create Object Class Source", 0, 0, "createObjectClassSourceFile")
{
  DEFINE_ARG(bool, writeCPP, false, 0, "write CPP File?", 0);
  if (VARLID_ARG)
  {
    QStringList names =
      w_QFileDialog::getOpenFileNames(core_Application::core(), QObject::tr("Browse open file"),
        QDir::currentPath());

    if (names.size())
    {
      foreach(auto name, names)
      {
        createSourceFileFromHeader(name, writeCPP);
      }
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjExampleObject, editDateTime, "Edit Date Time", 0, 0, "editDateTime")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample01");

    w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs");

    QString dateTime;
    property_t* p = object->property("sDateTime");
    if (w_Property* w = holder->addProperty(p))
    {

    }
    //if (w_Property* w = holder->addProperty(&dateTime, "Date Time", 0, false, false, false, "TCallbackDateTimeEditor")) { w->setToolTip("File tooltip"); }

    if (dlg.exec() == QDialog::Accepted)
    {
      dateTime = p->toString();
      eprintf("Date time is %s", qPrintable(dateTime));
    }
  }
}
O_METHOD_END;


static QVector< QMap<QString, QString>> getCommandArgInfo(const QString& name)
{
  QVector< QMap<QString, QString>> vars;

  if (name.size())
  {
    QFile file(name);
    if (file.open(w_QFile::ReadOnly))
    {
      QTextStream in(&file);
      while (!in.atEnd())
      {
        QString line = in.readLine().simplified();

        if (line.size())
        {
          {
            QStringList L = line.split(" ");
            if (L.size() >= 2)
            {
              auto a = getVariableInfo(line);
              if (a.size())
              {
                vars.push_back(a);
              }
            }
          }
        }
      }
    }
  }

  return vars;
}

static void printMethodArgs(const QString& fileName)
{
  auto vars = getCommandArgInfo(fileName);
  foreach(auto v, vars)
  {
    auto varName = v["varName"];
    auto type = v["type"];
    auto value = v["value"];
    auto  baseName = v["baseName"];

    if (type == "QString")
      eprintf("DEFINE_ARG(%s, %s, \"%s\", 0, \"%s\", 0);", type.ascii(), varName.ascii(), value.ascii(), baseName.ascii());
    else
      eprintf("DEFINE_ARG(%s, %s, %s, 0, \"%s\", 0);", type.ascii(), varName.ascii(), value.ascii(), baseName.ascii());
  }
}

O_METHOD_BEGIN(exp_ObjExampleObject, createMethodArgs, "Create Method Args", 0, 0, "createMethodArgs")
{
  if (VARLID_ARG)
  {
    QString name =
      w_QFileDialog::getOpenFileName(core_Application::core(), QObject::tr("Browse open file"),
        QDir::currentPath());

    if (name.size())
    {
      printMethodArgs(name);
    }
  }
}
O_METHOD_END;
