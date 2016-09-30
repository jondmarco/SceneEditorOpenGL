#pragma once

#include "SceneEditorUI.h"
#include "mainwindow.h"
#include <QApplication>

#include <thread>
#include <iostream>

#include "App\SceneEditor.h"

using namespace std;

SceneEditorUI::SceneEditorUI()
{
	int argc = NULL;
	char *argv[1];
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	a.exec();
}
