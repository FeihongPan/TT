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

#include "exp_CurveToplogyExampleObject.h"
#include "core_Application.h"
#include "w_QFile.h"
#include "w_TModule.h"
#include "w_TTaskWindow.h"

#include "mshs_Vector2D.h"
#include "mshs_Vector3D.h"

#include "core_Application.h"
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

REGISTER_OBJECT_CLASS(exp_CurveToplogyExampleObject, "CurveToplogyExampleObject", TObject);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_CurveToplogyExampleObject, "Curve Toplogy Example Class");

exp_CurveToplogyExampleObject::exp_CurveToplogyExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  setTag("MethodOrder", "curveTopologyScaleExample,curveTopologyExample,curveTopologyScaleInTaskWindow,curveTopologyInTaskWindow,curveTopologyAxis,$");
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

      // set the plot parameters
      /* available parameters
      unitCatX  -  0 
      unitCatY  -  0 
      unitNameX  -  "" 
      unitNameY  -  "" 

      lineType  -  (int)Qt::SolidLine
      lineColor  -  "black"
      lineWidth  -  1
      textColor  -  "black"
      fontType  -  ""
      fontSize  -  9

      arrowLength  -  0.0 
      arrowAngle  -  20.0 
      endSymbolSize  -  6 

      textLineType  -  (int)Qt::SolidLine
      textLineColor  -  "black"
      textLineWidth  -  1
      textTextColor  -  "black"
      textFontType  -  ""
      textFontSize  -  9

      scaleDistance  -  0.0 
      scaleExtension  -  0.0 
      scaleExtensionSkipDistance  -  0.0 
      angleScaleDistance  -  0.0 
      scaleFontSize  -  9
      scaleFontType  -  ""
      scaleParallelLineWidth  -  1
      scaleExtensionLineWidth  -  1
      scaleLineType  -  (int)Qt::SolidLine
      scaleLineColor  -  "black"
      scaleTextColor  -  "black"

      // shape fills
      fillPattern  -  (int)Qt::FDiagPattern
      fillPatternColor  -  "black"
      fillColor  -  "lightGrey"

      // method to set
      plot->getParameter("scaleExtensionSkipDistance").setValue(0.15);
      */

      if (auto plot = w->getPlot())
      {
        plot->setScaleDistance(0.6);        
        plot->setArrowSize(0.25);
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

O_METHOD_BEGIN(exp_CurveToplogyExampleObject, curveTopologyScaleExample, "Curve Topology and Scale Example Dialog", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->getTopology1())
    {
      showTopologyInDialog(topology);
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CurveToplogyExampleObject, curveTopologyExample, "Curve Topology Example Dialog", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->getTopology2())
    {
      showTopologyInDialog(topology);
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CurveToplogyExampleObject, curveTopologyScaleInTaskWindow, "Curve Topology and Scale Example Taskwindow", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->getTopology1())
    {
      if (TModule* m = CURRENT_MODULE)
      {
        if (TTaskWindow* w = m->findOrCreateTaskWindow("exp_PlotWindow", 0))
        {
          w->displayObject(topology);
          w->setWindowTitle("Topology Window 0");
        }
      }
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CurveToplogyExampleObject, curveTopologyInTaskWindow, "Curve Topology Example Taskwindow", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->getTopology2())
    {
      if (TModule* m = CURRENT_MODULE)
      {
        if (TTaskWindow* w = m->findOrCreateTaskWindow("exp_PlotWindow", 1))
        {
          w->displayObject(topology);
          w->setWindowTitle("Topology Window 1");
        }
      }
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_CurveToplogyExampleObject, curveTopologyAxis, "Curve Topology Axis", 0, 0, "")
{
  if (VARLID_ARG)
  {
    if (curve_Topology* topology = object->getTopology3())
    {
      showTopologyInDialog(topology);
    }
  }
}
O_METHOD_END;

curve_Topology* exp_CurveToplogyExampleObject::getTopology1()
{
  QString name = "topology1";
  curve_Topology* topology = dynamic_cast<curve_Topology*>(object(name));

  if (!topology)
  {
    if (topology = (curve_Topology*)TObject::new_object("curve_Topology", name, this))
    {
      curve_Curve* curve = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);
      curve_Curve* curve1 = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);
      curve_Curve* curve2 = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);

      curve_Curve* line = 0;
      if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      {
        c->setPoints(Double2(2, -2), Double2(2, 0));
        line = c;
      }

      curve_Curve* circle = 0;
      if (curve_Circle* c = dynamic_cast<curve_Circle*>(curve->addSegment(curve_Curve::Circle)))
      {
        c->setStartAngle(PI);
        c->setEndAngle(PI / 2);
        c->setRadius(1);
        c->setCenter(Double2(3, 0));
        circle = c;
      }

      curve_Curve* spline = 0;
      if (curve_Spline* c = dynamic_cast<curve_Spline*>(curve->addSegment(curve_Curve::Spline)))
      {
        QVector<Double2> points = {
          Double2(3, 1),
          Double2(3.5, 0.6),
          Double2(3.2, -0.5),
          Double2(4.5, -1)
        };
        c->setControlPoints(points);
        spline = c;
      }

      curve_Curve* nurbs = 0;
      if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
      {
        QVector<Double2> points = {
          Double2(4.5, -1),
          Double2(4.5, -2),
          Double2(3.6, -2.1),
          Double2(2.8, -3)
        };
        c->setControlPoints(points);
        nurbs = c;
      }

      curve_Curve* spline1 = 0;
      if (curve_Spline* c = dynamic_cast<curve_Spline*>(curve1->addSegment(curve_Curve::Spline)))
      {
        QVector<Double2> points = {
          Double2(4.5, -1),
          Double2(6.5, 1),
          Double2(7.5, 1.5)
        };
        c->setControlPoints(points);
        spline1 = c;
      }

      curve_Polygon* polygon = 0;
      if (curve_Polygon* c = dynamic_cast<curve_Polygon*>(curve1->addSegment(curve_Curve::Polygon)))
      {
        QVector<Double2> points = {
          Double2(4.5, -0),
          Double2(6.5, 2),
          Double2(7.5, 3.5)
        };
        c->setDouble2Vector(points);
        polygon = c;
      }


      // add connectivities
      topology->setEndPointConnected(circle, curve_Curve::StartPoint, line, curve_Curve::EndPoint);
      topology->setEndPointConnected(circle, curve_Curve::EndPoint, spline, curve_Curve::StartPoint);
      topology->setEndPointConnected(nurbs, curve_Curve::StartPoint, spline, curve_Curve::EndPoint);

      topology->setEndPointTangentDrive(line, curve_Curve::EndPoint, circle, curve_Curve::StartPoint);

      topology->setEndPointTangentDrive(spline, curve_Curve::EndPoint, nurbs, curve_Curve::StartPoint);
      topology->setEndPointFixed(nurbs, curve_Curve::EndPoint);
      topology->setEndPointConstantY(spline, 1);
      topology->setEndPointOnVector(spline, 2, Double2(0.5, 0.5));

      topology->setEndPointOnCurve(spline1, curve_Curve::StartPoint, curve);

      topology->setEndPointOnCurve(polygon, curve_Curve::StartPoint, curve);

      // add scales
      if (auto s = topology->addDistanceScale())
      {
        s->setStartPointName(topology->firstPointName(line));
        s->setEndPointName(topology->lastPointName(line));
      }

      if (auto s = topology->addAngleScale())
      {
        s->setStartPointName(topology->firstPointName(line));
        s->setEndPointName(topology->lastPointName(circle));
        s->setCenterName(topology->firstPointName(circle));
      }

      if (auto s = topology->addFilledShapeCurves())
      {
        QStringList L = { topology->curveToPath(circle) };
        s->setCurves(L);
      }

      if (auto s = topology->addFilledShapeCircle())
      {
        s->setCenterPoint(topology->pointToName(nurbs, 2));
        s->setRadius(0.3);
      }

      if (auto s = topology->addFilledShape())
      {
        QStringList L = {
          topology->pointToName(spline, 1),
          topology->pointToName(spline, 2),
          topology->pointToName(spline, 3),
        };
        s->setPointNames(L);
        s->setFillColor("lightGreen");
      }

      if (auto s = topology->addText())
      {
        s->setText("Text with lead line");
        QStringList L = {
          topology->pointToName(nurbs, 1)
        };
        s->setStartPoints(L);

        QVector<Double2> LL = {
          {1, 1},
          {1.7, 1},
        };
        s->setOffsetExtentions(LL);
      }

      // finalize the dependencies
      topology->updateDependencies0();
    }
  }

  return topology;
}

curve_Topology* exp_CurveToplogyExampleObject::getTopology2()
{
  QString name = "topology2";
  curve_Topology* topology = dynamic_cast<curve_Topology*>(object(name));

  if (!topology)
  {
    if (topology = (curve_Topology*)TObject::new_object("curve_Topology", name, this))
    {
      curve_Curve* curve = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);
      curve_Curve* curve1 = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);
      curve_Curve* curve2 = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);

      curve_Curve* line = 0;
      if (curve_Line* c = dynamic_cast<curve_Line*>(curve->addSegment(curve_Curve::Line)))
      {
        c->setPoints(Double2(2, -2), Double2(2, 0));
        line = c;
      }

      curve_Curve* circle = 0;
      if (curve_Circle* c = dynamic_cast<curve_Circle*>(curve->addSegment(curve_Curve::Circle)))
      {
        c->setStartAngle(PI);
        c->setEndAngle(PI / 2);
        c->setRadius(1);
        c->setCenter(Double2(3, 0));
        circle = c;
      }

      curve_Curve* spline = 0;
      if (curve_Spline* c = dynamic_cast<curve_Spline*>(curve->addSegment(curve_Curve::Spline)))
      {
        QVector<Double2> points = {
          Double2(3, 1),
          Double2(3.5, 0.6),
          Double2(3.2, -0.5),
          Double2(4.5, -1)
        };
        c->setControlPoints(points);
        spline = c;
      }

      curve_Curve* nurbs = 0;
      if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
      {
        QVector<Double2> points = {

          Double2(4.5, -1),
          Double2(4.5, -2),
          Double2(3.6, -2.1),
          Double2(2.8, -3)
        };
        c->setControlPoints(points);
        nurbs = c;
      }

      curve_Curve* spline1 = 0;
      if (curve_Spline* c = dynamic_cast<curve_Spline*>(curve1->addSegment(curve_Curve::Spline)))
      {
        QVector<Double2> points = {
          Double2(4.5, -1),
          Double2(6.5, 1),
          Double2(7.5, 1.5)
        };
        c->setControlPoints(points);
        spline1 = c;
      }

      curve_Polygon* polygon = 0;
      if (curve_Polygon* c = dynamic_cast<curve_Polygon*>(curve1->addSegment(curve_Curve::Polygon)))
      {
        QVector<Double2> points = {
          Double2(4.5, -0),
          Double2(6.5, 2),
          Double2(7.5, 3.5)
        };
        c->setDouble2Vector(points);
        polygon = c;
      }

      // add connectivities
      topology->setEndPointConnected(circle, curve_Curve::StartPoint, line, curve_Curve::EndPoint);
      topology->setEndPointConnected(circle, curve_Curve::EndPoint, spline, curve_Curve::StartPoint);
      topology->setEndPointConnected(nurbs, curve_Curve::StartPoint, spline, curve_Curve::EndPoint);

      topology->setEndPointTangentDrive(line, curve_Curve::EndPoint, circle, curve_Curve::StartPoint);

      topology->setEndPointTangentDrive(spline, curve_Curve::EndPoint, nurbs, curve_Curve::StartPoint);
      topology->setEndPointFixed(nurbs, curve_Curve::EndPoint);
      topology->setEndPointConstantY(spline, 1);
      topology->setEndPointOnVector(spline, 2, Double2(0.5, 0.5));

      topology->setEndPointOnCurve(spline1, curve_Curve::StartPoint, curve);

      topology->setEndPointOnCurve(polygon, curve_Curve::StartPoint, curve);

      // finalize the dependencies
      topology->updateDependencies0();
    }
  }

  return topology;
}

curve_Topology* exp_CurveToplogyExampleObject::getTopology3()
{
  QString name = "topology3";
  curve_Topology* topology = dynamic_cast<curve_Topology*>(object(name));

  if (!topology)
  {
    if (topology = (curve_Topology*)TObject::new_object("curve_Topology", name, this))
    {
      curve_Curve* curve = (curve_Curve*)TObject::new_object("curve_Curve", "", topology);

      curve_Curve* nurbs = 0;
      if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
      {
        QVector<Double2> points = {

          Double2(0, -60),
          Double2(0.5, -5),
          Double2(1., -30)
        };
        c->setControlPoints(points);
        nurbs = c;
      }

      // add connectivities
      topology->setEndPointConstantX(nurbs, 0);
      topology->setEndPointConstantX(nurbs, 2);
    
      // finalize the dependencies
      topology->updateDependencies0();
    }
  }

  return topology;
}