#include "InstrumentDemo.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkAxesActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
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

  vtkSmartPointer<vtkFloatArray> cellData = vtkSmartPointer<vtkFloatArray>::New();
  for (int i = 0; i < 9 * 49; i++)
    cellData->InsertNextValue(static_cast<float>(9 * 49) / i);
  /* grid->GetCellData()->SetScalars(cellData); */

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(9 * 49);
  lut->Build();
  vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
  lut->SetTableValue(0, colors->GetColor4d("Blue").GetData());
  lut->SetTableValue((9 * 49) - 1, colors->GetColor4d("Red").GetData());

  vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInputConnection(grid->GetProducerPort());
#else
  mapper->SetInputData(grid);
#endif
  /* mapper->SetScalarRange(0, (9 * 49) - 1); */
  /* mapper->SetLookupTable(lut); */

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(-5.0, 0.0, 0.0);
    actor->SetUserTransform(transform);
  }

  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetLayer(1);
  renderer->AddActor(actor);

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(0.0, 0.0, 0.0);
    axes->SetUserTransform(transform);
  }

  // properties of the axes labels can be set as follows
  // this sets the x axis label to red
  // axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1,0,0);

  // the actual text of the axis label can be changed:
  // axes->SetXAxisLabelText("test");

  auto renderer2 = vtkSmartPointer<vtkRenderer>::New();
  renderer2->SetLayer(0);
  renderer2->AddActor(axes);
  renderer2->SetInteractive(false);

  renderer->SetActiveCamera(renderer2->GetActiveCamera());

#if VTK890
  vtkWidget->renderWindow()->SetNumberOfLayers(2);
  vtkWidget->renderWindow()->AddRenderer(renderer);
  vtkWidget->renderWindow()->AddRenderer(renderer2);
#else
  vtkWidget->GetRenderWindow()->SetNumberOfLayers(2);
  vtkWidget->GetRenderWindow()->AddRenderer(renderer);
  vtkWidget->GetRenderWindow()->AddRenderer(renderer2);
#endif
}
