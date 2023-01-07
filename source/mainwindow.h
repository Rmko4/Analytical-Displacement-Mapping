#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include "mesh/mesh.h"
#include "subdivision/subdivider.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class represents the main window.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void on_LoadOBJ_pressed();
  void on_MeshPresetComboBox_currentTextChanged(const QString &meshName);
  void on_SubdivSteps_valueChanged(int subdivLevel);
  void on_TessellationCheckBox_toggled(bool checked);

  void on_HideMeshCheckBox_toggled(bool checked);
  void on_OriginalProjectionButton_pressed();
  void on_LimitProjectionButton_pressed();

  void on_bilinearButton_clicked();
  void on_bicubicButton_clicked();

  void on_InnerTessLevel_valueChanged(int arg1);

  void on_displacementButton_clicked();

  void on_amplitudeSlider_valueChanged(int value);

private:
  void importOBJ(const QString &fileName);

  Ui::MainWindow *ui;
  Subdivider *subdivider;
  QVector<Mesh> meshes;
  Mesh *currentMesh;
};

#endif // MAINWINDOW_H
