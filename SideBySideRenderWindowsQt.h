#pragma once

#include <vtkSmartPointer.h>

#include <QMainWindow>

class SideBySideRenderWindowsQt : public QMainWindow
{
  Q_OBJECT
public:
  SideBySideRenderWindowsQt();
  ~SideBySideRenderWindowsQt() = default;
};
