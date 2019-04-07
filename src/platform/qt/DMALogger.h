#pragma once

#include <QDialog>
#include <QList>

#include <memory>

#include "ui_DMALogger.h"

namespace QGBA {

class CoreController;

class DMALogger : public QDialog {
Q_OBJECT

public:
	struct RegisterItem {
		RegisterItem(const QString& description, uint start, uint size = 1, bool readonly = false)
			: start(start)
			, size(size)
			, readonly(readonly)
			, description(description) {}
		RegisterItem(const QString& description, uint start, uint size, QStringList items, bool readonly = false)
			: start(start)
			, size(size)
			, readonly(readonly)
			, description(description)
			, items(items) {}
		uint start;
		uint size;
		bool readonly;
		QString description;
		QStringList items;
	};

	DMALogger(std::shared_ptr<CoreController> controller, QWidget* parent = nullptr);

signals:

public slots:

private slots:
	void bitFlipped();

private:
	Ui::DMALogger m_ui;

	std::array<QCheckBox*, 5> m_b;

	std::shared_ptr<CoreController> m_controller;
};

}
