#pragma once

#include <QMainWindow>
#include "ui_HelloQtChild.h"

class HelloQtChild : public QWidget
{
  Q_OBJECT

public:
  HelloQtChild(QWidget *parent = nullptr);
  ~HelloQtChild();

private slots:
	void DrawPolyline();

private:
  Ui::HelloQtChildClass ui;
};
