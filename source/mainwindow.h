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
  void on_bicubicButton_clicked();

  void on_TileSizeLevel_valueChanged(int arg1);

  void on_displacementButton_clicked();
  void on_amplitudeSlider_valueChanged(int value);

  void on_radioButton_clicked();
  void on_radioButton_2_clicked();
  void on_radioButton_3_clicked();
  void on_radioButton_4_clicked();

  void on_detailSlider_valueChanged(int value);
  void on_levelOfDetailCheckBox_clicked(bool checked);

  void on_phong_shad_clicked();
  void on_norm_shad_clicked();

  void on_true_norms_clicked();
  void on_approx_norms_clicked();
  void on_interpolated_norms_clicked();



private:
  void importOBJ(const QString &fileName);

  Ui::MainWindow *ui;
  Subdivider *subdivider;
  QVector<Mesh> meshes;
  Mesh *currentMesh;
};

#endif // MAINWINDOW_H
