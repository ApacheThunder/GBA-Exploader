#ifndef SC_SRAM_H
#define SC_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/ndstypes.h>

extern u32 romSize;

bool flash_patchV120(const struct save_type* type);
bool flash_patchV123(const struct save_type* type);
bool flash_patchV126(const struct save_type* type);
bool flash_patch512V130(const struct save_type* type);
bool flash_patch1MV102(const struct save_type* type);
bool flash_patch1MV103(const struct save_type* type);

bool eeprom_patchV111(const struct save_type* type);
bool eeprom_patchV120(const struct save_type* type);
bool eeprom_patchV124(const struct save_type* type);
bool eeprom_patchV126(const struct save_type* type);

const struct save_type* save_findTag();


#ifdef __cplusplus
}
#endif
#endif

