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
#include <vtkCameraOrientationWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataReader.h>

static int vtk_camera_orientation()
{
  std::string fileName("C:\\SoftDev\\WorkSpace\\Cases\\vtk\\Bunny.vtp");

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iRen;

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(fileName.c_str());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  renWin->AddRenderer(renderer);
  renWin->SetSize(600, 600);
  renWin->SetWindowName("CameraOrientationWidget");

  // Important: The interactor must be set prior to enabling the widget.
  iRen->SetRenderWindow(renWin);

  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();

  renWin->Render();
  iRen->Initialize();
  iRen->Start();

  return EXIT_SUCCESS;
}
