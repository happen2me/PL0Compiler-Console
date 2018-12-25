#pragma once

#include <QMainWindow>
#include "ui_CompilerWindow.h"

class CompilerWindow : public QMainWindow
{
	Q_OBJECT

public:
	CompilerWindow(QWidget *parent = Q_NULLPTR);
	~CompilerWindow();

private:
	Ui::CompilerWindow ui;
};
