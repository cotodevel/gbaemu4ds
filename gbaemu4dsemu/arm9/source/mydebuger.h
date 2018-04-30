#ifdef __cplusplus
extern "C" {
#endif

extern char* seloptionsshowmem [6];
extern u32 userinputval(u32 original_val,u32 bits);
extern void show_gba_mem();

extern char str_dis_buf[0x200]; //512b
extern void show_nds_mem();

extern void show_mem();

#ifdef __cplusplus
}
#endif
