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

#include "exp_OptimizationExampleObject.h"
#include "w_PropertyHolderDialog.h"

REGISTER_OBJECT_CLASS(exp_OptimizationExampleObject, "OptimizationExample", TObject);

exp_OptimizationExampleObject::exp_OptimizationExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  setTag("MethodOrder", "OptimizationMinimizeCurveLength,OptimizationExampleWithTargetValue,optiMaxCurveLength,$");    
}

#include "mshs_Vector2D.h"
#include "mshs_Vector3D.h"
#include "io_XMLUtil.h"
#include "core_Application.h"
#include "w_PropertyString.h"
#include "w_PropertyDouble.h"
#include "util_Parameter.h"
#include "w_PropertyHolderDialog.h"
#include "w_PropertyHolderWidget.h"
#include "draw_Curve.h"
#include "vis_Widget.h"
#include "curve_Curve.h"
#include "curve_Line.h"
#include "curve_Polygon.h"
#include "curve_Circle.h"
#include "curve_Spline.h"
#include "curve_Nurbs.h"
#include "opt_OptimizationDefineUnderOwner.h"
#include "w_ParametricDlg.h"

#include "opt_GlobalOptimizationDefine.h"

// an example to optimize the nurbs control points to minimize the curve length
O_METHOD_BEGIN(exp_OptimizationExampleObject, OptimizationMinimizeCurveLength, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    QString name = "OptimizationMinimizeCurveLength";
    Double2 start(4.5, -1);
    Double2 end(2.8, -3);
    curve_Curve* curve = dynamic_cast<curve_Curve*>(object->object(name));
    if (!curve)
    {
      curve = dynamic_cast<curve_Curve*>(TObject::new_object("curve_Curve", name, object));

      if (curve)
      {
        // add parameters
        // create a curve to optimize
        if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
        {
          QVector<Double2> points = {
            start,
            Double2(4.5, -2),
            Double2(3.6, -2.1),
            end
          };
          c->setControlPoints(points);

          // create parameters based on the control points
          if (property_t* p = c->property("_controlPoints"))
          {
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 1, 0 }))
              .setName("x1")
              .setAsOptimizationDefaultInput()
              ;
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 1, 1 }))
              .setName("y1")
              .setAsOptimizationDefaultInput()
              ;

            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 2, 0 }))
              .setName("x2")
              .setAsOptimizationInput()
              ;
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 2, 1 }))
              .setName("y2")
              .setAsOptimizationInput()
              ;
          }
        }
      }

      QString optimizationObjectName = "optimizerMinimize";
      opt_Parametric* o = nullptr;
      if (!(o = opt_Parametric::findObjectOptimizer(optimizationObjectName, curve)))
      {
        if (o = opt_Parametric::newObjectOptimizer(optimizationObjectName, curve))
        {
          // set minimize the object
          o->setOptMethodMinimize();

          // set the curve length as the objective
          o->setObjectiveFunction([curve](const opt_Parametric* p) {
            return curve->getLength();
          });

          // no penalty
          o->setPenalizeFunction([curve](const opt_Parametric* p) {
            return 1;
          });
        }
      }

      if (o)
      {
        vis_Widget* vis = nullptr;
        w_PropertyHolderDialog curveShowDlg;
        draw_Curve* last_curve = nullptr;

        // set the solve function as to show the curve, in comparison with a straight line
        o->setSolveFunction([&](const opt_Parametric* p)
        {
          if (!vis)
          {
            curveShowDlg.setWindowFlag(Qt::WindowStaysOnTopHint);
            curveShowDlg.setModal(false);
            curveShowDlg.setWindowTitle("Curve show");

            if (w_PropertyHolderWidget* holder = curveShowDlg.getHolder(0, 0, 1, 1))
            {
              if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetDraw", holder))
              {
                vis = w;
                w->setSizeHint(QSize(800, 600));
                w->setKeepAspect();
                w->displayCurve(*curve, "Example curve");
                holder->placeWidget(w);
              }
              curveShowDlg.show();
            }
          }

          // change the last curve to grey color
          if (last_curve)
          {
            last_curve->setColor("grey").setLineWidth(1);
          }
          else // draw the straight line
          {
            QVector < QVector<Double2>> points = { { start, end} };

            auto L = vis->displayCurves(points);
            if (L.size())
            {
              if (auto draw = static_cast<draw_Curve*>(L.first()))
              {
                draw->setColor("green").setLineWidth(2);
              }
            }
          }

          // draw the current curve
          if (auto curve_draw = static_cast<draw_Curve*>(vis->displayCurve(*curve, "Example curve")))
          {
            curve_draw->setColor("red").setLineWidth(2);
            last_curve = curve_draw;
          }

          // successful
          return 0;
        });

        // show the optimization dialog
        auto ret = w_ParametricDlg::optimizeBase(o, name);
      }
    }
  }
}
O_METHOD_END;


// an example to optimize the nurbs control points to get a length of 3.2
O_METHOD_BEGIN(exp_OptimizationExampleObject, OptimizationExampleWithTargetValue, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    QString name = "OptimizationExampleWithTargetValue";
    curve_Curve* curve = dynamic_cast<curve_Curve*>(object->object(name));
    if (!curve)
    {
      curve = dynamic_cast<curve_Curve*>(TObject::new_object("curve_Curve", name, object));

      if (curve)
      {
        // add parameters
        // create a curve to optimize
        if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs)))
        {
          QVector<Double2> points = {
            Double2(4.5, -1),
            Double2(4.5, -2),
            Double2(3.6, -2.1),
            Double2(2.8, -3)
          };
          c->setControlPoints(points);

          // create parameters based on the control points
          if (property_t* p = c->property("_controlPoints"))
          {
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 1, 0 }))
              .setName("x1")
              .setAsOptimizationDefaultInput()
              ;
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 1, 1 }))
              .setName("y1")
              .setAsOptimizationDefaultInput()
              ;

            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 2, 0 }))
              .setName("x2")
              .setAsOptimizationDefaultInput()
              ;
            c->addParameter<util_Parameter>(p, &(QVector<int>() = { 2, 1 }))
              .setName("y2")
              .setAsOptimizationDefaultInput()
              ;
          }
        }
      }

      QString optimizationObjectName = "optimizerTargetValue";
      opt_Parametric* o = nullptr;
      if (!(o = opt_Parametric::findObjectOptimizer(optimizationObjectName, curve)))
      {
        if (o = opt_Parametric::newObjectOptimizer(optimizationObjectName, curve))
        {
          // set the target length as 3.2
          o->setOptMethodTargetValue(3.2);

          // set the curve length as the objective
          o->setObjectiveFunction([curve](const opt_Parametric* p) {
            return curve->getLength();
          });

          // no penalty
          o->setPenalizeFunction([curve](const opt_Parametric* p) {
            return 1;
          });
        }
      }

      if (o)
      {
        vis_Widget* vis = nullptr;
        w_PropertyHolderDialog curveShowDlg;
        draw_Curve* last_curve = nullptr;

        // set the solve function as to show the curve, in comparison with a straight line
        o->setSolveFunction([&](const opt_Parametric* p)
        {
          if (!vis)
          {
            curveShowDlg.setWindowFlag(Qt::WindowStaysOnTopHint);
            curveShowDlg.setModal(false);
            curveShowDlg.setWindowTitle("Curve show");

            if (w_PropertyHolderWidget* holder = curveShowDlg.getHolder(0, 0, 1, 1))
            {
              if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetDraw", holder))
              {
                vis = w;
                w->setSizeHint(QSize(800, 600));
                w->setKeepAspect();
                w->displayCurve(*curve, "Example curve");
                holder->placeWidget(w);
              }
              curveShowDlg.show();
            }
          }

          // change the last curve to grey color
          if (last_curve)
          {
            last_curve->setColor("grey").setLineWidth(1);
          }

          // draw the current curve
          if (auto curve_draw = static_cast<draw_Curve*>(vis->displayCurve(*curve, "Example curve")))
          {
            curve_draw->setColor("red").setLineWidth(2);
            last_curve = curve_draw;
          }

          // successful
          return 0;
        });

        // show the optimization dialog
        auto ret = w_ParametricDlg::optimizeBase(o, name);
      }
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_OptimizationExampleObject, optiMaxCurveLength, "optiMaxCurveLength", 0, 0, "") {
  {
    if (VARLID_ARG)
    {
      QString name = "OptiMaxCurveLen";
      Double2 startP(0.0, 0.0);
      Double2 endP(10.0, 10.0);

      curve_Curve* curve = dynamic_cast<curve_Curve *>(TObject::new_object("curve_Curve", name, object));
      if (curve)
      {
        QVector<Double2> allControlPoints = { startP,Double2(3.0,1.1),Double2(4.5,3.5),Double2(6.5,7.6),endP };
        curve_Nurbs* nurb = dynamic_cast<curve_Nurbs*>(curve->addSegment(curve_Curve::Nurbs, "nurbs curve"));
        nurb->setControlPoints(allControlPoints);
        if (property_t* p = nurb->property("_controlPoints"))
        {
          nurb->addParameter<util_Parameter>(p, &QVector<int>{1, 0}).setName("x_1").setAsOptimizationInput().setAsOptimizationDefaultInput();
          nurb->addParameter<util_Parameter>(p, &QVector<int>{1, 1}).setName("y_1").setAsOptimizationInput().setAsOptimizationDefaultInput();
          nurb->addParameter<util_Parameter>(p, &QVector<int>{2, 0}).setName("x_2").setAsOptimizationInput().setAsOptimizationDefaultInput();
          nurb->addParameter<util_Parameter>(p, &QVector<int>{2, 1}).setName("y_2").setAsOptimizationInput().setAsOptimizationDefaultInput();
          nurb->addParameter<util_Parameter>(p, &QVector<int>{3, 0}).setName("x_3").setAsOptimizationInput();
          nurb->addParameter<util_Parameter>(p, &QVector<int>{3, 1}).setName("y_3").setAsOptimizationInput();
        }
      }

      QString optObjectName = "optimizerMinimize";
      opt_Parametric* optParametric = nullptr;
      if (!(optParametric = opt_Parametric::findObjectOptimizer(optObjectName, curve)))
      {
        if (optParametric = opt_Parametric::newObjectOptimizer(optObjectName, curve))
        {
          optParametric->setOptMethodMinimize();
          optParametric->setObjectiveFunction([curve](const opt_Parametric* p) {
            return curve->getLength();
          });

          optParametric->setPenalizeFunction([curve](const opt_Parametric* p) {
            return 1;
          });

        }
      }
      if (optParametric)
      {
        vis_Widget* vis = nullptr;
        w_PropertyHolderDialog curveShowDlg;
        draw_Curve* last_curve = nullptr;

        optParametric->setSolveFunction([&](const opt_Parametric *p) {
          if (!vis)
          {
            curveShowDlg.setWindowFlag(Qt::WindowStaysOnTopHint);
            curveShowDlg.setModal(false);
            curveShowDlg.setWindowTitle("Nurb show");

            if (w_PropertyHolderWidget* holder = curveShowDlg.getHolder(0, 0, 1, 1))
            {
              if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetDraw", holder))
              {
                vis = w;
                w->setSizeHint(QSize(800, 700));
                w->setKeepAspect();
                w->displayCurve(*curve, "example nurb curve");
                holder->placeWidget(w);
              }
              curveShowDlg.show();
            }
          }

          if (last_curve)
          {
            last_curve->setColor("green").setLineWidth(1);
          }
          else
          {
            QVector < QVector<Double2>> points = { { startP, endP} };

            auto L = vis->displayCurves(points);
            if (L.size())
            {
              if (auto draw = static_cast<draw_Curve*>(L.first()))
              {
                draw->setColor("blue").setLineWidth(2);
              }
            }
            //curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(TObject::new_object("curve_Nurbs", "orginal curve"));
            //n->setControlPoints(allControlPoints);
            //auto ret=vis->displayCurve(*n);
            //if (ret)
            //{
            //  auto draw = static_cast<draw_Curve*>(ret);
            //  if (draw)
            //  {
            //    draw->setColor("blue").setLineWidth(2);
            //  }
            //}
          }

          if (auto curve_draw = static_cast<draw_Curve*>(vis->displayCurve(*curve, "sample nurbs curve")))
          {
            curve_draw->setColor("red").setLineWidth(2);
            last_curve = curve_draw;
          }
          return 0;
        });

        auto ret = w_ParametricDlg::optimizeBase(optParametric, name);
      }
    }
  }
}
O_METHOD_END;
