#include "SideBySideRenderWindowsQt.h"

#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <QVTKOpenGLNativeWidget.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

SideBySideRenderWindowsQt::SideBySideRenderWindowsQt()
{
  auto* vtkWidget = new QVTKOpenGLNativeWidget;
  setCentralWidget(vtkWidget);

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
#if VTK890
  vtkWidget->setRenderWindow(renderWindow);
#else
  vtkWidget->SetRenderWindow(renderWindow);
#endif

  // Cube
  auto cubeSource = vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->Update();
  vtkNew<vtkElevationFilter> cubeElev;
  cubeElev->SetInputConnection(cubeSource->GetOutputPort());
  cubeElev->SetLowPoint(0, -1.0, 0);
  cubeElev->SetHighPoint(0, 1.0, 0);
  auto cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  cubeMapper->SetInputConnection(cubeElev->GetOutputPort());
  auto cubeActor = vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(cubeMapper);

  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(cubeActor);

#if VTK890
  vtkWidget->renderWindow()->AddRenderer(renderer);
#else
  vtkWidget->GetRenderWindow()->AddRenderer(renderer);
#endif
}
