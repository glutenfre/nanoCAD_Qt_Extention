#pragma once

#include <QMainWindow>
#include "ui_HelloQtChild.h"

class Mediator
{
public:
	void SetUI(Ui::HelloQtChildClass* ui);
	void UpdateTable(long row, NcGePoint3d value);
	void ClearTable();
private:
	Ui::HelloQtChildClass* med_ui_;
};

class HelloQtChild : public QWidget
{
  Q_OBJECT

public:
  HelloQtChild(QWidget *parent = nullptr);
  ~HelloQtChild();

private slots:
	void DrawPolyline();
	void SelectPolyline();
	void UpdatePolyline();
private:
    Ui::HelloQtChildClass ui;
};

class MyReactor : public NcEditorReactor
{
public:
	MyReactor(const bool autoInitAndRelease = true);
	virtual ~MyReactor();
protected:
	void pickfirstModified() override;
private:
	Mediator* mediator_ = NULL;
	bool m_autoInitAndRelease;
};