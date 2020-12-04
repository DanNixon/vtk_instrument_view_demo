#pragma once

#include <vtkSmartPointer.h>

#include <QMainWindow>

class InstrumentDemo : public QMainWindow
{
  Q_OBJECT
public:
  InstrumentDemo();
  ~InstrumentDemo() = default;
};
