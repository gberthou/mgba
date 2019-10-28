#include <mgba/internal/gba/dma.h>
#include <mgba/internal/gba/dma_notifier.h>

#include <mgba/internal/gba/gba.h>
#include <mgba/internal/gba/io.h>

DEFINE_VECTOR(GBADMAList, struct GBADMANotifierEntry)

const uint32_t LCD_BEG  = 0x04000000;
const uint32_t LCD_END  = 0x04000057;
const uint32_t PAL_BEG  = 0x05000000;
const uint32_t PAL_END  = 0x050003ff;
const uint32_t VRAM_BEG = 0x06000000;
const uint32_t VRAM_END = 0x06017fff;
const uint32_t ATTR_BEG = 0x07000000;
const uint32_t ATTR_END = 0x070003ff;
const uint32_t RAM0_BEG = 0x02000000;
const uint32_t RAM0_END = 0x0203ffff;
const uint32_t RAM1_BEG = 0x03000000;
const uint32_t RAM1_END = 0x03007fff;

static inline int rangesOverlap(uint32_t beg0, uint32_t end0, uint32_t beg1, uint32_t end1)
{
	return !(end0 < beg1 || beg0 > end1);
}

static int sameDMA(const struct GBADMA *a, const struct GBADMA *b)
{
    return a->dest == b->dest
        && a->source == b->source;
}

static int sameEntry(const struct GBADMANotifierEntry *a, const struct GBADMANotifierEntry *b)
{
    return sameDMA(&a->dma, &b->dma)
        && a->pc == b->pc
        && a->actualSize == b->actualSize;
}

static struct GBADMANotifierEntry *search(struct GBADMAList *list, const struct GBADMANotifierEntry *entry)
{
    size_t size = GBADMAListSize(list);
    for(size_t i = 0; i < size; ++i) {
        struct GBADMANotifierEntry *ptr = GBADMAListGetPointer(list, i);
        if(sameEntry(ptr, entry))
            return ptr;
    }

    return NULL;
}

void DMANotifierCallback(struct GBA* gba, int channel, struct GBADMA* dma)
{
	uint32_t destBegin = dma->dest;
	uint32_t width_shift = 1 + GBADMARegisterGetWidth(dma->reg);
	uint32_t size = dma->count << width_shift;
	size = (size != 0 ? size : (channel != 3 ? 0x4000 : 0x10000));
	uint32_t destEnd = destBegin + size;
	int mustRecord = 0;

	if (rangesOverlap(destBegin, destEnd, LCD_BEG, LCD_END)) {
		mustRecord = gba->dmaNotifier.enable_lcd;
	} else if (rangesOverlap(destBegin, destEnd, PAL_BEG, PAL_END)) {
		mustRecord = gba->dmaNotifier.enable_palette;
	} else if (rangesOverlap(destBegin, destEnd, VRAM_BEG, VRAM_END)) {
		mustRecord = gba->dmaNotifier.enable_vram;
	} else if (rangesOverlap(destBegin, destEnd, ATTR_BEG, ATTR_END)) {
		mustRecord = gba->dmaNotifier.enable_attr;
	} else if (rangesOverlap(destBegin, destEnd, RAM0_BEG, RAM0_END)
			|| rangesOverlap(destBegin, destEnd, RAM1_BEG, RAM1_END)) {
		mustRecord = gba->dmaNotifier.enable_ram;
	}

	if(mustRecord) {
		struct GBADMANotifierEntry entry = {
			.dma = *dma,
			.pc = gba->cpu->gprs[ARM_PC],
			.actualSize = size,
            .occurrences = 0
		};

        struct GBADMANotifierEntry *existing = search(&gba->dmaNotifier.entries, &entry);
        if(existing)
            ++existing->occurrences;
        else if(GBADMAListSize(&gba->dmaNotifier.entries) < 1000)
            *GBADMAListAppend(&gba->dmaNotifier.entries) = entry;
	}
}

