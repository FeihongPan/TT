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

 /* class example
 *
 * TODO: put int x under private
 */
class MyClass
{
  int x;
public:
  MyClass(int val) : x(val) {}
  const int& get() const { return x; }
};

O_METHOD_BEGIN(TObject, cppExample01Class, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    MyClass m(40);
    printMessage("value = %d", m.get());
    /* * * * * * * example start * * * * * * */
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
  T values[2];
public:
  MyPair(T first, T second)
  {
    values[0] = first; values[1] = second;
  }
  T getmax()
  {
    T retval;
    retval = values[0] > values[1] ? values[0] : values[1];
    return retval;
  }
};

O_METHOD_BEGIN(TObject, cppExample02template, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    MyPair<int>  m(40, 20);
    printMessage("max = %d", m.getmax());
    /* * * * * * * example start * * * * * * */
  }
}
O_METHOD_END;


/* friend class example
*
* TODO: modify to add a perimeter function to Rectangle
*/
class Square;
class Rectangle1 {
  int width, height;
public:
  int area() { return (width * height); }
  void convert(Square a);
};

class Square {
  friend class Rectangle1;
private:
  int side;
public:
  Square(int a) : side(a) {}
};

void Rectangle1::convert(Square a) {
  width = a.side; // access of private members
  height = a.side;
}

O_METHOD_BEGIN(TObject, cppExample03FriendClass, 0, 0, 0, "")
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
protected:
  int width, height;
public:
  void set_values(int a, int b) { width = a; height = b; }
  virtual int area() { return 0; }
};

class Rectangle : public Polygon
{
public:
  int area() { return width * height; }
};

class Triangle : public Polygon
{
public:
  int area() { return (width * height / 2); }
};

O_METHOD_BEGIN(TObject, cppExample04Polymorphism, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    Rectangle rect;
    Polygon * ppoly1 = &rect;
    ppoly1->set_values(4, 5);
    cout << ppoly1->area() << '\n';
    printMessage("area = %g", ppoly1->area());
    /* * * * * * * example start * * * * * * */
  }
}
O_METHOD_END;

/* Exception example
*
* TODO: modify to throw a string exception
*/
O_METHOD_BEGIN(TObject, cppExample05Exception, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    try
    {
      throw 20;
    }
    catch (int e)
    {
      printMessage("Int exception occurred. Exception int %d", e);
    }
    catch (char e)
    {
      printMessage("Char exception occurred. Exception char %c", e);
    }
    /* * * * * * * example start * * * * * * */
  }
}
O_METHOD_END;

/* QString example
*
* TODO: to modify b to change ',' to ';'
*/
O_METHOD_BEGIN(TObject, cppExample06String, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    /* * * * * * * example start * * * * * * */
    QString a = "Today, it is sunny, and warm.";
    QString b = a;
    printMessage("a = %s \n b = %s", a.ascii(), b.ascii());
    /* * * * * * * example start * * * * * * */
  }
}
O_METHOD_END;
