#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SceneEditor.h"
#include "Game\Light.h"

SceneEditor* _scene;
bool _appStarted;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	_appStarted = false;
    ui->setupUi(this);
    setDefaults ();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow :: setDefaults () {

    ui->doubleSpinBox->setRange(0.00,1.0);
    ui->doubleSpinBox_2->setRange(0.00,1.0);
    ui->doubleSpinBox_3->setRange(0.00,1.0);

    ui->doubleSpinBox-> setSingleStep(0.01);
    ui->doubleSpinBox_2-> setSingleStep(0.01);
    ui->doubleSpinBox_3-> setSingleStep(0.01);

    ui->doubleSpinBox_4->setRange(0.00,1.0);
    ui->doubleSpinBox_5->setRange(0.00,1.0);
    ui->doubleSpinBox_6->setRange(0.00,1.0);

    ui->doubleSpinBox_4-> setSingleStep(0.01);
    ui->doubleSpinBox_5-> setSingleStep(0.01);
    ui->doubleSpinBox_6-> setSingleStep(0.01);

    ui->doubleSpinBox_7->setRange(0.00,1.0);
    ui->doubleSpinBox_8->setRange(0.00,1.0);
    ui->doubleSpinBox_9->setRange(0.00,1.0);

    ui->doubleSpinBox_7-> setSingleStep(0.01); // translate x
    ui->doubleSpinBox_8-> setSingleStep(0.01);
    ui->doubleSpinBox_9-> setSingleStep(0.01);

    ui->doubleSpinBox_10->setRange(-100.0, 100.0);
    ui->doubleSpinBox_11->setRange(-100.0, 100.0); // translate y
    ui->doubleSpinBox_12->setRange(-100.0, 100.0);

    ui->doubleSpinBox_10-> setSingleStep(0.01);
    ui->doubleSpinBox_11-> setSingleStep(0.01);
    ui->doubleSpinBox_12-> setSingleStep(0.01); // translate z

    ui->doubleSpinBox_13->setRange(-360.00, 360.0); // rotate x
    ui->doubleSpinBox_14->setRange(-360.00, 360.0); // rotate y
    ui->doubleSpinBox_15->setRange(-360.00, 360.0); // rotate z

    ui->doubleSpinBox_13-> setSingleStep(0.1);
    ui->doubleSpinBox_14-> setSingleStep(0.1);
    ui->doubleSpinBox_15-> setSingleStep(0.1);

    ui->doubleSpinBox_16->setRange(0.00,100.0); // scale x
    ui->doubleSpinBox_17->setRange(0.00,100.0); // scale y
    ui->doubleSpinBox_18->setRange(0.00,100.0); // scale z

    ui->doubleSpinBox_16-> setSingleStep(0.01);
    ui->doubleSpinBox_17-> setSingleStep(0.01);
    ui->doubleSpinBox_18-> setSingleStep(0.01);
}

//BUTTONS :

//SAVE
void MainWindow::on_pushButton_clicked()
{
	if (_appStarted)
	{
		_scene->SaveScene();
	}
}

//LOAD
void MainWindow::on_pushButton_2_clicked()
{
	if (!_appStarted)
	{
		Mode mode = PLAY;
		if (ui->checkBox->isChecked()) {
			mode = EDIT;
		}

		_scene = new SceneEditor();
		_appStarted = true;
		_scene->runApp(true, mode);

		ui->pushButton_2->setEnabled(false);
		ui->pushButton_6->setEnabled(false);
	}
}

//UPDATE COLORS
void MainWindow::on_pushButton_3_clicked()
{
	
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT && !_scene->GetSelectedEntity()->isLight())
	{
		float r = ui->doubleSpinBox->value();
		float g = ui->doubleSpinBox_2->value();
		float b = ui->doubleSpinBox_3->value();

		_scene->GetSelectedEntity()->setBaseColor(r, g, b);
	}
}

//IMPORT LIGHT
void MainWindow::on_pushButton_4_clicked()
{
	if (_appStarted && _scene->GetMode() == EDIT)
	{
		float intensity = ui->doubleSpinBox_8->value();

		float colorR = ui->doubleSpinBox_6->value() * intensity;
		float colorG = ui->doubleSpinBox_4->value() * intensity;
		float colorB = ui->doubleSpinBox_5->value() * intensity;

		_scene->AddLight(vec3(0,3,0), vec3(colorR, colorG, colorB));
	}
}

// Exits the app.
void MainWindow::on_pushButton_5_clicked()
{
    QCoreApplication::exit(); // Exits the application
}

//New Scene
void MainWindow::on_pushButton_6_clicked()
{
	if (!_appStarted)
	{
		Mode mode = PLAY;
		if (ui->checkBox->isChecked()) {
			mode = EDIT;
		}
		_scene = new SceneEditor();
		_appStarted = true;
		_scene->runApp(false, mode);

		ui->pushButton_2->setEnabled(false);
		ui->pushButton_6->setEnabled(false);
	}
}

// apply Light
void MainWindow::on_pushButton_11_clicked()
{
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT)
	{
		if (_scene->GetSelectedEntity()->isLight())
		{
			float intensity = ui->doubleSpinBox_8->value();

			float colorR = ui->doubleSpinBox_6->value() * intensity;
			float colorG = ui->doubleSpinBox_4->value() * intensity;
			float colorB = ui->doubleSpinBox_5->value() * intensity;


			_scene->GetSelectedEntity()->setBaseColor(colorR, colorG, colorB);
		}
	}
}

// apply Scale
void MainWindow::on_pushButton_7_clicked()
{
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT)
	{
		float sx = (ui->doubleSpinBox_16->value() == 0.0f) ? 1.0: ui->doubleSpinBox_16->value();
		float sy = (ui->doubleSpinBox_17->value() == 0.0f) ? 1.0 : ui->doubleSpinBox_17->value();;
		float sz = (ui->doubleSpinBox_18->value() == 0.0f) ? 1.0 : ui->doubleSpinBox_18->value();;

		_scene->GetSelectedEntity()->Scale(sx,sy,sz);
	}
}

// apply translate
void MainWindow::on_pushButton_8_clicked()
{
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT)
	{
		_scene->GetSelectedEntity()->Translate(ui->doubleSpinBox_7->value(), ui->doubleSpinBox_11->value(), ui->doubleSpinBox_12->value());
	}
}

// apply rotate
void MainWindow::on_pushButton_9_clicked()
{
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT)
	{
		_scene->GetSelectedEntity()->Rotate(ui->doubleSpinBox_13->value(), 1.0f, 0.0f, 0.0f);
		_scene->GetSelectedEntity()->Rotate(ui->doubleSpinBox_14->value(), 0.0f, 1.0f, 0.0f);
		_scene->GetSelectedEntity()->Rotate(ui->doubleSpinBox_15->value(), 0.0f, 0.0f, 1.0f);
	}
}

// import object
void MainWindow::on_pushButton_10_clicked()
{
	if (_appStarted && _scene->GetMode() == EDIT)
	{
		_scene->LoadEntity();
	}
}


//material
void MainWindow::on_pushButton_13_clicked()
{
	if (_appStarted && _scene->GetSelectedEntity() != nullptr && _scene->GetMode() == EDIT)
	{
		_scene->GetSelectedEntity()->setMetallness(ui->doubleSpinBox_10->value());
		_scene->GetSelectedEntity()->setRoughness(ui->doubleSpinBox_9->value());
	}
}
