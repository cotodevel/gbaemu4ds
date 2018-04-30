#ifdef __cplusplus
extern "C" {
#endif

extern u32 arm7_held_buttons;


extern void HblankHandler_gba(void);
extern void HblankHandler_nds(void);

extern void VblankHandler_nds(void);
extern void VblankHandler_gba(void);

#ifdef __cplusplus
}
#endif
