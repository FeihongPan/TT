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

#include "exp_CppExampleObject.h"

REGISTER_OBJECT_CLASS(exp_CppExampleObject, "CppExampleObject", TObject);

exp_CppExampleObject::exp_CppExampleObject(QString object_n, TObject *iparent) :
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

  setTag("MethodOrder", "cpp01_class,"
    "cpp02_template,"
    "cpp03_friend_class,"
    "cpp04_polymorphism_override,"
    "cpp05_operator_overload,"
    "cpp06_exception,"
    "cpp07_lambda,"
    "cpp08_std_function_and_lambda,"
    "cpp09_summary,"
    "$");
}

/* class example
*
* TODO: put int x under private
*/
class MyClass
{
public:
  MyClass(int val) : x(val) {}

  const int& get() const { return x; }

  void set(int i) { x = i; }

private:
  int x;
};

O_METHOD_BEGIN(exp_CppExampleObject, cpp01_class, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    MyClass m(40);

    m.set(50);

    printMessage("value = %d", m.get());
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* template example
*
* TODO: modify to add the getmin function
*/
template <class T>
class MyPair
{  
public:
  MyPair(T first, T second)
  {
    values[0] = first; values[1] = second;
  }

  const T& getmax() const
  {
    return values[0] > values[1] ? values[0] : values[1];
  }

#if 0
  const T* const getmax1() const // equivalent to const T& getmax() const
  {
    return values[0] > values[1] ? &values[0] : &values[1];
  }

  const T* getmax2() const
  {
    return values[0] > values[1] ? &values[0] : &values[1];
  }

  const T getmax3() const
  {
    return values[0] > values[1] ? values[0] : values[1];
  }
#endif

  QString getMaxAsString() //const
  {
    const T& retval = getmax();

    QString s = QString("%1").arg(retval);

    return s;
  }

private:
  T values[2];
};

O_METHOD_BEGIN(exp_CppExampleObject, cpp02_template, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    MyPair<int>  m(40, 20);

    MyPair<double>  m1(40.1, 20.2);

    MyPair<QString>  m2("string 1", "string 2");

    printMessage("max = %s", m2.getMaxAsString().ascii());
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;


/* friend class example
*
* TODO: modify to add a perimeter function to Rectangle
*/
class Square;
class Rectangle1 
{  
public:
  int area() { return (width * height); }
  void convert(Square a);

private:
  int width, height;
};

class Square 
{
  friend class Rectangle1;

public:
  Square(int a) : side(a) {}

private:
  int side;
};

void Rectangle1::convert(Square a) 
{
  width = a.side; // access of private members
  height = a.side;
}

O_METHOD_BEGIN(exp_CppExampleObject, cpp03_friend_class, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    Square square(10);
    Rectangle1 rectangle;
    rectangle.convert(square);
    printMessage("area = %g", rectangle.area());
    /* * * * * * * example start * * * * * * */
  }
}
O_METHOD_END;

/* Polymorphism example
*
* TODO: modify to add a perimeter function
*/
class Polygon
{
public:
  void set_values(int a, int b) { width = a; height = b; }
  virtual int area() const = 0;// { return 0; }

protected:
  int width, height;
};

class Rectangle : public Polygon
{
public:
  virtual int area() const override { return width * height; }
};

class Triangle : public Polygon
{
public:
  virtual int area() const  override { return (width * height / 2); }
};

static int getArea(const Polygon& p)
{
  return p.area();
}

O_METHOD_BEGIN(exp_CppExampleObject, cpp04_polymorphism_override, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    Rectangle rect;
    Polygon& ppoly1 = rect;
    ppoly1.set_values(4, 5);
    cout << getArea(rect) << '\n';
    printMessage("area = %g", ppoly1.area());
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

class Vector 
{
public:  
  Vector(double a = 0, double b = 0) : x(a), y(b) {}

  Vector operator + (const Vector&);

  operator double() const
  {
    return sqrt(x*x + y * y);
  }

private:
  double x;
  double y;

  /*
  // lambda as a private class data member
  std::function<int(int, int)> add = [](int i, int j) -> int
  {
    return i + j;
  };
  */
};

Vector Vector::operator+ (const Vector& param) 
{
  Vector temp;
  temp.x = x + param.x;
  temp.y = y + param.y;

  return temp;
}

O_METHOD_BEGIN(exp_CppExampleObject, cpp05_operator_overload, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    Vector v1(1.4, 6.7);
    Vector v2(2.4, 1.7);

    auto v = v1 + v2;

    double len = v;
    
    printMessage("length = %g", len);
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

/* Exception example
*
* TODO: modify to throw a string exception
*/
O_METHOD_BEGIN(exp_CppExampleObject, cpp06_exception, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    int number = 15;
    try
    {
      if(number < 20)
        throw number;

      double a = sqrt(number - 20);
#if 0
      // recursive
      try
      {
        a += 2;
      }
      catch (...)
      {

      }
#endif
    }
    catch (int e)
    {
      printMessage("Int exception occurred. Exception int %d", e);
    }
    catch (char e)
    {
      printMessage("Char exception occurred. Exception char %c", e);
    }
    catch (...)
    {
      printMessage("exception happened!");
    }
    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CppExampleObject, cpp07_lambda, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    int j = 10;
    /* * * * * * * example start * * * * * * */
    auto plus_j = [&](int i)
    {
      return i + j;
    };

    auto a  = 2;
    eprintf("a plus_1 = %d", plus_j(a));

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

static int add1(std::function<int(int, int)> f, int i, int j)
{
  return f(i, j);
}

O_METHOD_BEGIN(exp_CppExampleObject, cpp08_std_function_and_lambda, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    int abc = 5;
    
    auto add = [abc](int i, int j) -> int
    {
      return i + j + abc;
    };

    auto a = 2;

    std::function<int(int, int)> f = add;

    eprintf("add = %d", add1(f, a, 5));

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CppExampleObject, cpp09_summary, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */

    QStringList summary = 
    {
      "01: If you have two places uses similar code, do NOT copy, but use a function",
      "02: If you can use some thing on the stack if temparorily, do not use heap",
      "03: Try to access class data through getter/setter, instead of direct data access",
      "04: Try to avoid static_cast, use dynamic_cast instead",
      "05: Try to refactor the code first before adding new functionality",
      "06: Be careful when you cache a pointer, because it may out dated"
    };    

    printMessage("\n%s\n", summary.join("\n").ascii());

    /* * * * * * * example end * * * * * * */
  }
}
O_METHOD_END;
