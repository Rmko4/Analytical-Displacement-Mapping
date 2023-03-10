#include "mainwindow.h"

#include "initialization/meshinitializer.h"
#include "initialization/objfile.h"
#include "subdivision/catmullclarksubdivider.h"
#include "subdivision/subdivider.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Creates a new Main Window UI.
 * @param parent Qt parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->tessSettingsGroupBox->setEnabled(
      ui->MainDisplay->settings.tesselationMode);
  ui->shadinggroupBox->setEnabled(
      ui->MainDisplay->settings.tesselationMode &&
      ui->MainDisplay->settings.currentTessellationShader ==
          ShaderType::DISPLACEMENT);
  ui->displacementGroupBox->setEnabled(
      ui->MainDisplay->settings.tesselationMode &&
      ui->MainDisplay->settings.currentTessellationShader ==
          ShaderType::DISPLACEMENT);
}

/**
 * @brief MainWindow::~MainWindow Deconstructs the main window.
 */
MainWindow::~MainWindow() {
  delete ui;

  meshes.clear();
  meshes.squeeze();
}

/**
 * @brief MainWindow::importOBJ Imports an obj file and adds the constructed
 * half-edge to the collection of meshes.
 * @param fileName Path of the .obj file.
 */
void MainWindow::importOBJ(const QString &fileName) {
  OBJFile newModel = OBJFile(fileName);
  meshes.clear();
  meshes.squeeze();

  if (newModel.loadedSuccessfully()) {
    MeshInitializer meshInitializer;
    meshes.append(meshInitializer.constructHalfEdgeMesh(newModel));
    ui->MainDisplay->updateBuffers(meshes[0]);
    currentMesh = &meshes[0];
    ui->MainDisplay->settings.modelLoaded = true;
  } else {
    ui->MainDisplay->settings.modelLoaded = false;
  }

  ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->shadinggroupBox->setEnabled(
      ui->MainDisplay->settings.tesselationMode &&
      ui->MainDisplay->settings.currentTessellationShader ==
          ShaderType::DISPLACEMENT);
  ui->displacementGroupBox->setEnabled(
      ui->MainDisplay->settings.tesselationMode &&
      ui->MainDisplay->settings.currentTessellationShader ==
          ShaderType::DISPLACEMENT);

  ui->SubdivSteps->setValue(0);
  ui->MainDisplay->update();
}

void MainWindow::on_LoadOBJ_pressed() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Import OBJ File", "../", tr("Obj Files (*.obj)"));
  importOBJ(filename);
}

void MainWindow::on_MeshPresetComboBox_currentTextChanged(
    const QString &meshName) {
  importOBJ(":/models/" + meshName + ".obj");
}

void MainWindow::on_SubdivSteps_valueChanged(int value) {
  ui->MainDisplay->settings.subdivSteps = value;
  Subdivider *subdivider = new CatmullClarkSubdivider();
  for (int k = meshes.size() - 1; k < value; k++) {
    meshes.append(subdivider->subdivide(meshes[k]));
  }
  ui->MainDisplay->updateBuffers(meshes[value]);
  currentMesh = &meshes[value];
  delete subdivider;
}

void MainWindow::on_TessellationCheckBox_toggled(bool checked) {
  ui->MainDisplay->settings.tesselationMode = checked;
  ui->tessSettingsGroupBox->setEnabled(checked);
  if (!checked) {
    // Always display cpu mesh when disabling tessellation
    ui->MainDisplay->settings.showCpuMesh = true;
  } else {
    // Upon enabling tessellation the previous settings should be restored
    // again.
    bool showMesh = !ui->HideMeshCheckBox->isChecked();
    ui->MainDisplay->settings.showCpuMesh = showMesh;
    //    ui->MeshRenderGroupBox->setEnabled(showMesh);
  }
  ui->MainDisplay->settings.uniformUpdateRequired = true;

  bool displacementMode = ui->MainDisplay->settings.tesselationMode &&
                          ui->MainDisplay->settings.currentTessellationShader ==
                              ShaderType::DISPLACEMENT;

  ui->shadinggroupBox->setEnabled(displacementMode);
  ui->displacementGroupBox->setEnabled(displacementMode);
  ui->DynamicTessGroupBox->setEnabled(displacementMode);

  ui->MainDisplay->update();
}

void MainWindow::on_HideMeshCheckBox_toggled(bool checked) {
  // Useful for clearly seeing only the patches rendered by the Tessellation
  // shaders.
  ui->MainDisplay->settings.showCpuMesh = !checked;
  //  ui->MeshRenderGroupBox->setEnabled(!checked);
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_bicubicButton_clicked() {
  ui->MainDisplay->settings.currentTessellationShader = ShaderType::BICUBIC;

  ui->shadinggroupBox->setEnabled(false);
  ui->displacementGroupBox->setEnabled(false);
  ui->DynamicTessGroupBox->setEnabled(false);

  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_displacementButton_clicked() {
  ui->MainDisplay->settings.currentTessellationShader =
      ShaderType::DISPLACEMENT;

  ui->shadinggroupBox->setEnabled(true);
  ui->displacementGroupBox->setEnabled(true);
  ui->DynamicTessGroupBox->setEnabled(true);

  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_TileSizeLevel_valueChanged(int arg1) {
  ui->MainDisplay->settings.tileSize = arg1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_levelOfDetailCheckBox_clicked(bool checked) {
  ui->MainDisplay->settings.dynamicLoD = checked;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_detailSlider_valueChanged(int value) {
  ui->MainDisplay->settings.tessDetail = static_cast<float>(value);
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_amplitudeSlider_valueChanged(int value) {
  ui->MainDisplay->settings.amplitude = static_cast<float>(value) / 50;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Bubblewrap displacement:
void MainWindow::on_dispMode1Button_clicked() {
  ui->MainDisplay->settings.displacement_mode = 0;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Pinhead displacement:
void MainWindow::on_dispMode2Button_clicked() {
  ui->MainDisplay->settings.displacement_mode = 1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Chocolate bar displacement:
void MainWindow::on_dispMode3Button_clicked() {
  ui->MainDisplay->settings.displacement_mode = 2;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Random displacement:
void MainWindow::on_dispMode4Button_clicked() {
  ui->MainDisplay->settings.displacement_mode = 3;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::enable_normal_buttons(bool enable) {
  ui->true_norms->setEnabled(enable);
  ui->approx_norms->setEnabled(enable);
  ui->interpolated_norms->setEnabled(enable);
}

// Using phong shading on the displacement mesh:
void MainWindow::on_phong_shad_clicked() {
  ui->MainDisplay->settings.shading_mode = 0;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  enable_normal_buttons(true);
  ui->MainDisplay->update();
}

// Visualizing normals on the displacment mesh:
void MainWindow::on_norm_shad_clicked() {
  ui->MainDisplay->settings.shading_mode = 1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  enable_normal_buttons(true);
  ui->MainDisplay->update();
}

void MainWindow::on_error_shad_clicked() {
  ui->MainDisplay->settings.shading_mode = 2;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  enable_normal_buttons(false);
  ui->MainDisplay->update();
}

// Using true normals that include that were calculated by uncluded the
// Weingarten term in the partial derivatives of the analytical surface function
// w.r.t. u and v:
void MainWindow::on_true_norms_clicked() {
  ui->MainDisplay->settings.normal_mode = 0;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Using an approximation that skips the Weingarten term (see
// on_true_norms_clicked comment)
void MainWindow::on_approx_norms_clicked() {
  ui->MainDisplay->settings.normal_mode = 1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

// Using approximation and doing this in the tese shader such that in the
// fragment shader we just use interpolated values for the normals
void MainWindow::on_interpolated_norms_clicked() {
  ui->MainDisplay->settings.normal_mode = 2;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}
