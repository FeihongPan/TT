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
#include <vtkHoverWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
  class vtkHoverCallback : public vtkCommand
  {
  public:
    static vtkHoverCallback* New()
    {
      return new vtkHoverCallback;
    }

    vtkHoverCallback()
    {
    }

    virtual void Execute(vtkObject*, unsigned long event,
      void* vtkNotUsed(calldata))
    {
      switch (event)
      {
      case vtkCommand::TimerEvent:
        std::cout
          << "TimerEvent -> the mouse stopped moving and the widget hovered"
          << std::endl;
        break;
      case vtkCommand::EndInteractionEvent:
        std::cout << "EndInteractionEvent -> the mouse started to move"
          << std::endl;
        break;
      }
    }
  };
} // namespace

static int vtk_hover()
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HoverWidget");

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget
  vtkNew<vtkHoverWidget> hoverWidget;
  hoverWidget->SetInteractor(renderWindowInteractor);
  hoverWidget->SetTimerDuration(1000);

  // Create a callback to listen to the widget's two VTK events
  vtkNew<vtkHoverCallback> hoverCallback;
  hoverWidget->AddObserver(vtkCommand::TimerEvent, hoverCallback);
  hoverWidget->AddObserver(vtkCommand::EndInteractionEvent, hoverCallback);

  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  hoverWidget->On();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
