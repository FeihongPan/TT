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

#include <vtkCellArray.h>
#include <vtkContourWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

static int vtk_contour()
{
  vtkNew<vtkNamedColors> colors;

  // Create the contour widget
  vtkNew<vtkContourWidget> contourWidget;

  // Override the default representation for the contour widget to customize its
  // look
  vtkNew<vtkOrientedGlyphContourRepresentation> contourRepresentation;
  contourRepresentation->GetLinesProperty()->SetColor(
    colors->GetColor3d("Red").GetData());
  contourWidget->SetRepresentation(contourRepresentation);

  // Generate a set of points arranged in a circle
  int numPts = 10;
  vtkNew<vtkPoints> points;
  for (int i = 0; i < numPts; i++)
  {
    // Create numPts points evenly spread around a circumference of radius 0.1
    const double angle = 2.0 * vtkMath::Pi() * i / numPts;
    points->InsertPoint(static_cast<vtkIdType>(i), 0.1 * cos(angle),
      0.1 * sin(angle), 0.0);
  }

  // Create a cell array to connect the points into meaningful geometry
  vtkIdType* vertexIndices = new vtkIdType[numPts + 1];
  for (int i = 0; i < numPts; i++)
  {
    vertexIndices[i] = static_cast<vtkIdType>(i);
  }
  // Set the last vertex to 0; this means the last line segment will join the
  // 19th point (vertices[19]) with the first one (vertices[0]), thus closing
  // the circle.
  vertexIndices[numPts] = 0;
  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(numPts + 1, vertexIndices);

  // Create polydata to hold the geometry just created, and populate it
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetLines(lines);

  // Create the renderer to visualize the scene
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  // Create the GUI window to hold the rendered scene
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ContourWidget");
  renderWindow->SetSize(600, 600);

  // Create the events manager for the renderer window
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Use the "trackball camera" interactor style, rather than the default
  // "joystick camera"
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  // Set up the contour widget within the visualization pipeline just assembled
  contourWidget->SetInteractor(interactor);
  contourWidget->On(); // Turn on the interactor observer
  contourWidget->Initialize(polydata);
  renderer->ResetCamera(); // Reposition camera to fit the scene elements

  // Start the interaction
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
