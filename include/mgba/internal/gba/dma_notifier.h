#ifndef GBA_DMA_NOTIFIER_H
#define GBA_DMA_NOTIFIER_H

#include <mgba-util/common.h>

CXX_GUARD_START

struct GBADMANotifier
{
	void (*callback)(const struct GBA* gba, int channel, const struct GBADMA* dma);
	int enable_lcd;
	int enable_palette;
	int enable_vram;
	int enable_attr;
	int enable_ram;
};

struct GBADMANotifierEntry
{
	struct GBADMA dma;
	uint32_t pc;
};

void DMANotifierCallback(const struct GBA* gba, int channel, const struct GBADMA* dma);

CXX_GUARD_END

#endif
