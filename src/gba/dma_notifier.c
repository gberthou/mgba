#include <mgba/internal/gba/dma.h>
#include <mgba/internal/gba/dma_notifier.h>

#include <mgba/internal/gba/gba.h>
#include <mgba/internal/gba/io.h>

const uint32_t LCD_BEG = 0x04000000;
const uint32_t LCD_END = 0x04000057;
const uint32_t PAL_BEG = 0x05000000;
const uint32_t PAL_END = 0x050003ff;
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

void DMANotifierCallback(const struct GBA* gba, int channel, const struct GBADMA* dma)
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

    if(mustRecord)
        printf("%08x~%08x\n", destBegin, destEnd);
}

