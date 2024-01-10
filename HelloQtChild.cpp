#include "stdafx.h"
#include "HelloQtChild.h"

HelloQtChild::HelloQtChild(QWidget *parent)
  : QWidget(parent)
{
  ui.setupUi(this);

  QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(DrawPolyline()));
}

HelloQtChild::~HelloQtChild()
{}

void HelloQtChild::DrawPolyline()
{
	NcDb::Poly3dType type = NcDb::Poly3dType::k3dSimplePoly;
	NcGePoint3dArray vertices; //NcArray< NcGePoint3d >
	//NcDb3dPolylineVertex* vertex;
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

	/*NcGePoint3d startPt(4.0, 2.0, 0.0);
	NcGePoint3d endPt(10.0, 7.0, 0.0);
	NcDbLine* pLine = new NcDbLine(startPt, endPt);*/

	NcDbBlockTable* pBlockTable;
	ncdbHostApplicationServices()->workingDatabase()
		->getSymbolTable(pBlockTable, NcDb::kForRead);
	NcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(NCDB_MODEL_SPACE, pBlockTableRecord,
		NcDb::kForWrite);
	pBlockTable->close();
	AcDbObjectId lineId;
	//NcDbEntity e = static_cast<NcDbEntity>(polyline);
	pBlockTableRecord->appendNcDbEntity(lineId, polyline);
	pBlockTableRecord->close();
	//pLine.close();
	polyline->close();
}