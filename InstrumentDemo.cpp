#include "InstrumentDemo.h"

#include <QVTKOpenGLNativeWidget.h>
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
  mapper->SetScalarRange(0, (9 * 49) - 1);
  mapper->SetLookupTable(lut);

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
