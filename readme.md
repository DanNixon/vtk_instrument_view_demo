# Cross Platform Graphics Evaluation

## Background

The continued use of OpenGL for 3D rendering (primarily in the Instrument View) is a cause for concern.

- Apple deprecated OpenGL on all platforms (on macOS 10.14) in favour of Metal
- OpenGL is typically the worst performing graphics API
- OpenGL is not a pleasent API, which gives rise to readability and performance issues in the Instrument View code

To rectify the above issues the instrument view code should be refactored to use a new API (be it a graphics API or higher level).

## Options Considered

### Vulkan

Pros:
- Not OpenGL
- Very platform/device agnostic
- Easy to embed in a Qt window

Cons:
- Limited high level functionality
  - Would still need to keep/modify code for custom bank representation, picking, etc.

### Qt3D

Pros:
- Can use Vulkan as the graphics API
- Easy to use, high level API

Cons:
- Is Qt
  - Given the [questionable choices made by The Qt Company regarding open source use of their framework](https://www.theregister.com/2021/01/05/qt_lts_goes_commercial_only/) additional Qt component dependencies should be avoided

### VTK

Pros:
- Very versatile
- Good performance
- Easy to embed in a Qt window

Cons:
- Currently only supports OpenGL
  - Vulkan support is in the works
    - https://discourse.vtk.org/t/vulkan-development/3307
    - https://discourse.vtk.org/t/update-on-vulkan-support/2819

## VTK

The Visualisation Toolkit (VTK) is a well established library for scientific visualisation.

It has in built support for many of the features required for the Instrument View:

- Abstractions over detector constructions
- Detector pixel picking
- Intensity mapping

The included demo application shows how these can be assembled to recreate the core visualisation functionality of the instrument view:
- A `vtkRectilinearGrid` is used to represent a 2D detector panel
- A `vtkDataSetMapper` colours each cell based on it's intensity and a `vtkLookupTable`
- A custom `vtkInteractionStyle` is used to obtain the coordinates of a selected pixel and the bank it belongs to

A screenshot of the included demo is provided:
![Demo Application](./demo.png)

### Risks

The only significant risk involved with this approach is a potential lack of non-OpenGL renderers by the time OpenGL becomes unavailable on a platform we support.
As mentioned above, this is something being worked on and in the worst case it is unlikely that a platform will go unsupported for long given the range of other software in which VTK is used.
Despite not being in a "zero risk" situation through an adoption of VTK, the risk would be significantly reduced.

## Implementation within Mantid

### Overview

The majority of the work to replace the existing implementation likely resides within `InstrumentWidget`, `InstrumentRenderer` and each component type.
`InsturmentWidget` should handle the configuration of the VTK widget, interactor and additional functionality (e.g. axes, auxiliary controls).
`InstrumentRenderer` and each component type will change from just iterating over the components, to each component returning a `vtkActor` representing the specific part of the instrument.

## Next steps/roadmap

A potential roadmap for the implementation of a VTK based instrument view in Mantid is given below.
Note that the durations are given in person weeks.

The majority of work in stages 1, 3 and 4 can be performed by multiple developers in parallel.
Stage 2 contains the foundation work of integrating VTK in Mantid, therefore a good amount of this will not distribute well over multiple developers.

The task of improving dependency management for Mantid is a prerequisite of stage 2, this removes a non trivial task from this project.

1. Demonstration using all bank types Mantid currently supports (6w)
  - Cylinder
  - Mesh

2. Bare minimum implementation of instrument rendering (18w)
  - All detectors rendered
  - Basic picking
  - Intensity mapping

3. Full rendering (8w)
  - Non detector components
  - Axes indicators
  - Intuitive interaction

4. Equivalent functionality (18w)
  - Match picking functionality
  - Other things I have overlooked?
