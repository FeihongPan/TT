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

#include "exp_ObjMethodExampleObject.h"
#include "core_Application.h"
#include "w_QFile.h"

REGISTER_OBJECT_CLASS(exp_ObjMethodExampleObject, "ObjMethodExampleObject", TObject);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_ObjMethodExampleObject, "Object Method Example Class");

exp_ObjMethodExampleObject::exp_ObjMethodExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  DEFINE_SCALAR_INIT(int, anInt, "AnInt", 0, NULL, NULL); 
  anInt = 5;
  DEFINE_SCALAR_INIT(bool, aBool, "ABool", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, aDouble, "ADouble", 0, NULL, TUnit::length); 
  aDouble = 12.5;
  DEFINE_QSTRING_INIT(aString, "AString", 0, NULL, NULL); aString = "A string";
  DEFINE_QSTRINGLIST_INIT(aStringList, "AStringList", 0, NULL, NULL); 
  aStringList = QStringList() << "string 1" << "string 2";
  DEFINE_QDOUBLE_VECTOR_INIT(aQVectorDouble, "AQVectorDouble", 0, 0, NULL, TUnit::temperature);
  aQVectorDouble = {1.2, 3.3, 4.5};
  DEFINE_QDOUBLE2_VECTOR_INIT(aQVectorDouble2, "AQVectorDouble2", 0, 0, NULL, NULL);
  aQVectorDouble2 = { {1.2, 3.4}, {2.3, 6.7}, {4.5, 8.9} };
  DEFINE_QDOUBLE_VECTOR_VECTOR_INIT(aQVectorVectorDouble, "AQVectorVectorDouble", 0, 0, NULL, NULL);
  aQVectorVectorDouble = { {1.2, 3.4, 5.6, 7.8}, {2.3, 6.7}, {4.5, 8.9, 9.9} };

  setTag("MethodOrder", "aMethod,bMethod,$,ArgsTabExample,ArgsTabExample2,$");
}


O_METHOD_BEGIN(exp_ObjMethodExampleObject, aMethod, "A Method", ":images/generic-icon.png", 0, "A Method Help")
{
  DEFINE_ARG(int, int1, 0, 0, "Int input", "Int input help dummy");
  DEFINE_ARG(double, double1, 1.0, TUnit::length, 0, "");
  DEFINE_ARG(bool, bool1, true, 0, 0, "");

  if (VARLID_ARG)
  {
    printMessage("int1 = %d", int1);

    return int1;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjMethodExampleObject, bMethod, "B Method", ":images/generic-icon.png", 0, "B Method Help")
{
  DEFINE_ARG(int, int1, 0, 0, "Int input", "Int input help dummy");
  RESET_ARG_DEFAULT(int1, object->getAnInt());

  if (VARLID_ARG)
  {
    printMessage("int1 = %d", int1);

    return int1;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjMethodExampleObject, ArgsTabExample, "Args Tab Example 1", ":images/generic-icon.png", 0, "ArgsTabExample")
{
  DEFINE_ARG(bool, wrapAsMacro, true, 0, 0, "wrap as macro");

  DEFINE_ARG(int, int1, 0, 0, 0, "");
  RESET_ARG_DEFAULT(int1, object->getAnInt());

  DEFINE_ARG(int, int2, 0, 0, 0, "");
  SET_ARG_PAGE(int2, "", "Group 0");
  DEFINE_ARG(int, int3, 0, 0, 0, "");
  SET_ARG_EXPERT(int3);
  SET_ARG_PAGE(int3, "", "Group 0");

  DEFINE_ARG(double, double1, 1.0, TUnit::length, 0, "");
  SET_ARG_PAGE(double1, "Options", "Group 1");
  DEFINE_ARG(double, double2, 1.0, TUnit::angle, 0, "");
  SET_ARG_PAGE(double2, "Options", "Group 1");
  

  DEFINE_ARG(double, double3, 1.0, TUnit::mass, 0, "");
  SET_ARG_PAGE(double3, "Options", "");
  SET_ARG_EXPERT(double3);
  DEFINE_ARG(double, double4, 1.0, TUnit::power, 0, "");
  SET_ARG_PAGE(double4, "Options", "Group 2");
  SET_ARG_EXPERT(double4);

  DEFINE_ARG(double, double5, 1.0, TUnit::length, 0, "");
  DEFINE_ARG(double, double6, 1.0, TUnit::angle, 0, "");
  SET_ARG_EXPERT(double6);
  SET_ARG_PAGE(double6, "Options", "");
  DEFINE_ARG(double, double7, 1.0, TUnit::mass, 0, "");
  DEFINE_ARG(double, double8, 1.0, TUnit::power, 0, "");
  SET_ARG_HIDE_GROUP_IF_TRUE(wrapAsMacro, "Group 2");


  DEFINE_ARG(QString, string1, "", 0, 0, "");
  static QStringList choices = { "choice 1", "choice 2", "choice 3" };
  SET_ARG_COMBO(string1, &choices);

  DEFINE_ARG(QString, string2, "File to open", 0, "File to open", "");
  SET_ARG_PAGE(string2, "Options", "Group 2");
  SET_ARG_CALLBACK_FILE_OPEN(string2);

  DEFINE_ARG(QString, string3, "File to save", 0, "File to save", "");
  SET_ARG_PAGE(string3, "Options", "Group 2");
  SET_ARG_CALLBACK_FILE_SAVE(string3);

  DEFINE_ARG(QString, string4, "Folder", 0, "Folder", "");
  SET_ARG_PAGE(string4, "Options", "Group 2");
  SET_ARG_CALLBACK_DIR_BROWSE(string4);

  if (VARLID_ARG)
  {
    //QList<QVariant> args = { 5, 2.3, true };
    //object->execute("aMethod", args);

    printMessage("int1 = %d", int1);
    object->setAnInt(int1);    
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_ObjMethodExampleObject, ArgsTabExample2, "Args Tab Example 2", ":images/generic-icon.png", 0, "ArgsTabExample2")
{
  class MethodFilterArgsTabExample2 : public util_MethodFilter
  {
  public:
    virtual bool isShownGUI(TObject* o)
    {
      if (exp_ObjMethodExampleObject* oo = dynamic_cast<exp_ObjMethodExampleObject*>(o))
      {
        if (oo->getAnInt() > 4)
          return true;
      }

      return false;
    }
  };
  static MethodFilterArgsTabExample2 _methodFilterArgsTabExample2;
  SET_METHOD_FILTER(&_methodFilterArgsTabExample2);

  DEFINE_ARG(bool, wrapAsMacro, false, 0, 0, "wrap as macro");
  SET_ARG_HIDE_IF_TRUE(wrapAsMacro, "int1");
  SET_ARG_HIDE_GROUP_IF_TRUE(wrapAsMacro, "Group 0;Group 1");
  DEFINE_ARG(int, int1, 0, 0, 0, "");
  RESET_ARG_DEFAULT(int1, object->getAnInt());
  SET_ARG_HIDE_GROUP_IF_VALUE(int1, 2, "Group 0;Group 1");
  DEFINE_ARG(int, int2, 0, 0, 0, "");
  SET_ARG_PAGE(int2, "", "Group 0");
  DEFINE_ARG(int, int3, 0, 0, 0, "");
  SET_ARG_EXPERT(int3);
  SET_ARG_PAGE(int3, "", "Group 0");
  SET_ARG_COLUMN(int3, 1);

  DEFINE_ARG(double, double1, 1.0, TUnit::length, 0, "");
  SET_ARG_PAGE(double1, "Options", "Group 1");
  DEFINE_ARG(double, double2, 1.0, TUnit::angle, 0, "");
  SET_ARG_PAGE(double2, "Options", "Group 1");
  SET_ARG_COLUMN(double2, 1);
  DEFINE_ARG(double, double3, 1.0, TUnit::mass, 0, "");
  SET_ARG_PAGE(double3, "Options", "");
  SET_ARG_EXPERT(double3);
  DEFINE_ARG(double, double4, 1.0, TUnit::power, 0, "");
  SET_ARG_PAGE(double4, "Options", "Group 2");
  SET_ARG_EXPERT(double4);

  DEFINE_ARG(double, double5, 1.0, TUnit::length, 0, "");
  DEFINE_ARG(double, double6, 1.0, TUnit::angle, 0, "");
  SET_ARG_EXPERT(double6);
  SET_ARG_PAGE(double6, "Options", "");
  DEFINE_ARG(double, double7, 1.0, TUnit::mass, 0, "");
  DEFINE_ARG(double, double8, 1.0, TUnit::power, 0, "");
  SET_ARG_HIDE_GROUP_IF_TRUE(double8, "Group 2");

  DEFINE_ARG(QString, string1, "", 0, 0, "");
  DEFINE_ARG(QString, string2, "", 0, 0, "");
  SET_ARG_PAGE(string2, "Options", "Group 2");

  if (VARLID_ARG)
  {
    printMessage("double1 = %g", double1);
    object->setADouble(double1);
  }
}
O_METHOD_END;
