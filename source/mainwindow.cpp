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
  ui->MeshRenderGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
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
    // TODO check for limit projection
    ui->MainDisplay->updateBuffers(meshes[0]);
    currentMesh = &meshes[0];
    ui->MainDisplay->settings.modelLoaded = true;
  } else {
    ui->MainDisplay->settings.modelLoaded = false;
  }

  ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
  ui->MeshRenderGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
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
    ui->MeshRenderGroupBox->setEnabled(true);
  } else {
    // Upon enabling tessellation the previous settings should be restored
    // again.
    bool showMesh = !ui->HideMeshCheckBox->isChecked();
    ui->MainDisplay->settings.showCpuMesh = showMesh;
    ui->MeshRenderGroupBox->setEnabled(showMesh);
  }
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_HideMeshCheckBox_toggled(bool checked) {
  // Useful for clearly seeing only the patches rendered by the Tessellation
  // shaders.
  ui->MainDisplay->settings.showCpuMesh = !checked;
  ui->MeshRenderGroupBox->setEnabled(!checked);
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_OriginalProjectionButton_pressed() {
  ui->MainDisplay->settings.limitProjectionMode = false;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_LimitProjectionButton_pressed() {
  ui->MainDisplay->settings.limitProjectionMode = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_bilinearButton_clicked() {
  ui->MainDisplay->settings.currentTessellationShader = ShaderType::BILINEAR;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_bicubicButton_clicked() {
  ui->MainDisplay->settings.currentTessellationShader = ShaderType::BICUBIC;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}

void MainWindow::on_radioButton_clicked() {
  ui->MainDisplay->settings.currentTessellationShader = ShaderType::DISPLACEMENT;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->updateBuffers(*currentMesh);
}


void MainWindow::on_InnerTessLevel_valueChanged(int arg1) {
  ui->MainDisplay->settings.innerTessellationLevel = arg1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}

void MainWindow::on_OuterTessLevel_valueChanged(int arg1) {
  ui->MainDisplay->settings.outerTessellationLevel = arg1;
  ui->MainDisplay->settings.uniformUpdateRequired = true;
  ui->MainDisplay->update();
}


