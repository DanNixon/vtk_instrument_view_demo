#include "InstrumentDemo.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
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

  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  /* todo */

#if VTK890
  vtkWidget->renderWindow()->AddRenderer(renderer);
#else
  vtkWidget->GetRenderWindow()->AddRenderer(renderer);
#endif
}
