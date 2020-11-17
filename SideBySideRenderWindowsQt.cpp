#include "SideBySideRenderWindowsQt.h"

#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

SideBySideRenderWindowsQt::SideBySideRenderWindowsQt()
{
  this->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindowLeft;
#if VTK890
  this->qvtkWidgetLeft->setRenderWindow(renderWindowLeft);
#else
  this->qvtkWidgetLeft->SetRenderWindow(renderWindowLeft);
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

  // VTK Renderer
  auto leftRenderer = vtkSmartPointer<vtkRenderer>::New();
  leftRenderer->AddActor(cubeActor);
  leftRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

  // VTK/Qt wedded
#if VTK890
  this->qvtkWidgetLeft->renderWindow()->AddRenderer(leftRenderer);
#else
  this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftRenderer);
#endif

  // Set up action signals and slots
  connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

void SideBySideRenderWindowsQt::slotExit()
{
  qApp->exit();
}
