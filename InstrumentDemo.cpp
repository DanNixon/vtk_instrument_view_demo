#include "InstrumentDemo.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkAssembly.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

vtkSmartPointer<vtkAssembly> build_tube(int const num_pixels)
{
  auto assembly = vtkSmartPointer<vtkAssembly>::New();
  for (int i = 0; i < num_pixels; i++) {
    auto source = vtkSmartPointer<vtkCubeSource>::New();
    source->Update();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(source->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    /* todo */
    actor->SetPosition(0, i * 1.1, 0);
    actor->GetProperty()->SetColor(i / (float)num_pixels, 1. - (i / (float)num_pixels), 0);

    assembly->AddPart(actor);
  }
  return assembly;
}

vtkSmartPointer<vtkAssembly> build_bank(int const num_pixels, int num_tubes)
{
  auto assembly = vtkSmartPointer<vtkAssembly>::New();
  for (int i = 0; i < num_tubes; i++) {
    auto tube = build_tube(num_pixels);

    /* todo */
    tube->SetPosition(i * 1.1, 0, 0);

    assembly->AddPart(tube);
  }
  return assembly;
}

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

  auto assembly = vtkSmartPointer<vtkAssembly>::New();

  auto bank1 = build_bank(50, 10);
  assembly->AddPart(bank1);

  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(assembly);

#if VTK890
  vtkWidget->renderWindow()->AddRenderer(renderer);
#else
  vtkWidget->GetRenderWindow()->AddRenderer(renderer);
#endif
}
