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

#include "exp_VtkBaseExampleObject.h"
#include "w_PropertyHolderDialog.h"

REGISTER_OBJECT_CLASS(exp_VtkBaseExampleObject, "vtkBaseExampleObject", TObject);

exp_VtkBaseExampleObject::exp_VtkBaseExampleObject(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;  

  setTag("MethodOrder", "VtkHover,VtkContour,VtkHighlightPickedActor,VtkHighlightSilhouettePickedActor,$");
}

#include "vtk/vtk_hover.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkHover, "Vtk Hover", 0, 0, "Vtk Hover")
{
  if (VARLID_ARG)
  {
    vtk_hover();
  }
}
O_METHOD_END;

#include "vtk/vtk_area_pick.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkAreaPick, "Vtk Area Pick", 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_area_pick();
  }
}
O_METHOD_END;

#include "vtk/vtk_affine_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_affine_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_affine_widget();
  }
}
O_METHOD_END;

#include "vtk/vtk_compass_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_compass_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_compass_widget();
  }
}
O_METHOD_END;

#include "vtk/vtk_orientation_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_orientation_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_orientation_widget();
  }
}
O_METHOD_END;

#include "vtk/vtk_section.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkSelection, "Vtk Selection", 0, 0, "Vtk Selection")
{
  if (VARLID_ARG)
  {
    vtk_section();
  }
}
O_METHOD_END;

#include "vtk/vtk_contour.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkContour, "Vtk Contour", 0, 0, "Vtk Contour")
{
  if (VARLID_ARG)
  {
    vtk_contour();
  }
}
O_METHOD_END;

#include "vtk/vtk_highlight_picked_actor.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkHighlightPickedActor, "Vtk High light Picked Actor", 0, 0, "Vtk High light Picked Actor")
{
  if (VARLID_ARG)
  {
    vtk_highlight_picked_actor();
  }
}
O_METHOD_END;

#include "vtk/vtk_highlight_silhouette_picked_actor.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, VtkHighlightSilhouettePickedActor, "Vtk High light Silhouette Picked Actor", 0, 0, "Vtk High light Silhouette Picked Actor")
{
  if (VARLID_ARG)
  {
    vtk_highlight_silhouette_picked_actor();
  }
}
O_METHOD_END;

#include "vtk/vtk_text_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_text_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_text_widget();
  }
}
O_METHOD_END;


#include "vtk/vtk_textured_button.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_textured_button, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_textured_button();
  }
}
O_METHOD_END;

#include "vtk/vtk_logo_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_logo_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_logo_widget();
  }
}
O_METHOD_END;


#include "vtk/vtk_caption_widget.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_caption_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_caption_widget();
  }
}
O_METHOD_END;



#include "vtk/vtk_camera_orientation.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_camera_orientation, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    vtk_camera_orientation();
  }
}
O_METHOD_END;


#include "vtk/vtk_qt_widget.h"
#include "vtkDoubleArray.h"

namespace {
  void Randomize(vtkSphereSource* sphere, vtkMapper* mapper,
    vtkGenericOpenGLRenderWindow* window, std::mt19937& randEng)
  {
    // generate randomness
    double randAmp = 0.2 + ((randEng() % 1000) / 1000.0) * 0.2;
    double randThetaFreq = 1.0 + (randEng() % 9);
    double randPhiFreq = 1.0 + (randEng() % 9);

    // extract and prepare data
    sphere->Update();
    vtkSmartPointer<vtkPolyData> newSphere;
    newSphere.TakeReference(sphere->GetOutput()->NewInstance());
    newSphere->DeepCopy(sphere->GetOutput());
    vtkNew<vtkDoubleArray> height;
    height->SetName("Height");
    height->SetNumberOfComponents(1);
    height->SetNumberOfTuples(newSphere->GetNumberOfPoints());
    newSphere->GetPointData()->AddArray(height);

    // deform the sphere
    for (int iP = 0; iP < newSphere->GetNumberOfPoints(); iP++)
    {
      double pt[3] = { 0.0 };
      newSphere->GetPoint(iP, pt);
      double theta = std::atan2(pt[1], pt[0]);
      double phi =
        std::atan2(pt[2], std::sqrt(std::pow(pt[0], 2) + std::pow(pt[1], 2)));
      double thisAmp =
        randAmp * std::cos(randThetaFreq * theta) * std::sin(randPhiFreq * phi);
      height->SetValue(iP, thisAmp);
      pt[0] += thisAmp * std::cos(theta) * std::cos(phi);
      pt[1] += thisAmp * std::sin(theta) * std::cos(phi);
      pt[2] += thisAmp * std::sin(phi);
      newSphere->GetPoints()->SetPoint(iP, pt);
    }
    newSphere->GetPointData()->SetScalars(height);

    // reconfigure the pipeline to take the new deformed sphere
    mapper->SetInputDataObject(newSphere);
    mapper->SetScalarModeToUsePointData();
    mapper->ColorByArrayComponent("Height", 0);
    window->Render();
  }
} // namespace

#include "core_Application.h"
O_METHOD_BEGIN(exp_VtkBaseExampleObject, vtk_qt_widget, 0, 0, 0, "")
{
  if (VARLID_ARG)
  {
    w_PropertyHolderDialog dlg;
    dlg.setWindowTitle("WidgetExample02 123456 123456 123456 123456 123456 ");

    if (w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, "Any inputs"))
    {
      // main window
      QMainWindow* mainWindowP = new QMainWindow(core_Application::core());
      QMainWindow& mainWindow = *mainWindowP;
      mainWindow.resize(1200, 900);

      // control area
      QDockWidget controlDock;
      mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

      QLabel controlDockTitle("Control Dock");
      controlDockTitle.setMargin(20);
      controlDock.setTitleBarWidget(&controlDockTitle);

      QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
      QWidget layoutContainer;
      layoutContainer.setLayout(dockLayout);
      controlDock.setWidget(&layoutContainer);

      QPushButton randomizeButton;
      randomizeButton.setText("Randomize");
      dockLayout->addWidget(&randomizeButton);

      // render area
      QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget =
        new QVTKOpenGLNativeWidget(core_Application::core());
      mainWindow.setCentralWidget(vtkRenderWidget);

      // VTK part
      vtkNew<vtkGenericOpenGLRenderWindow> window;
      vtkRenderWidget->setRenderWindow(window.Get());

      vtkNew<vtkSphereSource> sphere;
      sphere->SetRadius(1.0);
      sphere->SetThetaResolution(100);
      sphere->SetPhiResolution(100);

      vtkNew<vtkDataSetMapper> mapper;
      mapper->SetInputConnection(sphere->GetOutputPort());

      vtkNew<vtkActor> actor;
      actor->SetMapper(mapper);
      actor->GetProperty()->SetEdgeVisibility(true);
      actor->GetProperty()->SetRepresentationToSurface();

      vtkNew<vtkRenderer> renderer;
      renderer->AddActor(actor);

      window->AddRenderer(renderer);

      // setup initial status
      std::mt19937 randEng(0);
      ::Randomize(sphere, mapper, window, randEng);

      // connect the buttons
      QObject::connect(&randomizeButton, &QPushButton::released,
        [&]() { ::Randomize(sphere, mapper, window, randEng); });

      mainWindow.show();



     
      {
        holder->placeWidget(mainWindowP);
      }

      if (dlg.exec() == QDialog::Accepted)
      {

      }
    }


    
  }
}
O_METHOD_END;

