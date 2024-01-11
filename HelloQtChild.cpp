#include "stdafx.h"
#include "HelloQtChild.h"
#include <string>

Mediator mediator;

HelloQtChild::HelloQtChild(QWidget *parent)
  : QWidget(parent)
{
  ui.setupUi(this);
  mediator.SetUI(&ui);
  QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(DrawPolyline()));
  QObject::connect(ui.tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(UpdatePolyline(int, int)));
}

HelloQtChild::~HelloQtChild()
{
}

void HelloQtChild::SelectPolyline()
{
	NcEditorReactor reactor;
	reactor.pickfirstModified();
}

void HelloQtChild::DrawPolyline()
{
	NcDb::Poly3dType type = NcDb::Poly3dType::k3dSimplePoly;
	NcGePoint3dArray vertices;
	NRX::Boolean closed = NRX::kFalse;

	for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
		QTableWidgetItem* item = ui.tableWidget->item(i, 0);
		if (item != nullptr) {
			QString s0 = item->text();
			if (!s0.isEmpty()) {
				item = ui.tableWidget->item(i, 1);
				if (item != nullptr) {
					QString s1 = item->text();
					if (!s1.isEmpty()) {
						item = ui.tableWidget->item(i, 2);
						if (item != nullptr) {
							QString s2 = item->text();
							if (!s2.isEmpty()) {
								NcGePoint3d p(s0.toDouble(), s1.toDouble(), s2.toDouble());
								vertices.push_back(p);
							}
							else {
								NcGePoint3d p(s0.toDouble(), s1.toDouble(), 0);
								vertices.push_back(p);
							}
						}
						else {
							NcGePoint3d p(s0.toDouble(), s1.toDouble(), 0);
							vertices.push_back(p);

						}
					}
				}
			}
		}
	}

	NcDb3dPolyline* polyline = new NcDb3dPolyline(type, vertices, closed);

	NcDbBlockTable* pBlockTable;
	ncdbHostApplicationServices()->workingDatabase()
		->getSymbolTable(pBlockTable, NcDb::kForRead);
	NcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(NCDB_MODEL_SPACE, pBlockTableRecord,
		NcDb::kForWrite);
	pBlockTable->close();
	AcDbObjectId lineId;
	pBlockTableRecord->appendNcDbEntity(lineId, polyline);
	pBlockTableRecord->close();
	polyline->close();
}

void HelloQtChild::UpdatePolyline(int row, int column)
{
	bool first_found = false;
	nds_name ss;
	int status = ncedSSGet(L"I", NULL, NULL, NULL, ss);

	if (status == RTNORM) {
		long length = 0;
		if ((ncedSSLength(ss, &length) != RTNORM) && (length == 0)) {
			ncedSSFree(ss);
		}
		else {
			nds_name ent;
			NcDbObjectId id = NcDbObjectId::kNull;

			for (long i = 0; i < length; i++) {
				if (!first_found) {
					if (ncedSSName(ss, i, ent) == RTNORM) {
						if (ncdbGetObjectId(id, ent) == Nano::ErrorStatus::eOk) {
							if (id.objectClass() == NcDb3dPolyline::desc()) {
								first_found = true;
								NcDb3dPolyline* polyline;
								Nano::ErrorStatus status = ncdbOpenObject(polyline, id, NcDb::kForWrite);
								NcDbObjectIterator* iterator = polyline->vertexIterator();

								status = polyline->close();
								NcDb3dPolylineVertex* vertex;
								NcDbObjectId obj_id;
								NcGePoint3d location;

								QTableWidgetItem* item = ui.tableWidget->item(row, column);
								QString val_qs = item->text();
								double val = val_qs.toDouble();

								for (int vertex_num = 0; ((!iterator->done())&&(vertex_num <= row)); vertex_num++, iterator->step())
								{
									if (vertex_num == row) {
										obj_id = iterator->objectId();
										status = ncdbOpenObject(vertex, obj_id, NcDb::kForWrite);

										location = vertex->position();

										if (column == 0) { //x
											location.x = val;
										}
										else if (column == 1) { //y
											location.y = val;
										}
										else { //z
											location.z = val;
										}
										status = vertex->setPosition(location);
										int a = 100;
										status = vertex->close();
									}
								}
								delete iterator;
							}
						}
					}
				}
			}
		}
		ncedSSFree(ss);
	}
}

MyReactor::MyReactor(const bool autoInitAndRelease)
{
	mediator_ = &mediator;
	m_autoInitAndRelease = autoInitAndRelease;
	if (m_autoInitAndRelease)
		if (NULL != ncedEditor)
			acedEditor->addReactor(this);
		else
			m_autoInitAndRelease = false;
}

MyReactor::~MyReactor()
{
	if (m_autoInitAndRelease)
		if (NULL != ncedEditor)
			ncedEditor->removeReactor(this);
}

void MyReactor::pickfirstModified()
{
    bool first_found = false;
	nds_name ss;
	int status = ncedSSGet(L"I", NULL, NULL, NULL, ss);

	if (status == RTNORM) {
		long length = 0;
		if ((ncedSSLength(ss, &length) != RTNORM) && (length == 0)) {
			ncedSSFree(ss);
		}
		else {
			nds_name ent;
			NcDbObjectId id = NcDbObjectId::kNull;

			for (long i = 0; i < length; i++) {
				if (!first_found) {
					if (ncedSSName(ss, i, ent) == RTNORM) {
						if (ncdbGetObjectId(id, ent) == Nano::ErrorStatus::eOk) {
							if (id.objectClass() == NcDb3dPolyline::desc()) {
								first_found = true;
								NcDb3dPolyline* polyline;
								ncdbOpenObject(polyline, id, NcDb::kForRead);
								NcDbObjectIterator* iterator = polyline->vertexIterator();
								NcDb3dPolylineVertex* vertex;
								NcGePoint3d location;
								NcDbObjectId obj_id;

								mediator_->ClearTable();

								for (int vertex_num = 0; !iterator->done();
									vertex_num++, iterator->step())
								{
									obj_id = iterator->objectId();
									ncdbOpenObject(vertex, obj_id,
										NcDb::kForRead);
									location = vertex->position();
									vertex->close();
									mediator_->UpdateTable(vertex_num, location);
								}
								polyline->close();
								delete iterator;
							}
						}
					}

				}
			}
		}
		ncedSSFree(ss);
	}
}

void Mediator::SetUI(Ui::HelloQtChildClass* ui)
{
	med_ui_ = ui;
}

void Mediator::UpdateTable(long row, NcGePoint3d value)
{
	med_ui_->tableWidget->blockSignals(true);
	QTableWidgetItem* item_x = new QTableWidgetItem;
	std::string str = std::to_string(value.x);
	item_x->setText(str.c_str());
	med_ui_->tableWidget->setItem(row, 0, item_x);
	QTableWidgetItem* item_y = new QTableWidgetItem;
	str = std::to_string(value.y);
	item_y->setText(str.c_str());
	med_ui_->tableWidget->setItem(row, 1, item_y);
	QTableWidgetItem* item_z = new QTableWidgetItem;
	str = std::to_string(value.z);
	item_z->setText(str.c_str());
	med_ui_->tableWidget->setItem(row, 2, item_z);
	med_ui_->tableWidget->blockSignals(false);
}

void Mediator::ClearTable()
{
	med_ui_->tableWidget->blockSignals(true);
	QTableWidgetItem* item = NULL;
	std::string empty_s;
	for (int i = 0; i < med_ui_->tableWidget->rowCount(); i++) {
		for (int j = 0; j < med_ui_->tableWidget->columnCount(); j++) {
			item = med_ui_->tableWidget->item(i, j);
			if (item != NULL) {
				item->setText(empty_s.c_str());
			}
		}
	}
	med_ui_->tableWidget->blockSignals(false);
}