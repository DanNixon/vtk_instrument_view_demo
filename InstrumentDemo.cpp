#include "InstrumentDemo.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

InstrumentDemo::InstrumentDemo()
{
  auto* vtkWidget = new QVTKOpenGLNativeWidget;
  setCentralWidget(vtkWidget);

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
#if VTK890
  vtkWidget->setRenderWindow(renderWindow);
#else
  vtkWidget->SetRenderWindow(renderWindow);
#endif

  vtkSmartPointer<vtkRectilinearGrid> grid = vtkSmartPointer<vtkRectilinearGrid>::New();

  grid->SetDimensions(10, 50, 2);

  vtkSmartPointer<vtkDoubleArray> xArray = vtkSmartPointer<vtkDoubleArray>::New();
  for (int i = 0; i < 10; i++)
    xArray->InsertNextValue(i);

  vtkSmartPointer<vtkDoubleArray> yArray = vtkSmartPointer<vtkDoubleArray>::New();
  for (int i = 0; i < 50; i++)
    yArray->InsertNextValue(i);

  vtkSmartPointer<vtkDoubleArray> zArray = vtkSmartPointer<vtkDoubleArray>::New();
  zArray->InsertNextValue(0.0);
  zArray->InsertNextValue(-1.0);

  grid->SetXCoordinates(xArray);
  grid->SetYCoordinates(yArray);
  grid->SetZCoordinates(zArray);

  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInputConnection(grid->GetProducerPort());
#else
  mapper->SetInputData(grid);
#endif

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);

#if VTK890
  vtkWidget->renderWindow()->AddRenderer(renderer);
#else
  vtkWidget->GetRenderWindow()->AddRenderer(renderer);
#endif
}
