#pragma once

#include <QDialog>
#include <QTimer>

#include <memory>
#include <mgba/internal/gba/dma_notifier.h>

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

	void addEntry(const struct GBADMANotifierEntry &entry);

signals:

public slots:

private slots:
	void bitFlipped();
	void updateTable();

private:
	Ui::DMALogger m_ui;

	std::array<QCheckBox*, 5> m_b;
	QTableWidget* m_table;
	QTimer m_updateTimer;
	size_t m_lastSize;

	std::shared_ptr<CoreController> m_controller;
};

}
