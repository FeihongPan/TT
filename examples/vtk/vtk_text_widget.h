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
#include <vtkCommand.h>
#include <vtkCoordinate.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>

static int vtk_text_widget()
{
  vtkNew<vtkNamedColors> colors;

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Create a test pipeline
  vtkNew<vtkSphereSource> sphereSource;
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create the widget
  vtkNew<vtkTextActor> textActor;
  textActor->SetInput("This is a test");
  textActor->GetTextProperty()->SetColor(colors->GetColor3d("Lime").GetData());

  vtkNew<vtkTextWidget> textWidget;

  vtkNew<vtkTextRepresentation> textRepresentation;
  textRepresentation->GetPositionCoordinate()->SetValue(0.15, 0.15);
  textRepresentation->GetPosition2Coordinate()->SetValue(0.7, 0.2);
  textWidget->SetRepresentation(textRepresentation);

  textWidget->SetInteractor(interactor);
  textWidget->SetTextActor(textActor);
  textWidget->SelectableOff();

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderWindow->SetSize(300, 300);
  renderWindow->SetWindowName("TextWidget");

  interactor->Initialize();
  renderWindow->Render();
  textWidget->On();
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
