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
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law. 
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TaiZe Inc.
 */
#pragma once

#include <vtkActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <array>

static int vtk_logo_widget()
{

  vtkNew<vtkNamedColors> colors;
  // colors->SetColor("Bkg", 0.2, 0.3, 0.4);

  // A sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(4.0);
  sphereSource->SetPhiResolution(4);
  sphereSource->SetThetaResolution(8);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LogoWidget");

  renderer->AddActor(actor);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create an image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 200, 0, 200, 0, 0);

  std::array<double, 3> drawColor1{ 0, 0, 0 };
  auto color1 = colors->GetColor3ub("Coral").GetData();
  std::array<double, 3> drawColor2{ 0, 0, 0 };
  auto color2 = colors->GetColor3ub("Black").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Clear the image
  // Note: SetDrawColour() uses double values of the rgb colors in the
  //       range [0 ... 255]
  //       So SetDrawColour(255, 255, 255) is white.
  drawing->SetDrawColor(drawColor1.data());
  drawing->FillBox(0, 200, 0, 200);
  drawing->SetDrawColor(drawColor2.data());
  drawing->DrawCircle(100, 100, 50);
  drawing->Update();

  vtkNew<vtkLogoRepresentation> logoRepresentation;
  logoRepresentation->SetImage(drawing->GetOutput());
  logoRepresentation->SetPosition(0, 0);
  logoRepresentation->SetPosition2(0.4, 0.4);
  logoRepresentation->GetImageProperty()->SetOpacity(.7);

  vtkNew<vtkLogoWidget> logoWidget;
  logoWidget->SetInteractor(renderWindowInteractor);
  logoWidget->SetRepresentation(logoRepresentation);

  renderWindow->Render();
  logoWidget->On();
  renderer->SetBackground(colors->GetColor3d("MidnightBLue").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
