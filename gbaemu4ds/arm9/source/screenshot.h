
#ifndef moonshell2_screenshot
#define moonshell2_screenshot
#define add8(d) { pbuf[ofs]=d; ofs++; }
#define add16(d) { pbuf[ofs+0]=(u8)((d>>0)&0xff); pbuf[ofs+1]=(u8)((d>>8)&0xff); ofs+=2; }
#define add32(d) { pbuf[ofs+0]=(u8)((d>>0)&0xff); pbuf[ofs+1]=(u8)((d>>8)&0xff); pbuf[ofs+2]=(u8)((d>>16)&0xff); pbuf[ofs+3]=(u8)((d>>24)&0xff); ofs+=4; }

#define ScreenWidth (256)
#define ScreenHeight (192)

#endif
#ifdef __cplusplus
extern "C"{
#endif

extern u8 *pbuf;
extern volatile u8 buf[ScreenWidth*ScreenHeight*3];
extern int ofs;
extern u8* CreateBMPImage();
extern int writebuf2file(char * filename,u8 * buf,int size);

#ifdef __cplusplus
}
#endif
