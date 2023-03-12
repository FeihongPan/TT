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

#include "exp_VtkExampleObject.h"

REGISTER_OBJECT_CLASS(exp_VtkExampleObject, "vtkExampleObject", TObject);

exp_VtkExampleObject::exp_VtkExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;  

  setTag("MethodOrder", "WidgetExampleScatterGraph,WidgetExampleSurfaceGraph,VtkProfileWidgetExample,VtkSurfaceMeshWidgetExample,$");
}

#include "core_Application.h"
#include "w_PropertyHolderDialog.h"
#include "plt_SurfaceGraphWidget.h"
O_METHOD_BEGIN(exp_VtkExampleObject, WidgetExampleScatterGraph, "3D Scatter Graph", 0, 0, "WidgetExampleScatterGraph")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg(core_Application::core());
    dlg.setWindowTitle("WidgetExampleScatterGraph");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      QVector<QVector<double> > data;
      {
        double sampleMin = -8;
        double sampleMax = 8;
        double min[3] = { sampleMin, sampleMin, sampleMin };
        double max[3] = { sampleMax, sampleMax, sampleMax };
        int sampleCountX = 50;
        int sampleCountY = 50;
        float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        float stepY = (sampleMax - sampleMin) / float(sampleCountY - 1);

        for (int i = 0; i < sampleCountY; i++) {
          float y = MIN(sampleMax, (i * stepY + sampleMin));
          int index = 0;
          for (int j = 0; j < sampleCountX; j++)
          {
            QVector<double> point;
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float R = qSqrt(y * y + x * x) + 0.01f;
            float z = (qSin(R) / R + 0.24f) * 1.61f;
            point.push_back(x);
            point.push_back(y);
            point.push_back(z);
            data.push_back(point);
          }
        }
      }

      QStringList names = QStringList()
        << "Pressure"
        << "Static Pressure"
        << "Cp"
        ;

      QVector<int> unitCats = QVector<int>()
        << TUnit::pressure
        << TUnit::pressure
        << TUnit::specific_heat
        ;

      if (plt_SurfaceGraphWidget* surface = new plt_SurfaceGraphWidget(&dlg, 0, 0, plt_SurfaceGraphWidget::GT_Scatter))
      {
        surface->setData(data, names, unitCats,
          names[0], names[1], names[2]);

        // add another data
        for (int i = 0; i < data.size(); i++)
          data[i][2] /= 2;
        surface->addData(data);

        holder->placeWidget(surface, 1, 0, 1, 2);
      }
    }

    if (dlg.exec() == QDialog::Accepted)
    {
    }

    return true;
  }
}
O_METHOD_END;


#include "plt_SurfaceGraphWidget.h"
O_METHOD_BEGIN(exp_VtkExampleObject, WidgetExampleSurfaceGraph, "3D Surface Graph", 0, 0, "WidgetExampleSurfaceGraph")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg(core_Application::core());
    dlg.setWindowTitle("WidgetExampleSurfaceGraph");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      QVector<QVector<QVector<double> > > data;
      {
        double sampleMin = -8;
        double sampleMax = 8;
        double min[3] = { sampleMin, sampleMin, sampleMin };
        double max[3] = { sampleMax, sampleMax, sampleMax };
        int sampleCountX = 50;
        int sampleCountY = 50;
        float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        float stepY = (sampleMax - sampleMin) / float(sampleCountY - 1);

        for (int i = 0; i < sampleCountY; i++) {
          QVector<QVector<double> > row;
          float y = MIN(sampleMax, (i * stepY + sampleMin));
          int index = 0;
          for (int j = 0; j < sampleCountX; j++)
          {
            QVector<double> point;
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float R = qSqrt(y * y + x * x) + 0.01f;
            float z = (qSin(R) / R + 0.24f) * 1.61f;
            point.push_back(x);
            point.push_back(y);
            point.push_back(z);
            row.push_back(point);
          }
          data.push_back(row);
        }
      }

      QStringList names = QStringList()
        << "Pressure"
        << "Static Pressure"
        << "Cp"
        ;

      QVector<int> unitCats = QVector<int>()
        << TUnit::pressure
        << TUnit::pressure
        << TUnit::specific_heat
        ;

      if (plt_SurfaceGraphWidget* surface = new plt_SurfaceGraphWidget(&dlg, 0, 0))
      {
        surface->setData(data, names, unitCats,
          names[0], names[1], names[2]);

        holder->placeWidget(surface, 1, 0, 1, 2);
      }
    }

    if (dlg.exec() == QDialog::Accepted)
    {
    }

    return true;
  }
}
O_METHOD_END;

#include "curve_Circle.h"
#include "curve_Spline.h"
#include "curve_Nurbs.h"
#include "w_PropertyHolderDialog.h"
#include "vis_Widget.h"
O_METHOD_BEGIN(exp_VtkExampleObject, VtkProfileWidgetExample, "VTK Draw with Profile", 0, 0, "")
{
  if (VARLID_ARG)
  {
    curve_Curve curve;

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

    QVector<QVector<Double3>> profiles;
    QVector<QVector<double>> values;
    QVector<Double2> points = curve.toDouble2Vector();

    for (int n = 0; n < 10; n++)
    {
      QVector<Double3> L;
      QVector<double> V;
      for (int i = 0; i < points.size(); i++)
      {

        Double3 x = points[i];
        x[2] = n * 0.5;
        L.push_back(x);
        V.push_back(log((i + 1) * (n + 5.)));

        points[i] *= 0.9;
      }

      profiles.push_back(L);
      values.push_back(V);
    }

    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 1},
      //{"showMode", "Edge+Face"}
    };

    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("VtkProfileWidgetExample");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetVtk", holder))
      {
        w->setSizeHint(QSize(800, 600));
        w->displaySurfaceFromProfiles("Multi-layer profiles", profiles, &values, &args);
        holder->placeWidget(w);
      }
      if (dlg.exec() == QDialog::Accepted)
      {
      }
    }
  }
}
O_METHOD_END;

O_METHOD_BEGIN(exp_VtkExampleObject, VtkSurfaceMeshWidgetExample, "VTK Draw Surface Mesh", 0, 0, "")
{
  if (VARLID_ARG)
  {
    // add a simple mesh with 6 faces
    QVector<Double3> points;
    QVector<QVector<int>> faces;
    QVector<double> values;
    const int ni = 4;
    const int nj = 3;
    for (int i = 0; i < ni; i++)
      for (int j = 0; j < nj; j++)
      {
        points.push_back(Double3(i, j, 0));
        values.push_back(log(i * j + 1));
      }

    auto id = [&](int i, int j)
    {
      return i * nj + j;
    };

    for (int i = 0; i < ni - 1; i++)
      for (int j = 0; j < nj - 1; j++)
      {
        QVector<int> L = { id(i, j), id(i + 1, j), id(i + 1, j + 1), id(i, j + 1) };
        faces.push_back(L);
      }

    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 1},
      {"showMode", "Edge+Face"}
    };

    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("VtkSurfaceMeshWidgetExample");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetVtk", holder))
      {
        w->setSizeHint(QSize(800, 600));
        w->displaySurfaceMesh("Surface mesh", points, faces, &values, &args);
        holder->placeWidget(w);
      }
      if (dlg.exec() == QDialog::Accepted)
      {

      }
    }
  }
}
O_METHOD_END;

//for cst method test add by Jian
#if 0
O_METHOD_BEGIN(exp_VtkExampleObject, CSTSurfaceGraph, "Parameterized airplane geometry test using CST method", 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg(core_Application::core());
    dlg.setWindowTitle("WidgetExampleSurfaceGraph");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      QVector<QVector<QVector<double> > > dataU;
      QVector<QVector<QVector<double> > > dataD;
      {
        int pointCount = 50;
        int crossSectionCount = 50;
        double etaStep = 1.0 / (pointCount - 1);
        double psiStep = 1.0 / (crossSectionCount - 1);

        double Nyu = 2;
        double Nu = 2;
        double Nyd = 10;
        double Nd = 3;
        double Nz = 1;
        
        double L = 1.0;
        double W = 1;
        double Hu = 1;
        double Hd = 1;

        double eta = 0;
        double psi = 0;

        double x = 0;
        double y = 0;
        double z = 0;
#if 1
        for(int i = 0; i < crossSectionCount; i++) {
          psi = i * psiStep;
          eta = 0;
          x = 0;
          y = 0;
          z = 0;
          QVector<QVector<double> > row;
          for (int j = 0; j < pointCount; j++)
          {
            QVector<double> point;
            eta = j * etaStep;
            y = psi * L;
            z = pow(2, 2 * Nyu)*pow(psi, Nyu)*pow(1 - psi, Nyu)*Hu*pow(2, 2 * Nu)*pow(eta, Nu)*pow(1 - eta, Nu);
            x = -1 * pow(2, 2 * Nz)*pow(psi, Nz)*pow(1 - psi, Nz)*(1.0-2*eta)*W;
            point.push_back(x);
            point.push_back(y);
            point.push_back(z);
            row.push_back(point);
          }
          dataU.push_back(row);
        }
#endif 

#if 0
        for (int i = 0; i < crossSectionCount; i++) {
          psi = i * psiStep;
          eta = 0;
          x = 0;
          y = 0;
          z = 0;
          QVector<QVector<double> > row;
          for (int j = 0; j < pointCount; j++)
          {
            QVector<double> point;
            eta = j * etaStep;
            y = psi * L;
            z = -1*pow(2, 2 * Nyd)*pow(psi, Nyd)*pow(1 - psi, Nyd)*Hd*pow(2, 2 * Nd)*pow(eta, Nd)*pow(1 - eta, Nd);
            x = -1 * pow(2, 2 * Nz)*pow(psi, Nz)*pow(1 - psi, Nz)*(1 - 2 * eta)*W;
            point.push_back(x);
            point.push_back(y);
            point.push_back(z);
            row.push_back(point);
          }
          dataD.push_back(row);
        }
#endif 
      }

      QStringList names = QStringList()
        << "x"
        << "yu"
        << "z"
        ;

      QVector<int> unitCats = QVector<int>()
        << TUnit::length
        << TUnit::length
        << TUnit::length
        ;

      if (plt_SurfaceGraphWidget* surfaceU = new plt_SurfaceGraphWidget(&dlg, 0, 0))
      {
        surfaceU->setData(dataU, names, unitCats,
          names[0], names[1], names[2]);

        holder->placeWidget(surfaceU, 1, 0, 1, 2);
      }

      //if (plt_SurfaceGraphWidget* surfaceD = new plt_SurfaceGraphWidget(&dlg, 0, 0))
      //{
      //  surfaceD->setData(dataD, names, unitCats,
      //    names[0], names[1], names[2]);

      //  holder->placeWidget(surfaceD, 1, 0, 1, 2);
      //}
    }

    if (dlg.exec() == QDialog::Accepted)
    {
    }

    return true;
  }
}
O_METHOD_END;
#endif 

O_METHOD_BEGIN(exp_VtkExampleObject, VtkCSTProfile, "VTK Draw use CST method", 0, 0, "")
{
  if (VARLID_ARG)
  {
    DEFINE_ARG(double, Nyu, 1.0, NULL, "Nyu", false);
    DEFINE_ARG(double, Nu, 2, NULL, "Nu", false);
    DEFINE_ARG(double, Nyd, 2.0, NULL, "Nyd", false);
    DEFINE_ARG(double, Nd, 5, NULL, "Nd", false);
    DEFINE_ARG(double, Nz, 0.5, NULL, "Nz", false);

    DEFINE_ARG(double, Length, 20.0, TUnit::length, "L", false);
    DEFINE_ARG(double, W, 3, TUnit::length, "W", false);
    DEFINE_ARG(double, Hu, 5, TUnit::length, "Hu", false);
    DEFINE_ARG(double, Hd, 1, TUnit::length, "Hd", false);

    int pointCount = 50;
    int crossSectionCount = 50;
    double etaStep = 1.0 / (pointCount - 1);
    double psiStep = 1.0 / (crossSectionCount - 1);

    //double Nyu = 2;
    //double Nu = 2;
    //double Nyd = 2;
    //double Nd = 5;
    //double Nz = 0.5;
    //double Length = 20.0;
    //double W = 3;
    //double Hu = 5;
    //double Hd = 1;

    double eta = 0;
    double psi = 0;

    QVector<QVector<Double3>> profilesUp;
    QVector<QVector<double>> valuesUp;
    QVector<QVector<Double3>> profilesDown;
    QVector<QVector<double>> valuesDown;

    //uppper cross-section of fuselage
    for (int n = 0; n < crossSectionCount; n++)
    {
      QVector<Double3> L;
      QVector<double> V;
      psi = 0.5*n * psiStep;
      //psi = n * psiStep;
      eta = 0;
      for (int i = 0; i < pointCount; i++)
      {
        eta = i * etaStep;
        Double3 x;      
        x[0] = -1 * pow(2, 2 * Nz)*pow(psi, Nz)*pow(1 - psi, Nz)*(1.0 - 2 * eta)*W;
        x[1] = pow(2, 2 * Nyu)*pow(psi, Nyu)*pow(1 - psi, Nyu)*Hu*pow(2, 2 * Nu)*pow(eta, Nu)*pow(1 - eta, Nu);
        x[2] = psi *2* Length;
        L.push_back(x);
        V.push_back(0);
      }
      profilesUp.push_back(L);
      valuesUp.push_back(V);
    }

    //lower cross-section of fuselage
    for (int n = 0; n < crossSectionCount; n++)
    {
      QVector<Double3> L;
      QVector<double> V;
      psi = 0.5*n * psiStep;
      //psi = n * psiStep;
      eta = 0;
      for (int i = 0; i < pointCount; i++)
      {
        eta = i * etaStep;
        Double3 x;
        x[0] = -1 * pow(2, 2 * Nz)*pow(psi, Nz)*pow(1 - psi, Nz)*(1.0 - 2 * eta)*W;
        x[1] = -1*pow(2, 2 * Nyd)*pow(psi, Nyd)*pow(1 - psi, Nyd)*Hd*pow(2, 2 * Nd)*pow(eta, Nd)*pow(1 - eta, Nd);
        x[2] = psi * 2 * Length;
        L.push_back(x);
        V.push_back(1);
      }
      profilesDown.push_back(L);
      valuesDown.push_back(V);
    }

    QMap<QString, QVariant> args =
    {
      {"colorName", "lightBlue"},
      {"lineWidth", 1},
      //{"showMode", "Edge+Face"}
    };

    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("VtkProfileWidgetExample");
    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1))
    {
      if (vis_Widget* w = vis_Widget::newWidget("vis_WidgetVtk", holder))
      {
        w->setSizeHint(QSize(800, 600));
        w->displaySurfaceFromProfiles("Multi-layer profiles", profilesUp, &valuesUp, &args);
        w->displaySurfaceFromProfiles("Multi-layer profiles1", profilesDown, &valuesDown, &args);
        holder->placeWidget(w);
      }
      if (dlg.exec() == QDialog::Accepted)
      {
      }
    }
  }
}
O_METHOD_END;

