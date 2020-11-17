#pragma once

#include <vtkSmartPointer.h>

#include <QMainWindow>

#include "ui_SideBySideRenderWindowsQt.h"

class SideBySideRenderWindowsQt : public QMainWindow,
                                  private Ui::SideBySideRenderWindowsQt
{
  Q_OBJECT
public:
  SideBySideRenderWindowsQt();
  ~SideBySideRenderWindowsQt() = default;

public slots:
  void slotExit();
};
