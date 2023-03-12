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

#include "exp_PlotExampleObject.h"
#include "w_PropertyHolderDialog.h"

REGISTER_OBJECT_CLASS(exp_PlotExampleObject, "plotExampleObject", TObject);

exp_PlotExampleObject::exp_PlotExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  setTag("MethodOrder", "drawPlotExample,$,drawCharExample,$,drawIllustration,drawTest,$,MultiCurveExample,CurveEditorExample,$,BladeEditorExample,BladeEditorExampleAirfoil,BladeEditorExampleBetaThickness,BladeEditorExamplePS,$");    
}

#include "draw_XYInteractiveEditorDialog.h"
#include "draw_XYInteractiveEditorWidget.h"
#include "util_CurveNurbs.h"
O_METHOD_BEGIN(exp_PlotExampleObject, CurveEditorExample, "Editing a Nurbs Curve", 0, 0, "CurveEditorExample")
{
  if (VARLID_ARG)
  {
    TObject* o = object->object("curve");
    if (!o)
    {
      o = TObject::new_object("TNurbsCurve", "curve", object);
      if (TNurbsCurve* curve = dynamic_cast<TNurbsCurve*>(o))
      {
        curve->setDimension(2);
        int nc = 4;
        QVector<double> x = QVector<double>()
          << 0 << 0
          << 0 << 1
          << 1 << 1
          << 1 << 0;

        curve->setInputData(nc, &x[0], 200);
        //curve->lockAspectRatio = true;
        curve->xUnit->setUnitCat(TUnit::length);
        curve->yUnit->setUnitCat(TUnit::length);
        curve->setXAxisTitle("Length");
        curve->setYAxisTitle("Angle");
      }
    }

    if (TNurbsCurve* curve = dynamic_cast<TNurbsCurve*>(o))
    {
      draw_XYInteractiveEditorDialog dlg;
      dlg.getEditor()->setControlCurve(curve);
      dlg.getEditor()->enableEndEditable(curve, false, "");
      dlg.getEditor()->enableEndEditable(curve, true, "x");


      if (dlg.exec() == QDialog::Accepted)
      {

      }
    }


    return true;
  }
}
O_METHOD_END;

#include "blade_ConfigDialog.h"
#include "blade_ConfigWidget.h"
#include "blade_Define.h"
#include "blade_NurbsCurve.h"
static blade_Define* getBlade(const QString& name, TObject* parent)
{
  if (blade_Define* b = blade_Define::newBladeDefine(blade_Define::DataTypeCamberThickness, name, "", parent))
  {
    // generate 3 layers in span with different height and length
    QVector<double> spans = QVector<double>() << 0 << 50 << 100;
    QVector<double> heights = QVector<double>() << 0.8 << 1.0 << 1.4;
    QVector<double> lengths = QVector<double>() << 0.7 << 1.0 << 1.3;
    QVector<double> z0 = QVector<double>() << 0 << 0 << 0;
    QVector<double> z1 = QVector<double>() << 1.3 << 1 << 0.7;
    QVector<double> r0 = QVector<double>() << 12 << 10 << 8;

    for (int i = 0; i < spans.size(); i++)
    {


      if (blade_NurbsCurve* camber = b->getCamberline(spans[i]))
      {
        QVector<Double2> x;
        x.push_back(Double2(0, 0 * heights[i]));
        x.push_back(Double2(0.2, 1 * heights[i]));
        x.push_back(Double2(0.6, 1 * heights[i]));
        x.push_back(Double2(1.0, 0 * heights[i]));
        double chordLength = 5 * lengths[i];
        camber->setChordLength(chordLength);
        camber->setControlPoints(x);
        camber->setLER(r0[i]);
        camber->setTER(r0[i] - chordLength);
        camber->setLEZ(z0[i]);
        camber->setTEZ(z1[i]);
      }

      // RZ
      if (blade_NurbsCurve* camber = b->getCamberline(spans[i]))
        if (blade_NurbsCurve* flowPath = b->getZRCurve(spans[i]))
        {
          QVector<Double2> x;
          x.push_back(Double2(camber->getLEZ(), camber->getLER()));
          x.push_back(Double2(camber->getTEZ(), camber->getTER()));
          flowPath->setControlPoints(x);
        }

      if (blade_NurbsCurve* thick = b->getThicknessCurve(spans[i]))
      {
        QVector<Double2> x;
        x.push_back(Double2(0, 0.5));
        x.push_back(Double2(0.2, 0.4));
        x.push_back(Double2(0.6, 0.2));
        x.push_back(Double2(1.0, 0.15));
        thick->setControlPoints(x);
      }
    }

    return b;
  }

  return 0;
}

O_METHOD_BEGIN(exp_PlotExampleObject, BladeEditorExample, "Blade Profile Editor", 0, 0, "BladeEditorExample")
{
  if (VARLID_ARG)
  {
    QString name = "bladeDefine";
    TObject* o = object->object(name);
    if (!o)
    {
      o = getBlade(name, object);
    }

    if (blade_Define* b = dynamic_cast<blade_Define*>(o))
    {
      blade_ConfigDialog dlg;
      //dlg.setWindowTitle("BladeEditorExample");


      // add some additional action
      QVector<w_QAction*> actions;

      w_QAction* a = new w_QAction(QPixmap(":images/plot.png"), QObject::tr("My added action"), &dlg);
      QObject::connect(a, &w_QAction::triggered, a, [=]() {printMessage("My added action clicked!"); });
      actions.push_back(a);

      a = new w_QAction(QPixmap(":images/plot.png"), QObject::tr("Another added action"), &dlg);
      QObject::connect(a, &w_QAction::triggered, a, [=]() {printMessage("Another added action clicked!"); });
      actions.push_back(a);

      dlg.getWidget()->setAdditionalPopupActions(actions);

      dlg.setBlade(b);

      if (dlg.exec() == QDialog::Accepted)
      {
        double r = b->getRLE();
        r = b->getRTE();

        int iii = 0;

      }
    }

    return true;
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_PlotExampleObject, BladeEditorExampleAirfoil, "Blade Airfoil Editor", 0, 0, "BladeEditorExampleAirfoil")
{
  if (VARLID_ARG)
  {
    QString name = "bladeDefineAirfoil";
    TObject* o = object->object(name);
    if (!o)
    {
      // create a airfoil style from a MeanlineThickness blade
      blade_Define* blade = getBlade(name + "MeanlineThickness", object);

      if (blade_Define* b = blade_Define::newBladeDefine(blade_Define::DataTypeAirFoil, name, "", object))
      {
        o = b;
        b->setVariableGeometry(false);

        QVector<double> spans = blade->getAllSpanPercents();
        QVector<int> sections = { blade_Define::SectionCamber, blade_Define::SectionPS, blade_Define::SectionSS, blade_Define::SectionLE, blade_Define::SectionTE };
        QVector<int> curves = { blade_Define::CurveCamber, blade_Define::CurvePS, blade_Define::CurveSS, blade_Define::CurveLE, blade_Define::CurveTE };
        for (int i = 0; i < spans.size(); i++)
        {
          if (blade_NurbsCurve* camber = blade->getDefineCurve(blade_Define::CurveCamber, spans[i], false))
          {
            double chordLength = camber->getChordLength();
            chordLength = MAX(chordLength, 1.E-10);
            for (int j = 0; j < sections.size(); j++)
            {
              QVector<Double2> L = blade->getBladeSection(spans[i], sections[j]);
              for (int k = 0; k < L.size(); k++)
                L[k][0] /= chordLength;
              if (blade_NurbsCurve* c = b->getDefineCurve(curves[j], spans[i]))
              {
                c->setOutputPoints(L);
              }
            }

            // RZ
            if (blade_NurbsCurve* c0 = blade->getDefineCurve(blade_Define::CurveZR, spans[i], false))
              if (blade_NurbsCurve* c1 = b->getDefineCurve(blade_Define::CurveZR, spans[i], true))
                TObject::copyObject(c1, c0, true);

            // Camber other properties
            if (blade_NurbsCurve* c0 = blade->getDefineCurve(blade_Define::CurveCamber, spans[i], false))
              if (blade_NurbsCurve* c1 = b->getDefineCurve(blade_Define::CurveCamber, spans[i], true))
              {
                c1->setChordLength(c0->getChordLength());
                c1->setLER(c0->getLER());
                c1->setTER(c0->getTER());
                c1->setLEZ(c0->getLEZ());
                c1->setLEZ(c0->getLEZ());
              }
          }
        }
      }

      TObject::delete_object(blade);
    }

    if (blade_Define* b = dynamic_cast<blade_Define*>(o))
    {
      blade_ConfigDialog dlg;
      //dlg.setWindowTitle("BladeEditorExample");
      dlg.setBlade(b);
      if (dlg.exec() == QDialog::Accepted)
      {
        double r = b->getRLE();
        r = b->getRTE();

        int iii = 0;

      }
    }

    return true;
  }
}
O_METHOD_END;
#if 1
O_METHOD_BEGIN(exp_PlotExampleObject, BladeEditorExampleBetaThickness, "Blade Beta Thickness Editor", 0, 0, "BladeEditorExampleBetaThickness")
{
  if (VARLID_ARG)
  {
    QString name = "bladeDefineBetaThickness";
    TObject* o = object->object(name);
    if (!o)
    {
      // create a airfoil style from a MeanlineThickness blade
      blade_Define* blade = getBlade(name + "MeanlineThickness", object);

      if (blade_Define* b = blade_Define::newBladeDefine(blade_Define::DataTypeBetaThickness, name, "", object))
      {
        o = b;
        b->setVariableGeometry(false);

        QVector<double> spans = blade->getAllSpanPercents();
        QVector<int> sections = { blade_Define::SectionCamber, blade_Define::SectionPS, blade_Define::SectionSS, blade_Define::SectionLE, blade_Define::SectionTE };
        QVector<int> curves = { blade_Define::CurveCamber, blade_Define::CurvePS, blade_Define::CurveSS, blade_Define::CurveLE, blade_Define::CurveTE };
        for (int i = 0; i < spans.size(); i++)
        {
          // RZ
          if (blade_NurbsCurve* c0 = blade->getDefineCurve(blade_Define::CurveZR, spans[i], false))
            if (blade_NurbsCurve* c1 = b->getDefineCurve(blade_Define::CurveZR, spans[i], true))
              TObject::copyObject(c1, c0, true);

          // Thickness
          if (blade_NurbsCurve* c0 = blade->getDefineCurve(blade_Define::CurveThickness, spans[i], false))
            if (blade_NurbsCurve* c1 = b->getDefineCurve(blade_Define::CurveThickness, spans[i], true))
              TObject::copyObject(c1, c0, true);

          // Beta
          if (blade_NurbsCurve* beta = b->getBetaCurve(spans[i]))
          {
            double start = -50 / 180.0 * PI;
            double end = -35 / 180.0 * PI;
            double d = end - start;
            QVector<Double2> x;
            x.push_back(Double2(0.));
            x.push_back(Double2(0.2));
            x.push_back(Double2(0.6));
            x.push_back(Double2(1.0));
            for (int j = 0; j < x.size(); j++)
              x[j][1] = start + d * x[j][0];

            beta->setControlPoints(x);
          }

          if (blade_NurbsCurve* c = b->getCamberline(spans[i], true))
          {
            if (blade_NurbsCurve* camber = blade->getCamberline(spans[i], false))
            {
              c->setChordLength(camber->getChordLength());
              c->setLER(camber->getLER());
              c->setTER(camber->getTER());
              c->setLEZ(camber->getLEZ());
              c->setTEZ(camber->getTEZ());
            }
          }
        }
      }

      TObject::delete_object(blade);
    }

    if (blade_Define* b = dynamic_cast<blade_Define*>(o))
    {
      blade_ConfigDialog dlg;
      //dlg.setWindowTitle("BladeEditorExample");
      dlg.setBlade(b);

      QVector<double> spans = b->getAllSpanPercents();
      for (int i = 0; i < spans.size(); i++)
      {
        dlg.getWidget()->updateCamberFromBeta(spans[i]);
      }
      if (dlg.exec() == QDialog::Accepted)
      {
        double r = b->getRLE();
        r = b->getRTE();

        int iii = 0;

      }
    }

    return true;
  }
}
O_METHOD_END;
#endif

#include "blade_DefinePressureSuction.h"
O_METHOD_BEGIN(exp_PlotExampleObject, BladeEditorExamplePS, "Blade PS Editor", 0, 0, "BladeEditorExamplePS")
{
  if (VARLID_ARG)
  {
    QString name = "bladeDefinePS";
    TObject* o = object->object(name);
    if (!o)
    {
      if (blade_Define* bladeDef = blade_Define::newBladeDefine(blade_Define::DataTypePressureSuction, name, "", object))
      {
        blade_DefinePressureSuction *b = dynamic_cast<blade_DefinePressureSuction*>(bladeDef);

        o = b;

        b->setVariableGeometry(false);
        //b->setAxR(blade_Define::AXAxial);

        // generate 3 layers in span with different height and length
        QVector<double> spans = QVector<double>() << 0 << 50 << 100;
        QVector<double> heights = QVector<double>() << 0.8 << 1.0 << 1.4;
        QVector<double> z = QVector<double>() << 0 << 1 << 2;

        for (int i = 0; i < spans.size(); i++)
        {
          if (blade_NurbsCurve* flowPath = b->getZRCurve(spans[i]))
          {
            QVector<Double2> x;
            x.push_back(Double2(z[i], 20));
            x.push_back(Double2(z[i], 0.2));
            flowPath->setControlPoints(x);
          }

          if (blade_NurbsCurve* camber = b->getCamberline(spans[i]))
          {
            QVector<Double2> x;
            x.push_back(Double2(0, 0 * heights[i]));
            x.push_back(Double2(0.2, 0.3 * heights[i]));
            x.push_back(Double2(0.6, 0.3 * heights[i]));
            x.push_back(Double2(1.0, 0 * heights[i]));
            double chordLength = 3;
            camber->setChordLength(chordLength);
            camber->setControlPoints(x);
            camber->setLECurveR(chordLength / 20);
            camber->setTECurveR(chordLength / 40);
            camber->setHalfWedgeAngle_exit(15);
          }

          if (blade_NurbsCurve* PS = b->getPSCurve(spans[i]))
          {
            PS->setNumIntermediatePnts(4);
            PS->setStretchingFactor(1.1);
            QVector<double> tPS = QVector<double>() << 0.1 << 0.1 << 0.1 << 0.1;
            PS->setCtrlPnt_Offsetfractions(tPS);
            PS->calculateSideCurve_ps_ssMode(false);

          }

          if (blade_NurbsCurve* SS = b->getSSCurve(spans[i]))
          {
            SS->setNumIntermediatePnts(4);
            SS->setStretchingFactor(1.1);
            QVector<double> tPS = QVector<double>() << 0.1 << 0.1 << 0.1 << 0.1;
            SS->setCtrlPnt_Offsetfractions(tPS);
            SS->calculateSideCurve_ps_ssMode(true);

          }

          if (blade_NurbsCurve* TE = b->getTECurve(spans[i]))
          {
            TE->calculateTECurve_ps_ssMode();
          }
        }
      }
    }

    if (blade_Define* b = dynamic_cast<blade_Define*>(o))
    {
      blade_ConfigDialog dlg;
      //dlg.setWindowTitle("BladeEditorExample");
      dlg.setBlade(b);
      if (dlg.exec() == QDialog::Accepted)
      {
        double r = b->getRLE();
        r = b->getRTE();

        int iii = 0;

      }
    }

    return true;
  }
}
O_METHOD_END;


#include "vis_Widget.h"
#include "curve_Curve.h"
#include "curve_Line.h"
#include "curve_Polygon.h"
#include "curve_Circle.h"
#include "curve_Spline.h"
#include "curve_Nurbs.h"

O_METHOD_BEGIN(exp_PlotExampleObject, MultiCurveExample, "Multi-curve Example", 0, 0, "")
{
  if (VARLID_ARG)
  {
    curve_Curve curve;
    if (curve_Line* c = dynamic_cast<curve_Line*>(curve.addSegment(curve_Curve::Line)))
    {
      c->setPoints(Double2(0, 0), Double2(2, 0));
    }

    if (curve_Circle* c = dynamic_cast<curve_Circle*>(curve.addSegment(curve_Curve::Circle)))
    {
      c->setStartAngle(PI);
      c->setEndAngle(PI / 2);
      c->setRadius(1);
      c->setCenter(Double2(3, 0));
    }

    if (curve_Spline* c = dynamic_cast<curve_Spline*>(curve.addSegment(curve_Curve::Spline)))
    {
      QVector<Double2> points = {
        Double2(3, 1),
        Double2(3.5, 0.6),
        Double2(3.2, -0.5),
        Double2(4.5, -1)
      };
      c->setControlPoints(points);
    }

    if (curve_Nurbs* c = dynamic_cast<curve_Nurbs*>(curve.addSegment(curve_Curve::Nurbs)))
    {
      QVector<Double2> points = {

        Double2(4.5, -1),
        Double2(4.5, -2),
        Double2(3.6, -2.1),
        Double2(2.8, -3)
      };
      c->setControlPoints(points);
    }

    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("MultiCurveExample");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetDraw", holder))
      {
        w->setSizeHint(QSize(800, 600));
        w->setKeepAspect();
        w->displayCurve(curve, "Example curve");
        holder->placeWidget(w);
      }
      if (dlg.exec() == QDialog::Accepted)
      {
      }
    }
  }
}
O_METHOD_END;


static QVector<QVector<Double2> > generateRandomData(int listCount, int valueMax, int valueCount)
{
  QVector<QVector<Double2> > L;

  // generate random data
  for (int i(0); i < listCount; i++) {
    QVector<Double2> LL;

    double yValue = 0;
    for (int j(0); j < valueCount; j++)
    {
      yValue += QRandomGenerator::global()->bounded(valueMax / (qreal)valueCount);
      double xValue = (j + QRandomGenerator::global()->generateDouble()) * ((qreal)valueMax / (qreal)valueCount);
      //double xValue = j;

      LL << Double2(xValue, yValue);
      //eprintf("%d: %g %g", i, xValue, yValue);
    }
    L << LL;
  }

  return L;
}

#include "chart_MultiWidget.h"
O_METHOD_BEGIN(exp_PlotExampleObject, drawCharExample, "Charting Example", 0, FLG_METHOD_SKIP_TUI, "drawCharExample")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("drawCharExample");

    chart_MultiWidget* plot = 0;
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      holder->setSizeHint(QSize(1920, 768));
      if (chart_MultiWidget* w = new chart_MultiWidget(holder))
      {
        plot = w;
        holder->placeWidget(w);
      }
    }

    if (plot)
    {
      QVector<QVector<Double2> > L = generateRandomData(4, 10, 7);

#if 1
      if (chart_View* p = plot->getPlot(0, 0))
      {
        p->setTitle("Line test");

        int lineStyleMin = Qt::SolidLine;
        int lineStyleMax = Qt::DashDotDotLine;
        int lineSytle = lineStyleMin;
        for (int i = 0; i < L.size(); i++)
        {
          if (w_QAbstractSeries* pp = p->addLine(L[i], QString("Line %1").arg(i)))
          {
            QPen pen = p->pen(pp);
            pen.setWidth(2);
            pen.setStyle(Qt::PenStyle(lineSytle++));
            p->setPen(pp, pen);
            if (lineSytle > lineStyleMax)
              lineSytle = lineStyleMin;
          }
        }

        p->setXTitle("x axis");
        p->setYTitle("y axis");
        p->resizeWithRelativeMargin(0, true);
        p->showLegend();

        Double2 point = L[0][L[0].size() / 2];
        p->addCallOut("Some callout", point, point + Double2(0.2, 1));

        point = L[2][L[2].size() / 2];
        p->addCallOut("Another callout", point, point + Double2(0.2, -0.6));
      }

#endif

#if 1
      if (chart_View* p = plot->getPlot(0, 1))
      {
        p->setTitle("Spline test");

        for (int i = 0; i < L.size(); i++)
        {
          if (i == 1)
          {
            for (int j = 0; j < L[i].size(); j++)
              L[i][j][1] *= 1.5;
          }
          if (w_QAbstractSeries* pp = p->addSpline(L[i], QString("Line %1").arg(i)))
          {
            if (i == 1)
              p->setRightYAxis(pp, true);
          }
        }
        p->setXTitle("x axis");
        p->setYTitle("y axis");
        p->setYRTitle("y right axis");
        p->resizeWithRelativeMargin();
        p->showLegend();

        if (QValueAxis* a = p->getAxisX())
        {
          a->setGridLineColor("red");
          a->setMinorTickCount(4);
          a->setMinorGridLineVisible(true);
        }
      }
#endif

#if 1
      if (chart_View* p = plot->getPlot(1, 0))
      {
        p->setTitle("Scatter test");

        for (int i = 0; i < L.size(); i++)
        {
          if (w_QAbstractSeries* pp = p->addScatter(L[i], QString("Line %1").arg(i)))
          {
          }
        }
        p->setXTitle("x axis");
        p->setYTitle("y axis");
        p->resizeWithRelativeMargin();
        p->showLegend();
      }
#endif
#if 1
      if (chart_View* p = plot->getPlot(1, 1))
      {
        p->setTitle("Area test");

        for (int i = 0; i < L.size(); i++)
        {
          QVector<Double2> x0;
          if (i > 0)
            x0 = L[i - 1];
          if (w_QAbstractSeries* pp = p->addAreaChart(x0, L[i], QString("Area %1").arg(i)))
          {
          }
        }
        p->setXTitle("x axis");
        p->setYTitle("y axis");
        p->resizeWithRelativeMargin();
        p->showLegend();
      }

#endif

      if (chart_View* p = plot->getPlot(0, 2))
      {
        p->setTitle("Stacked bar test");
        QStringList names = { "Jane", "John", "Axel", "Mary", "Sam" };
        QStringList categoryNames = QStringList() << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";

        QVector<QVector<double> > points(names.size());
        points[0] << 1 << 2 << 3 << 4 << 5 << 6;
        points[1] << 5 << 0 << 0 << 4 << 0 << 7;
        points[2] << 3 << 5 << 8 << 13 << 8 << 5;
        points[3] << 5 << 6 << 7 << 3 << 4 << 5;
        points[4] << 9 << 7 << 5 << 3 << 1 << 2;

        if (w_QAbstractSeries* pp = p->addBarChart(points, names, categoryNames, true))
        {

        }

        p->setXTitle("Month");
        p->setYTitle("Value");
        p->resizeWithRelativeMargin(0);
        p->showLegend();
      }

      if (chart_View* p = plot->getPlot(1, 2))
      {
        p->setTitle("Pie test");

        QStringList names = { "Jane", "John", "Axel", "Mary", "Sam" };
        QVector<double> x = QVector<double>() << 3 << 5 << 8 << 13 << 8;

        if (w_QAbstractSeries* pp = p->addPieChart(x, names))
        {

        }

        p->resizeWithRelativeMargin(0);
        p->showLegend();
      }

      if (chart_View* p = plot->getPlot(2, 0, true))
      {
        p->setTitle("Polor Chart Test");

        QStringList names;
        QVector<QVector<Double2>> data;
        QVector<Double2> xy;
        QVector<double> x = QVector<double>() << 0 << 60 << 120 << 180;
        QVector<double> y = QVector<double>() << 0 << 0.10 << 0.18 << 0.26;
        forLoop(i, x.size())
        {
          xy.push_back(Double2(x[i], y[i]));
        }
        data.push_back(xy);
        names.push_back("Line series1");

        /*xy.clear();
        x = QVector<double>() << 0 << 45 << 90 << 135;
        y = QVector<double>() << 0 << 0.02 << 0.05 << 0.06;
        forLoop(i, x.size())
        {
          xy.push_back(Double2(x[i], y[i]));
        }
        data.push_back(xy);
        names.push_back("Line series2");*/

        QVector<w_QAbstractSeries*> pp = p->addPolorChart(data, names);

        p->resizeWithRelativeMargin(0);
        p->showLegend();
      }

      // apply the default settings
      //plot->getPlot()->applySettings();
    }

    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;

#include "plt_PolarInteractive.h"

O_METHOD_BEGIN(exp_PlotExampleObject, drawPolorCoordinate, "Polor Coordinate Example", 0, FLG_METHOD_SKIP_TUI, "drawPolorCoordinate")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("Polor Coordinate Example");
    PolarPlotInteractive* plot = new PolarPlotInteractive(&dlg);
    plot->clear();
    plot->setTitle("Polor Test");


    QVector<Double2> points = QVector<Double2>() << Double2(0, 0) << Double2(60, 0.5) << Double2(120, 1) << Double2(180, 1.5);
    plot->addCurve(points, "Line 1");

    points = QVector<Double2>() << Double2(0, 0.5) << Double2(45, 0.7) << Double2(90, 1.5) << Double2(135, 2) << Double2(270, 3);
    plot->addCurve(points, "Line 2");
    
    plot->rescale();
    plot->replot();


    dlg.resize(600, 600);
    dlg.placeWidget(plot);

    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;

#include "draw_XYDialog.h"
#include "core_Application.h"
O_METHOD_BEGIN(exp_PlotExampleObject, drawTest, "Draw Items Example", 0, FLG_METHOD_SKIP_TUI, "drawTest")
{
  ARG_CACHE_OFF;

  if (VARLID_ARG)
  {
    draw_XYDialog dlg(core_Application::core());
    if (draw_XYInteractive* plot = dlg.getPlot())
    {
#if 0
      {
        QVector<w_QAction*> actions;
        w_QAction* a = new w_QAction(QPixmap(":images/export-csv.png"), QObject::tr("Apply"), plot);
        QObject::connect(a, &w_QAction::triggered, plot,
          [=]() { eprintf("Apply clicked!"); });
        actions.push_back(a);
        plot->setAdditionalRightClickActions(actions);
      }
#endif
      plot->setScaleDistance(0.6);
      plot->setArrowSize(0.25);
      plot->setEndSymboSize(8);
      plot->setKeepAspect(true);

      QVector<Double2> x;
      x.push_back(Double2(0.5, 1.0));
      x.push_back(Double2(0.0, 0.0));
      x.push_back(Double2(1.0, 1.0));
      x.push_back(Double2(2.0, 3.0));
      x.push_back(Double2(3.0, 4.5));

      if (draw_TPlotCurve* e = plot->newPlotCurve(x, "testCurve"))
      {
        e->setSymbolStyle(QwtSymbol::Ellipse)
          .setSymbolSize(4)
          .setLineType(Qt::DotLine)
          .setLineWidth(2)
          .setStartShape(draw_PlotInteractiveStyle::ArrowFilled);
        //e->detach();
        //e->hide();
        //e->attach(plot);

        if (draw_CurveScale* ee = plot->newScale(x,
          "scale",
          draw_PlotInteractiveStyle::ScaleTypeAlong,
          draw_PlotInteractiveStyle::ScalePositionRight,
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::Ellipse,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          ee->setLineType(Qt::DotLine)
            .setLineWidth(2);
          //ee->setScaleTextPrefix("Length = ");

          ee->hide();
          ee->show();
        }

        if (draw_CurveScale* ee = plot->newScale(x,
          "scale",
          draw_PlotInteractiveStyle::ScaleTypeX,
          draw_PlotInteractiveStyle::ScalePositionRight,
          draw_PlotInteractiveStyle::ArrowLocationOut,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          ee->setScaleTextPostfix(" (error = 0.01)");
          if (ee->getScaleText())
          {
            ee->getScaleText()->setFontSize(6);
            ee->getScaleText()->setColor("red");
          }
        }

        if (draw_CurveScale* ee = plot->newScale(x,
          "scale",
          draw_PlotInteractiveStyle::ScaleTypeY,
          draw_PlotInteractiveStyle::ScalePositionRight,
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Ellipse,
          draw_PlotInteractiveStyle::Left,
          1.0))
        {
          ee->setScaleText("User text");
        }
      }

      QVector<Double2> xx;
      xx.push_back(x.last());
      xx.push_back(Double2(5, 5));
      xx.push_back(Double2(6, 8));
      xx.push_back(Double2(7, 8));
      for (int i = 0; i < 5; i++)
      {
        if (draw_CurveText* e = plot->newText(
          "Some information here",
          xx,
          "text"))
        {

        }
        for (int j = 0; j < xx.size(); j++)
          xx[j][1] -= 1;
      }

      Double2 center(0, 5.5);
      Double2 start(1, 5.5);
      Double2 end(0.8, 6.5);
      QVector<Double2> L;
      L.push_back(start);
      L.push_back(center);
      L.push_back(end);
      if (draw_TPlotCurve* e = plot->newPlotCurve(L, "testAngleCurve"))
      {
        draw_CurveScaleAngle* firstAngleScale = 0;

        e->setLineWidth(4);
        e->setColor("red");
        if (draw_CurveScaleAngle* ee = plot->newAngleScale(center, start, end,
          "angleScale",
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          ee->setLineType(Qt::DotLine)
            .setLineWidth(2);
          ee->getScaleText()->setFontSize(12);
          //ee->setScaleTextPrefix("Length = ");
          firstAngleScale = ee;
        }

        if (draw_CurveScaleAngle* ee = plot->newAngleScale(center, start, end,
          "angleScale",
          draw_PlotInteractiveStyle::ArrowLocationOut,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Left,
          2.0))
        {
          ee->setLineType(Qt::DotLine);
          //ee->setScaleTextPrefix("Length = ");
          //ee->detach();
        }

        if (draw_CurveScaleAngle* ee = plot->newAngleScale(center, start, end,
          "angleScale",
          draw_PlotInteractiveStyle::ArrowLocationOut,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Right,
          3.0))
        {
          ee->setLineWidth(2);
          //ee->setScaleTextPrefix("Length = ");
        }

        // you can also attach another line to a scale
        if (firstAngleScale)
        {
          xx.clear();

          xx.push_back(firstAngleScale->getMiddlePoint());
          xx.push_back(xx.last() + Double2(2, 2));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* e = plot->newText(
            firstAngleScale->takeScaleText(),
            xx,
            "text"))
          {
            firstAngleScale->addChild(e->getName());
            //firstAngleScale->detach(); // one can detach these items
            //firstAngleScale->attach(plot);
          }
        }
      }

      plot->zoomAll();
    }

    if (dlg.exec())
    {
    }

    return true;
  }
}
O_METHOD_END;


O_METHOD_BEGIN(exp_PlotExampleObject, drawIllustration, "Draw Items, Scales, and Illustrations", 0, FLG_METHOD_SKIP_TUI, "drawIllustration")
{
  ARG_CACHE_OFF;

  if (VARLID_ARG)
  {
    draw_XYDialog dlg(core_Application::core());
    if (draw_XYInteractive* plot = dlg.getPlot())
    {
      plot->setScaleDistance(0.6);
      plot->getParameter("scaleExtensionSkipDistance").setValue(0.15);
      plot->setArrowSize(0.25);
      plot->setEndSymboSize(8);
      plot->setKeepAspect(true);

      QVector<Double2> shape1 = QVector<Double2>()
        << Double2(0, 0)
        << Double2(5, 5)
        << Double2(4, 6)
        << Double2(-1, 1)
        << Double2(0, 0);

      if (draw_CurveShape* e = plot->newFilledCurve(shape1, "shape1"))
      {
        e->setLineWidth(2);
        e->setFillBackgroundColor("lightGreen");

        QVector<Double2> x = QVector<Double2>()
          << shape1[1]
          << shape1[0];

        if (draw_CurveScale* ee = plot->newScale(x,
          "scale",
          draw_PlotInteractiveStyle::ScaleTypeY,
          draw_PlotInteractiveStyle::ScalePositionRight,
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Ellipse,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          //ee->setScaleText("User text");
        }

        x = QVector<Double2>()
          << shape1[3]
          << shape1[2];

        if (draw_CurveScale* ee = plot->newScale(x,
          "scale",
          draw_PlotInteractiveStyle::ScaleTypeAlong,
          draw_PlotInteractiveStyle::ScalePositionLeft,
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {

          QVector<Double2> xx;
          xx.push_back(shape1[1] + Double2(0.07, 0));
          xx.push_back(xx.last() + Double2(2, 2.5));
          xx.push_back(xx.last() + Double2(1, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            "scaleExtensionSkipDistance",
            xx,
            "text"))
          {
          }

          xx.clear();
          xx.push_back(shape1[1] + Double2(0.3, 0));
          xx.push_back(xx.last() + Double2(2, 1.8));
          xx.push_back(xx.last() + Double2(1, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            "scaleDistance",
            xx,
            "text"))
          {
          }

          xx.clear();
          xx.push_back(shape1[1] + Double2(0.7, 0));
          xx.push_back(xx.last() + Double2(2, 1.3));
          xx.push_back(xx.last() + Double2(1, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            "scaleExtension",
            xx,
            "text"))
          {
          }

          xx.clear();
          xx.push_back(shape1[1] + Double2(0.6, -0.125));
          xx.push_back(xx.last() + Double2(2, 0.7));
          xx.push_back(xx.last() + Double2(1, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            "arrowLength",
            xx,
            "text"))
          {
          }

          xx.clear();
          xx.push_back(shape1[1] + Double2(0.6, -0.2));
          xx.push_back(xx.last() + Double2(2, 0.1));
          xx.push_back(xx.last() + Double2(1, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            "arrowAngle",
            xx,
            "text"))
          {
          }

        }
      }

      QVector<Double2> shape2 = QVector<Double2>()
        << Double2(0, 8)
        << Double2(4, 8)
        << Double2(4, 8.5)
        << Double2(1, 8.5)
        << Double2(3, 12.5)
        << Double2(2.5, 12.5)
        << Double2(0, 8);

      if (draw_CurveShape* e = plot->newFilledCurve(shape2, "shape2"))
      {
        e->setLineWidth(3);
        e->setFilled(true, "lightYellow");
        e->setFillPatten(Qt::NoBrush);

        if (draw_CurveScaleAngle* ee = plot->newAngleScale(shape2[0], shape2[1], shape2[5],
          "angleScale",
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          //ee->setLineType(Qt::DotLine)
          //ee->setLineWidth(2);
          //ee->getScaleText()->setFontSize(12);
        }


        if (draw_CurveScaleAngle* ee = plot->newAngleScale(shape2[3],
          shape2[3] + (shape2[2] - shape2[3])*0.2,
          shape2[3] + (shape2[4] - shape2[3])*0.15,
          "angleScale",
          draw_PlotInteractiveStyle::ArrowLocationIn,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::ArrowFilled,
          draw_PlotInteractiveStyle::Top,
          1.0))
        {
          ee->setLineType(Qt::DotLine);
          ee->setLineWidth(2);

          QVector<Double2> xx;
          xx.push_back(ee->getMiddlePoint());
          xx.push_back(xx.last() + Double2(0.5, 0.5));
          xx.push_back(xx.last() + Double2(1, 0));
          if (draw_CurveText* eee = plot->newText(
            ee->takeScaleText(),
            xx,
            "text"))
          {
            ee->addChild(eee->getName());
          }
        }
      }

      plot->zoomAll();
    }

    if (dlg.exec())
    {
    }

    return true;
  }
}
O_METHOD_END;


static void overlapWidget(w_PropertyHolderWidget* holder, w_Property* w, QWidget* ref, int location)
{
  holder->placeWidgetOverlap(w, ref, Qt::AlignLeft | Qt::AlignTop);
}

#include "draw_XYMultiWidget.h"
#include "chart_Config.h"
O_METHOD_BEGIN(exp_PlotExampleObject, drawPlotExample, "Multi-Plot Window Example (default plot method)", 0, FLG_METHOD_SKIP_TUI, "drawPlotExample")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("drawPlotExample");

    bool choice = true;
    draw_XYMultiWidget* plot = 0;
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      holder->setSizeHint(QSize(1024, 768));
      if (draw_XYMultiWidget* w = new draw_XYMultiWidget(holder))
      {
        plot = w;
        holder->placeWidget(w);

        if (w_Property* ww = w_Property::getPropertyWidget(&choice))
        {
          holder->placeWidgetOverlap(ww, w, Qt::AlignLeft | Qt::AlignTop);
        }
      }
    }

    if (plot)
    {
      // generate the data
      QVector<QVector<Double2> > L = generateRandomData(4, 10, 7);

      // create the plots      
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
        {
          if (draw_XYInteractive* p = plot->getPlot(i, j))
          {
            int colorIndex = 1;
            for (int k = 0; k < L.size(); k++)
            {
              int symbol = (i == 0 && j == 1) ? QwtSymbol::NoSymbol : k;
              int lineType = (i == 1 && j == 0) ? 0 : k + 1;
#if 1
              if (draw_Curve* e = p->newPlotCurve(L[k], // convenience function
                QString("curve %1").arg(k))) // data as default SI unit
              {
                QString color = chart::_colors[(colorIndex++) % 12];
                e->setLineWidth(2)
                  .setLineType(lineType)
                  .setShowInLegend(true)
                  .setSymbolStyle(symbol)
                  .setColor(color)
                  ;
#else


              if (draw_TEdgePlot* e = p->addCurve(L[k], // convenience function
                QString("curve %1").arg(k),
                chart::_colors[(colorIndex++) % 12],
                2, lineType, symbol
              ))
              {
#endif
                if (symbol == QwtSymbol::NoSymbol || (i == j && i == 1))
                {
                  e->setLineFitted(true);
                }

                if (symbol != QwtSymbol::NoSymbol)
                {
                  e->setSymbolSize(12);
                }

                if (i == j && i == 1 && k == 0)
                {
                  QString text = "Some info text";
                  e->addText(text);
                }
              }
            }
            p->setXTitle("x axis");
            p->setYTitle("y axis");
            if (i == 0 && j == 0)
              p->setTitle("Line + Symbol");
            else if (i == 0 && j == 1)
              p->setTitle("Line fitted");
            else if (i == 1 && j == 0)
              p->setTitle("Symbol");
            else if (i == 1 && j == 1)
              p->setTitle("Line fitted + Symbol");

            p->zoomAll();

            p->applySettings();
          }
        }
    }

    if (dlg.exec() == QDialog::Accepted)
    {

    }

    return true;
  }
}
O_METHOD_END;
class Curve : public QwtPlotCurve
{
public:
  void setTransformation(const QTransform& transform)
  {
    d_transform = transform;
  }

  virtual void updateSamples(double phase)
  {
    setSamples(d_transform.map(points(phase)));
  }

private:
  virtual QPolygonF points(double phase) const = 0;

private:
  QTransform d_transform;
};

#include <qwt_curve_fitter.h>
#include <qwt_plot.h>
class Curve3 : public Curve
{
public:
  Curve3()
  {
    setStyle(QwtPlotCurve::Lines);
    setPen(Qt::red, 2);

    initSamples();

    // somewhere in the center
    //QTransform transform;
    //transform.translate(6.0, 3.0);
    //transform.scale(1.5, 1.5);

    //setTransformation(transform);
  }

private:
  virtual QPolygonF points(double phase) const
  {
    /*const double speed = 0.05;

    const double s = speed * qSin(phase);
    const double c = qSqrt(1.0 - s * s);

    for (int i = 0; i < d_points.size(); i++)
    {
      const QPointF p = d_points[i];

      const double u = p.x();
      const double v = p.y();

      d_points[i].setX(u * c - v * s);
      d_points[i].setY(v * c + u * s);
    }*/

    return d_points;
  }

  void initSamples()
  {

    QPointF p1(0, 0), p2(1, 0), p3(1, 1), p4(0, 1);
    d_points += p1;
    d_points += p2;
    d_points += p3;
    d_points += p4;
    /*const int numSamples = 4;

    for (int i = 0; i < numSamples; i++)
    {
      const double angle = i * (2.0 * M_PI / (numSamples - 1));

      QPointF p(qCos(angle), qSin(angle));
      if (i % 2)
        p *= 0.4;

      d_points += p;
    }*/
  }

private:
  mutable QPolygonF d_points;
};

O_METHOD_BEGIN(exp_PlotExampleObject, qwtPlotTest, "Test QwtPlot", 0, FLG_METHOD_SKIP_TUI, "Test QwtPlot")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("Test QwtPlot");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      if (QwtPlot* w = new QwtPlot(holder))
      {
        holder->placeWidget(w);
        Curve* c = new Curve3;
        const double speed = 2 * M_PI / 25000.0; // a cycle every 25 seconds
        c->setBrush(QBrush(QColor("yellow")));
        const double phase = 0.01;
        for (int i = 0; i < 1; i++)
          c->updateSamples(phase);

        c->attach(w);
        w->setAxisScale(2, -1, 3);
      }
    }
    
    dlg.exec();
  }
}
O_METHOD_END;
