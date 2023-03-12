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

#include "exp_CurveToplogyShroudMultiCurves.h"
#include "core_Application.h"
#include "w_QFile.h"
#include "w_TModule.h"
#include "w_TTaskWindow.h"

#include "mshs_Vector2D.h"
#include "mshs_Vector3D.h"

#include "w_PropertyString.h"
#include "w_PropertyDouble.h"
#include "util_Parameter.h"

#include "vis_Widget.h"
#include "curve_Curve.h"
#include "curve_Line.h"
#include "curve_Polygon.h"
#include "curve_Circle.h"
#include "curve_Spline.h"
#include "curve_Nurbs.h"
#include "curve_Topology.h"

#include "w_PropertyHolderDialog.h"
#include "w_PropertyHolderWidget.h"
#include "draw_TopologyInteractiveEditorWidget.h"
#include "draw_TopologyInteractiveEditor.h"




REGISTER_OBJECT_CLASS(exp_CurveToplogyShroudMultiCurves, "CurveToplogyShourdMultiCurves", TObject);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_CurveToplogyShourdMultiCurves, "Shourd Multi Curves Example Class");

exp_CurveToplogyShroudMultiCurves::exp_CurveToplogyShroudMultiCurves(QString object_n, TObject *iparent) :QObject(),
  TObject(object_n, iparent)
{
 // INIT_OBJECT;

  DEFINE_SCALAR_INIT(double, thickness_front, "thickness front", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, thickness_rear, "thickness rear", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, RC_front, "RC front", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, RC_rear, "RC rear", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, L_center_rear, "L distance of center", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, H_center_rear, "H distance of center", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, phi_rear, "contour angle rear", 0, NULL, TUnit::deltaAngle);
}

static void showTopologyInDialog(curve_Topology* topology)
{
  w_PropertyHolderDialog dlg;
  dlg.setWindowTitle("MultiCurveExample");
  if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
  {
    if (draw_TopologyInteractiveEditorWidget* w = new draw_TopologyInteractiveEditorWidget(holder))
    {
      w->setSizeHint(QSize(800, 600));
      if (auto plot = w->getPlot())
      {
        plot->setScaleDistance(0.6);        
        plot->setArrowSize(1);
        plot->setEndSymboSize(8);
        plot->getParameter("scaleExtensionSkipDistance").setValue(0.15);
      }

      w->setTopology(topology);

      holder->placeWidget(w);
    }

    if (dlg.exec() == QDialog::Accepted)
    {
    }
  }
}

void exp_CurveToplogyShroudMultiCurves::handleUpdateData()
{  
  this->makeShorudCurve();
  w->setTopology(this->createShroudMultiCurves());
  holder_topo->placeWidget(w);
  w->replot();
}

void exp_CurveToplogyShroudMultiCurves::showTopologyPropertiesInDialog(exp_CurveToplogyShroudMultiCurves* shroud)
{
  w_PropertyHolderDialog dlg;
  dlg.setWindowTitle("MultiCurveExample");

  w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1);
  //Add topo View
  holder_topo = holder->getHolder(0, 0, 1, 1, "Topo");

  if (w = new draw_TopologyInteractiveEditorWidget(holder_topo))
  {
    w->setSizeHint(QSize(800, 600));
    if (auto plot = w->getPlot())
    {
      plot->setScaleDistance(0.006);
      plot->setArrowSize(0.001);
      plot->setEndSymboSize(8);
      plot->getParameter("scaleExtensionSkipDistance").setValue(0.00001);
    }
    shroud->makeShorudCurve();
    w->setTopology(shroud->createShroudMultiCurves());
    holder_topo->placeWidget(w);
  }

  //Add topo View
  w_PropertyHolderWidget* holder_properties = holder->getHolder(0, 1, 1, 1, "Properties");

  w_QPushButton* apply = holder_properties->addButton("Click to Apply", 8, 0, 1, 2);
  QObject::connect(apply, SIGNAL(clicked()), this, SLOT(handleUpdateData()));
  //QObject::connect(apply, SIGNAL(clicked()), this, SLOT(test()));
  holder->addMappedWidget("button", apply);
  // 1.
  if (w_Property* w = holder_properties->addProperty( &(shroud->thickness_front), "thickness front", TUnit::length))
  {
    w->setAutoSave();
  }
  // 2.
  if (w_Property* w = holder_properties->addProperty(&(shroud->thickness_rear), "thickness rear", TUnit::length))
  {
    w->setAutoSave();
  }
  // 3.
  if (w_Property* w = holder_properties->addProperty(&(shroud->RC_front), "RC front", TUnit::length))
  {
    w->setAutoSave();
  }
  // 4.
  if (w_Property* w = holder_properties->addProperty(&(shroud->RC_rear), "RC rear", TUnit::length))
  {
    w->setAutoSave();
  }
  // 5.
  if (w_Property* w = holder_properties->addProperty(&(shroud->L_center_rear), "L distance of center", TUnit::length))
  {
    w->setAutoSave();
  }
  // 6.
  if (w_Property* w = holder_properties->addProperty(&(shroud->H_center_rear), "H distance of center", TUnit::length))
  {
    w->setAutoSave();
  }
  // 7.
  if (w_Property* w = holder_properties->addProperty(&(shroud->phi_rear), "contour angle rear", TUnit::deltaAngle))
  {
    w->setAutoSave();
  }

  if (dlg.exec() == QDialog::Accepted)
  {
  }

}


O_METHOD_BEGIN(exp_CurveToplogyShroudMultiCurves, curveTopologyOfShroudMultiCurves, "Original Shroud Multi Curves", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->createShroudMultiCurves(true))
    {

      showTopologyInDialog(topology);

      //if (TModule* m = CURRENT_MODULE)
      //{
      //  if (TTaskWindow* w = m->findOrCreateTaskWindow("exp_PlotWindow", 2))
      //  {
      //    w->displayObject(topology);
      //    w->setWindowTitle("Disk Curve Window 2");
      //  }
      //}
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CurveToplogyShroudMultiCurves, curveTopologyOfShroudMultiCurves_detail, "Detail Shroud Multi Curves", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->createShroudMultiCurves(false))
    {
      exp_CurveToplogyShroudMultiCurves* curretObj = object;

      object->showTopologyPropertiesInDialog(curretObj);

      //if (TModule* m = CURRENT_MODULE)
      //{
      //  if (TTaskWindow* w = m->findOrCreateTaskWindow("exp_PlotWindow", 2))
      //  {
      //    w->displayObject(topology);
      //    w->setWindowTitle("Disk Curve Window 2");
      //  }
      //}



    }
  }
}
O_METHOD_END;


curve_Topology * exp_CurveToplogyShroudMultiCurves::createShroudMultiCurves(bool createIfNotAvailable)
{
  QString name = "shorud";
  curve_Topology* topology = dynamic_cast<curve_Topology*> (object(name));

  if (!topology && createIfNotAvailable)
  {
    if (topology= dynamic_cast<curve_Topology*>(TObject::new_object("curve_Topology",name,this)))
    {
      curve_Curve* curve = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);

      curve_Curve* shroud = 0;
      if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
      {
        QVector<Double2> shroudPoints = {
          Double2(0, 0.140),
          Double2(0.016, 0.141),
          Double2(0.049, 0.149),
          Double2(0.067, 0.184),
          Double2(0.066, 0.200)
        };
        c->setControlPoints(shroudPoints);
        shroud = c;

        shroud_front = Double2(0, 0.140);
        shroud_rear = Double2(0.066, 0.200);
      }

      curve_Curve* hub = 0;
      if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
      {
        QVector<Double2> hubPoints = {
          Double2(0, 0.06),
          Double2(0.023, 0.06),
          Double2(0.075, 0.086),
          Double2(0.093, 0.165),
          Double2(0.093, 0.200)
        };
        c->setControlPoints(hubPoints);
        hub = c;

        hub_rear = Double2(0.093, 0.200);
      }

      curve_Curve* inlet = 0;
      if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      {
        QVector<Double2> inletPoints = {
          Double2(0, 0.06),
          Double2(0, 0.1405)
        };
        c->setPoints(inletPoints.first(),inletPoints.last());
        inlet = c;

      }

      curve_Curve* outlet = 0;
      if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      {
        QVector<Double2> outletPoints = {
          Double2(0.066, 0.20),
          Double2(0.093, 0.20)
        };
        c->setPoints(outletPoints.first(), outletPoints.last());
        outlet = c;
      }

      //curve_Curve* shroudHLine1 = 0;
      //if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      //{
      //  QVector<Double2> outletPoints = {
      //    Double2(0.066, 0.20),
      //    Double2(0.051, 0.20)
      //  };
      //  c->setPoints(outletPoints.first(), outletPoints.last());
      //  shroudHLine1 = c;

        thickness_rear = 0.01;

      //}

      //curve_Curve* shroudTiltLine = 0;
      //if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      //{
      //  QVector<Double2> outletPoints = {
      //    Double2(0.0510, 0.20),
      //    Double2(0.0459, 0.1860)
      //  };
      //  c->setPoints(outletPoints.first(), outletPoints.last());
      //  shroudTiltLine = c;
      //}

      //curve_Curve* shroudHLine2 = 0;
      //if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      //{
      //  QVector<Double2> outletPoints = {
      //    Double2(0, 0.1636),
      //    Double2(0.0153, 0.1636)
      //  };
      //  c->setPoints(outletPoints.first(), outletPoints.last());
      //  shroudHLine2 = c;
      //}

      //curve_Curve* shroudVLine = 0;
      //if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      //{
      //  QVector<Double2> outletPoints = {
      //    Double2(0.0, 0.14),
      //    Double2(0.0, 0.1636)
      //  };
      //  c->setPoints(outletPoints.first(), outletPoints.last());
      //  shroudVLine = c;

        thickness_front = 0.01;

      //}

      //curve_Curve* circleR2 = 0;
      //if (curve_Circle* c = dynamic_cast<curve_Circle*>(curve->addSegment(curve_Curve::Circle)))
      //{
      //  c->setCicle(Double2(0.0153, 0.1636), Double2(0.0322, 0.1687), Double2(0.0153, 0.1944));
      //  c->setRadius(0.030791506);
      //  //c->setCenter(Double2(0.0153, 0.1944));

      //  circleR2 = c;
      //  //
      //  //RC_front = 0.030791506;

      //}

      RC_front = 0.03;



      //curve_Curve* circleR1 = 0;
      //if (curve_Circle* c = dynamic_cast<curve_Circle*>(curve->addSegment(curve_Curve::Circle)))
      //{
      //  c->setCicle(Double2(0.0322, 0.1687), Double2(0.0459, 0.1860), Double2(0.013, 0.1979));
      //  //c->setStartAngle(PI);
      //  //c->setEndAngle(PI / 2);
      //  c->setRadius(0.035);
      //  //c->setCenter(Double2(0.013, 0.1979));

      //  circleR1 = c;     
      //  //

      //}

      RC_rear = 0.05;
      L_center_rear = 0.08;
      H_center_rear = 0.1979;


      double rad = PI / 180.;
      phi_rear = 80 * rad;


      // add connectivities
      topology->setEndPointFixed(inlet, curve_Curve::EndPoint);
      topology->setEndPointFixed(outlet, curve_Curve::StartPoint);
      //topology->setEndPointConstantX(shroudVLine, 1);
      //topology->setEndPointConstantY(shroudHLine1, 0);
      //topology->setEndPointConstantY(shroudHLine2, 0);
      //topology->setEndPointConnected(inlet, curve_Curve::EndPoint, shroudVLine, curve_Curve::StartPoint);

      //topology->setEndPointConnected(shroudVLine, curve_Curve::EndPoint, shroudHLine2, curve_Curve::StartPoint);
      //topology->setEndPointConnected(outlet, curve_Curve::StartPoint, shroudHLine1, curve_Curve::StartPoint);
      //topology->setEndPointConnected(shroudHLine1, curve_Curve::EndPoint, shroudTiltLine, curve_Curve::StartPoint);
      //topology->setEndPointConnected(shroudHLine2, curve_Curve::EndPoint, circleR2, curve_Curve::StartPoint);
      //topology->setEndPointConnected(circleR2, curve_Curve::EndPoint, circleR1, curve_Curve::StartPoint);
      //topology->setEndPointConnected(circleR1, curve_Curve::EndPoint, shroudTiltLine, curve_Curve::EndPoint);

      //topology->setEndPointTangentDrive(shroudTiltLine, curve_Curve::EndPoint, circleR1, curve_Curve::EndPoint);
      //topology->setEndPointTangentDrive(circleR2, curve_Curve::EndPoint, circleR1, curve_Curve::StartPoint);
      //topology->setEndPointTangentDrive(shroudHLine2, curve_Curve::EndPoint, circleR2, curve_Curve::StartPoint);
      //topology->setEndPointOnVector(shroudHLine2, curve_Curve::EndPoint,Double2(1,0));

      //topology->setEndPointTangentDrive(spline, curve_Curve::EndPoint, nurbs, curve_Curve::StartPoint);
      //topology->setEndPointFixed(nurbs, curve_Curve::EndPoint);
      //topology->setEndPointConstantY(spline, 1);
      //topology->setEndPointOnVector(spline, 2, Double2(0.5, 0.5));
    }
  }

  return topology;
}

int exp_CurveToplogyShroudMultiCurves::getCurveType(int index)
{
  int curveType = 0;

  if (index == 0)
    curveType = curve_Curve::Line;
  else if (index == 1) 
    curveType = curve_Curve::Line;
  else if (index == 2)
    curveType = curve_Curve::Circle;
  else if (index == 3)
    curveType = curve_Curve::Circle;
  else if (index == 4)
    curveType = curve_Curve::Line;
  else
    curveType = curve_Curve::Line;

  return curveType;
}

QString exp_CurveToplogyShroudMultiCurves::getCurveTypeName(int curveType)
{
  QString curveTypeName;
  if (curveType == curve_Curve::Line)
    curveTypeName = "Line";
  else
    curveTypeName = "Circle";
  return curveTypeName;
}

curve_Curve* exp_CurveToplogyShroudMultiCurves::getShroudMultiCurve(bool createIfNotAvailable)
{
  curve_Topology* T = createShroudMultiCurves();
  if (!T)
    return nullptr;

  QString name = "ShroudMultiCurve";
  curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
  if (!c && createIfNotAvailable)
    c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
  if (c)
    return c;

  return nullptr;
}

curve_Curve* exp_CurveToplogyShroudMultiCurves::getSubCurve(int index, bool createIfNotAvailable)
{
  QString name = QString::number(index) + getCurveTypeName(getCurveType(index));

  curve_Curve* c = getShroudMultiCurve()->getCurveByName(name);
  if (c)
    return c;

  if (!c && createIfNotAvailable)
    c = getShroudMultiCurve()->addSegment(getCurveType(index), name);
  if (c)
    return c;

  return nullptr;
}

// assiting annotations
void exp_CurveToplogyShroudMultiCurves::setAssiting()
{
  setAssiting_RearTangentDirection();

  setAssiting_HeightOfRearCircle();

  setAssiting_RadiusOfRearCircle();

  setAssiting_RadiusOfFrontCircle();

  setAssiting_FrontThickness();

  setAssiting_RearThickness();

}

void exp_CurveToplogyShroudMultiCurves::setAssiting_RearTangentDirection()
{
  curve_Curve* c = getSubCurve(4);
  curve_Line* L = dynamic_cast<curve_Line*>(c);
  Double2 endPoint = (L->getPoint(1))*2. - L->getPoint(0.);
  Double2 centerPoint = L->getPoint(1);
  Double2 firstPoint = shroud_rear;

  curve_Topology* T = createShroudMultiCurves();
  


  if (auto s = T->addAngleScale())
  {
    s->setStartPoint(firstPoint);
    s->setCenter(centerPoint);
    s->setEndPoint(endPoint);
    //s->setStartPointName(T->firstPointName(line));
    //s->setEndPointName(T->lastPointName(circle));
    //s->setCenterName( T->firstPointName(circle));
  }
}

void exp_CurveToplogyShroudMultiCurves::setAssiting_HeightOfRearCircle()
{
  curve_Curve* c = getSubCurve(3);
  curve_Circle* circle = dynamic_cast<curve_Circle*>(c);

  Double2 center = circle->getCenter();
  // add scales
  if (auto s = createShroudMultiCurves()->addDistanceScale())
  {
    s->setStartPoint(center);
    Double2 pos = { center[0], 0};
    s->setEndPoint(pos);
  }
}

void exp_CurveToplogyShroudMultiCurves::setAssiting_RadiusOfRearCircle()
{
  curve_Curve* c = getSubCurve(3);
  curve_Circle* circle = dynamic_cast<curve_Circle*>(c);

  Double2 center = circle->getCenter();
  Double2 edge = circle->getPoint(0.5);
  // add scales
  if (auto s = createShroudMultiCurves()->addDistanceScale())
  {
    s->setStartPoint(center);
    s->setEndPoint(edge);
  }
}

void exp_CurveToplogyShroudMultiCurves::setAssiting_RadiusOfFrontCircle()
{
  curve_Curve* c = getSubCurve(2);
  curve_Circle* circle = dynamic_cast<curve_Circle*>(c);

  Double2 center = circle->getCenter();
  Double2 edge = circle->getPoint(0.5);
  // add scales
  if (auto s = createShroudMultiCurves()->addDistanceScale())
  {
    s->setStartPoint(center);
    s->setEndPoint(edge);
  }
}

void exp_CurveToplogyShroudMultiCurves::setAssiting_FrontThickness()
{
  curve_Curve* c = getSubCurve(1);
  curve_Line* L = dynamic_cast<curve_Line*>(c);

  Double2 fristPoint = L->getPoint(0);
  Double2 endPoint = shroud_front;
  // add scales
  if (auto s = createShroudMultiCurves()->addDistanceScale())
  {
    s->setStartPoint(fristPoint);
    s->setEndPoint(endPoint);
  }
}

void exp_CurveToplogyShroudMultiCurves::setAssiting_RearThickness()
{
  curve_Curve* c = getSubCurve(4);
  curve_Line* L = dynamic_cast<curve_Line*>(c);

  Double2 fristPoint = L->getPoint(1);
  Double2 endPoint = shroud_rear;
  // add scales
  if (auto s = createShroudMultiCurves()->addDistanceScale())
  {
    s->setStartPoint(fristPoint);
    s->setEndPoint(endPoint);
  }
}



Double2 exp_CurveToplogyShroudMultiCurves::getPosition(QString postion)
{
  Double2 Pos;
  if (postion == "in")
  {
    Pos = shroud_front + Double2(0., thickness_front);
  }
  else
  {
    Pos = shroud_rear - Double2(thickness_rear, 0.);
  }
  return Pos;
}

Double2 exp_CurveToplogyShroudMultiCurves::getTangetial(QString postion)
{
  Double2 T;
  if (postion == "in")
    T = Double2(1., 0.);
  else
    T = Double2(cos(phi_rear), sin(phi_rear));
  return T;
}

int exp_CurveToplogyShroudMultiCurves::makeShorudCurve()
{

  // main creat mutli-curvess
  auto unit = [&](Double2 s)->Double2
  {
    Double2 s_unit = s / s.length();
    return s_unit;
  };

  double Rc_frontCircle = RC_front;
  Double2 pt_start = getPosition("in");
  Double2 pt_end = getPosition("out");

  double Rc_rearCircle = RC_rear;

  double H1 = H_center_rear;// TODO

  double dis0 = H1 - Rc_frontCircle;
  double dis1 = Rc_rearCircle;
  // 1.
  Double2 pt_center_rearCircle;


  double phi2 = phi_rear;
  double D2 = shroud_rear[1]*2; // TODO
  double deltaR = H1 - D2 / 2.;
  double deltaZ = deltaR * tan(PI / 2 - phi2);
  double L_axial = Rc_rearCircle / cos(PI / 2 - phi2) - deltaZ;

  pt_center_rearCircle = Double2(pt_end[0], H1) - Double2(L_axial, 0);


  double Ds = getPosition("in")[1]*2; // TODO

  // 2. 
  double cos_alpha0 = (H1 - Rc_frontCircle -Ds / 2.) / (Rc_rearCircle - Rc_frontCircle);
  double alpha0 = acos(cos_alpha0);
  // 2-1. 
  Double2 D1 = {0.,-1.};
  D1 = D1.rotate(alpha0);
  D1 = unit(D1);

  Double2 pt_center_frontCircle = pt_center_rearCircle + D1 * (Rc_rearCircle - Rc_frontCircle);
  Double2 pt_tangent_BetweenfrontCircleWithRear = pt_center_rearCircle + D1 * Rc_rearCircle;


  // front thickness line
  {
    curve_Curve* c = getSubCurve(0);
    curve_Line* L = dynamic_cast<curve_Line*>(c);
    L->setPoints(shroud_front, pt_start);
  }


  // front line
  Double2 pt_start_frontLine = pt_start;
  Double2 pt_end_frontLine = getPedal(pt_center_frontCircle, pt_start, getTangetial("in"));
  
  // setFrontLineShroud(pt_start_frontLine, pt_end_frontLine);
  {
    curve_Curve* c = getSubCurve(1);
    curve_Line* L = dynamic_cast<curve_Line*>(c);
    L->setPoints(pt_start_frontLine, pt_end_frontLine);
  }

  // front Circle
  Double2 pt_start_frontCircle = pt_end_frontLine;
  Double2 pt_end_frontCircle = pt_tangent_BetweenfrontCircleWithRear;
  double angle_start_frontCircle = angleCycle(pt_center_frontCircle, pt_start_frontCircle);
  double angle_end_frontCircle = angleCycle(pt_center_frontCircle, pt_end_frontCircle); // <== 
  {
    curve_Curve* c = getSubCurve(2);
    curve_Circle* circle = dynamic_cast<curve_Circle*>(c);

    circle->setStartAngle(angle_start_frontCircle);
    circle->setEndAngle(angle_end_frontCircle);
    circle->setRadius(Rc_frontCircle);
    circle->setCenter(pt_center_frontCircle);
  }

  // rear circle
  Double2 pt_start_rearCircle = pt_end_frontCircle;
  Double2 pt_end_rearCircle = getPedal(pt_center_rearCircle, pt_end, getTangetial("out"));
  double angle_start_rearCircle = angleCycle(pt_center_rearCircle, pt_start_rearCircle);
  double angle_end_rearCircle = angleCycle(pt_center_rearCircle, pt_end_rearCircle);
  {
    curve_Curve* c = getSubCurve(3);
    curve_Circle* circle = dynamic_cast<curve_Circle*>(c);
   
    circle->setStartAngle(angle_start_rearCircle);
    circle->setEndAngle(angle_end_rearCircle);
    circle->setRadius(Rc_rearCircle);
    circle->setCenter(pt_center_rearCircle);
  }
  // setRearCircleShroud(pt_center_rearCircle, Rc_rearCircle, angle_start_rearCircle, angle_end_rearCircle);

  // rea line
  Double2 pt_start_rearLine = pt_end_rearCircle;
  Double2 pt_end_rearLine = pt_end;
  // setRearLineShroud(pt_start_rearLine, pt_end_rearLine);
  {
    curve_Curve* c = getSubCurve(4);
    curve_Line* L = dynamic_cast<curve_Line*>(c);
    L->setPoints(pt_start_rearLine, pt_end_rearLine);
  }

  // reae thicnkess
  {
    curve_Curve* c = getSubCurve(5);
    curve_Line* L = dynamic_cast<curve_Line*>(c);
    L->setPoints(pt_end, shroud_rear);
  }



  //setAssiting();

  return 0;
}

Double2 exp_CurveToplogyShroudMultiCurves::getPedal(Double2 pt, Double2 pt_line, Double2 T_line)
{
  Double2 Pedal;

  Double2 P_line = T_line.rotate(PI / 2.);

  curve_Line Line1;
  Line1.setPoints(pt_line, pt_line + T_line);
  curve_Line Line2;
  Line2.setPoints(pt, pt + P_line);

  QPointF ptf;
  int insectionType = Line1.intersect(Line2, &ptf);

  if (insectionType == QLineF::NoIntersection)
    return Pedal;

  Pedal = Double2(ptf.x(), ptf.y());

  return Pedal;
}

double exp_CurveToplogyShroudMultiCurves::angleCycle(Double2 pt_center, Double2 pt)
{
  double angle = Double2(1., 0.).angleSigned((pt - pt_center));
  if (angle < 0.)
    angle += 2 * PI;

  return angle;
}
