#include "InstrumentDemo.h"

#include <QDesktopWidget>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkIndent.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

class PickingInteractor : public vtkInteractorStyleTrackballCamera {
public:
  static PickingInteractor *New();
  vtkTypeMacro(PickingInteractor, vtkInteractorStyleTrackballCamera);

  virtual void OnLeftButtonDown() override {
    auto *picker = dynamic_cast<vtkPicker *>(this->Interactor->GetPicker());

    auto const screen_position = this->Interactor->GetEventPosition();
    std::cout << "\nPicking at screen coordinates (" << screen_position[0]
              << ", " << screen_position[1] << ")\n";

    picker->Pick(screen_position[0], screen_position[1], 0,
                 this->Interactor->GetRenderWindow()
                     ->GetRenderers()
                     ->GetFirstRenderer());

    /* Report the picked actors */
    auto *actors = picker->GetActors();
    {
      std::cout << "Actors\n";
      vtkIndent indent(1);
      actors->PrintSelf(std::cout, indent);
    }

    /* Report the picked positions, relative to the origin of the actor */
    auto *positions = picker->GetPickedPositions();
    {
      std::cout << "Positions\n";
      vtkIndent indent(1);
      positions->PrintSelf(std::cout, indent);
    }

    /* Forward events */
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
};

vtkStandardNewMacro(PickingInteractor);

auto generate_bank(vtkSmartPointer<vtkLookupTable> lut,
                   auto const min_intensity, auto const max_intensity) {
  constexpr auto cells_x = 50;
  constexpr auto cells_y = 256;
  constexpr auto num_cells = cells_x * cells_y;

  /* Create an axis aligned grid */
  vtkSmartPointer<vtkRectilinearGrid> grid =
      vtkSmartPointer<vtkRectilinearGrid>::New();
  {
    grid->SetDimensions(cells_x + 1, cells_y + 1, 2);

    /* Generate coordinates */
    vtkSmartPointer<vtkDoubleArray> xArray =
        vtkSmartPointer<vtkDoubleArray>::New();
    for (int i = 0; i <= cells_x; i++) {
      xArray->InsertNextValue(i - (cells_x / 2.));
    }

    vtkSmartPointer<vtkDoubleArray> yArray =
        vtkSmartPointer<vtkDoubleArray>::New();
    for (int i = 0; i <= cells_y; i++) {
      yArray->InsertNextValue(i - (cells_y / 2.));
    }

    vtkSmartPointer<vtkDoubleArray> zArray =
        vtkSmartPointer<vtkDoubleArray>::New();
    zArray->InsertNextValue(0.0);
    zArray->InsertNextValue(-1.0);

    /* Apply coordinates to grid */
    grid->SetXCoordinates(xArray);
    grid->SetYCoordinates(yArray);
    grid->SetZCoordinates(zArray);
  }

  /* Generate some scalar data to apply to each cell (e.g. inetnsity, event
   * count for a detector pixel) */
  {
    vtkSmartPointer<vtkFloatArray> cellData =
        vtkSmartPointer<vtkFloatArray>::New();
    for (int i = 0; i < num_cells; i++) {
      cellData->InsertNextValue((float)i * max_intensity / num_cells);
    }
    grid->GetCellData()->SetScalars(cellData);
  }

  /* Create a mapper for the grid, using the look up table from above */
  vtkSmartPointer<vtkDataSetMapper> mapper =
      vtkSmartPointer<vtkDataSetMapper>::New();
  mapper->SetScalarRange(min_intensity, max_intensity);
  mapper->SetLookupTable(lut);
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInputConnection(grid->GetProducerPort());
#else
  mapper->SetInputData(grid);
#endif

  /* Create an actor for the grid, nothing special here */
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  return actor;
}

InstrumentDemo::InstrumentDemo() {
  /* The widget the contains the VTK render window */
  auto *vtkWidget = new QVTKOpenGLNativeWidget;
  setCentralWidget(vtkWidget);

  /* Container for VTK rendering */
  vtkNew<vtkGenericOpenGLRenderWindow> render_window;
#if VTK890
  vtkWidget->setRenderWindow(render_window);
#else
  vtkWidget->SetRenderWindow(render_window);
#endif

  /* Use fixed values for pixel intensity/colour scale */
  constexpr auto min_intensity = 0.;
  constexpr auto max_intensity = 10.;

  /* Create a basic look up table to conver the scalar cell values to a colour
   */
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(256);
  lut->SetRange(min_intensity, max_intensity);
  lut->Build();

  /* Create the renderer and add it to the window */
  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  render_window->AddRenderer(renderer);

  /* Add banks, each of which is it's own actor */
  {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    renderer->AddActor(actor);
  }
  {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    vtkSmartPointer<vtkTransform> transform =
        vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->RotateY(15.0);
    transform->Translate(-50, 0, 10);
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }
  {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    vtkSmartPointer<vtkTransform> transform =
        vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->RotateY(-15.0);
    transform->Translate(50, 0, 10);
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }
  {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    vtkSmartPointer<vtkTransform> transform =
        vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->RotateY(30.0);
    transform->Translate(-100, 0, 25);
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }
  {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    vtkSmartPointer<vtkTransform> transform =
        vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->RotateY(-30.0);
    transform->Translate(100, 0, 25);
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }

  /* Add more banks for the sake of performance testing (I got lazy defining
   * instrument like positiions) */
  for (auto i = 0; i < 5; i++) {
    auto actor = generate_bank(lut, min_intensity, max_intensity);
    vtkSmartPointer<vtkTransform> transform =
        vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->Translate(150, 0, i * 25);
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }

  /* Create and configure the interactor */
  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  {
    /* Attach it to the render window */
    interactor->SetRenderWindow(render_window);

    /* Create a point based picker */
    vtkSmartPointer<vtkPointPicker> point_picker =
        vtkSmartPointer<vtkPointPicker>::New();
    interactor->SetPicker(point_picker);

    /* Use the custom interaction style from above */
    vtkSmartPointer<PickingInteractor> style =
        vtkSmartPointer<PickingInteractor>::New();
    interactor->SetInteractorStyle(style);

    interactor->Initialize();
  }

  /* Hackily make the window take up a reasonable amount of the screen */
  resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
}
