#include <iostream>
#include <iomanip>

#include "DMALogger.h"

#include "CoreController.h"

#include <QComboBox>
#include <QFontDatabase>
#include <QGridLayout>
#include <QSpinBox>

#include <mgba/internal/gba/gba.h>
#include <mgba/internal/gba/io.h>
#include <mgba/internal/gba/memory.h>

struct ARMCore;

using namespace QGBA;

enum ButtonIndex
{
	BTN_LCD = 0,
	BTN_PAL = 1,
	BTN_VRAM = 2,
	BTN_ATTR = 3,
	BTN_RAM = 4
};

static QString u32ToQString(uint32_t x)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(8) << std::hex << x;
	return oss.str().c_str();
}

DMALogger::DMALogger(std::shared_ptr<CoreController> controller, QWidget* parent)
	: QDialog(parent)
	, m_controller(controller)
	, m_updateTimer(this)
	, m_lastSize(0)
{
	const struct GBADMANotifier *dmaNotifier = &static_cast<const GBA*>(m_controller->thread()->core->board)->dmaNotifier;

	m_ui.setupUi(this);

	m_b[BTN_LCD] = m_ui.b_lcd;
	m_b[BTN_LCD]->setCheckState(dmaNotifier->enable_lcd ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	m_b[BTN_PAL] = m_ui.b_palette;
	m_b[BTN_PAL]->setCheckState(dmaNotifier->enable_palette ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	m_b[BTN_VRAM] = m_ui.b_vram;
	m_b[BTN_VRAM]->setCheckState(dmaNotifier->enable_vram ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	m_b[BTN_ATTR] = m_ui.b_attr;
	m_b[BTN_ATTR]->setCheckState(dmaNotifier->enable_attr ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	m_b[BTN_RAM] = m_ui.b_ram;
	m_b[BTN_RAM]->setCheckState(dmaNotifier->enable_ram ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	for(size_t i = 0; i < m_b.size(); ++i)
		connect(m_b[i], &QAbstractButton::toggled, this, &DMALogger::bitFlipped);

	m_table = m_ui.t_log;

	connect(m_ui.buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
	connect(controller.get(), &CoreController::stopping, this, &QWidget::close);

	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
	m_updateTimer.start(100);
}

void DMALogger::addEntry(const struct GBADMANotifierEntry &entry)
{
	m_table->insertRow(m_table->rowCount());
	int row = m_table->rowCount() - 1;
	m_table->setItem(row, 0, new QTableWidgetItem(u32ToQString(entry.dma.dest)));
	m_table->setItem(row, 1, new QTableWidgetItem(u32ToQString(entry.dma.source)));
	m_table->setItem(row, 2, new QTableWidgetItem(u32ToQString(entry.actualSize)));
	m_table->setItem(row, 3, new QTableWidgetItem(u32ToQString(entry.pc)));
}

void DMALogger::bitFlipped()
{
	struct GBADMANotifier *dmaNotifier = &static_cast<GBA*>(m_controller->thread()->core->board)->dmaNotifier;
	dmaNotifier->enable_lcd = (m_b[BTN_LCD]->checkState() == Qt::CheckState::Checked ? 1 : 0);
	dmaNotifier->enable_palette = (m_b[BTN_PAL]->checkState() == Qt::CheckState::Checked ? 1 : 0);
	dmaNotifier->enable_vram = (m_b[BTN_VRAM]->checkState() == Qt::CheckState::Checked ? 1 : 0);
	dmaNotifier->enable_attr = (m_b[BTN_ATTR]->checkState() == Qt::CheckState::Checked ? 1 : 0);
	dmaNotifier->enable_ram = (m_b[BTN_RAM]->checkState() == Qt::CheckState::Checked ? 1 : 0);
}

void DMALogger::updateTable()
{
	struct GBADMANotifier *dmaNotifier = &static_cast<GBA*>(m_controller->thread()->core->board)->dmaNotifier;
	size_t size = GBADMAListSize(&dmaNotifier->entries);
	for(size_t i = m_lastSize; i < size; ++i)
		addEntry(*GBADMAListGetConstPointer(&dmaNotifier->entries, i));
	m_lastSize = size;
}

