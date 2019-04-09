#ifndef GBA_DMA_NOTIFIER_H
#define GBA_DMA_NOTIFIER_H

#include <mgba-util/common.h>
#include <mgba-util/vector.h>
#include <mgba/internal/gba/dma.h>

CXX_GUARD_START

struct GBADMANotifierEntry;
DECLARE_VECTOR(GBADMAList, struct GBADMANotifierEntry)

struct GBADMANotifier
{
	void (*callback)(struct GBA* gba, int channel, struct GBADMA* dma);
	int enable_lcd;
	int enable_palette;
	int enable_vram;
	int enable_attr;
	int enable_ram;
	struct GBADMAList entries;
};

struct GBADMANotifierEntry
{
	struct GBADMA dma;
	uint32_t pc;
	uint32_t actualSize;
};

void DMANotifierCallback(struct GBA* gba, int channel, struct GBADMA* dma);

CXX_GUARD_END

#endif
