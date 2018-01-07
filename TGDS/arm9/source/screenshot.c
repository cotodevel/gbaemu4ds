#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "screenshot.h"
#include "GBA.h"
#include "posixHandleTGDS.h"

int ofs=0;
u8 *pbuf;
volatile u8 buf[ScreenWidth*ScreenHeight*3];

//screenshot functions from moonshell2
u8* CreateBMPImage()
{
  ofs = 0;
  
  u32 linelen;
  int size = ScreenHeight*ScreenWidth*3;
  linelen=ScreenWidth*3;
  linelen=(linelen+3)&(~3);
  
  u32 bufsize=14+40+(ScreenHeight*linelen);
  
  
  //u8 *pbuf=(u8*)malloc(bufsize);
  pbuf=(u8*)&buf[0];
  
  if(pbuf==NULL){
    printf("memory overflow!! CreateBMPImage. malloc(%d)=NULL; ",bufsize);
	while(1);
    return (u8*)0;
  }
  
  u32 ofs=0;
  
  // BITMAPFILEHEADER
  
  // bfType 2 byte ファイルタイプ 'BM' - OS/2, Windows Bitmap
  add8((u8)'B');
  add8((u8)'M');
  // bfSize 4 byte ファイルサイズ (byte)
  add32(bufsize);
  // bfReserved1 2 byte 予約領域 常に 0
  add16(0);
  // bfReserved2 2 byte 予約領域 常に 0
  add16(0);
  // bfOffBits 4 byte ファイル先頭から画像データまでのオフセット (byte)
  add32(14+40);
  
  // BITMAPINFOHEADER
  
  // biSize 4 byte 情報ヘッダのサイズ (byte) 40
  add32(40);
  // biWidth 4 byte 画像の幅 (ピクセル)
  add32(ScreenWidth);
  // biHeight 4 byte 画像の高さ (ピクセル) biHeight の値が正数なら，画像データは下から上へ
  add32(ScreenHeight);
  // biPlanes 2 byte プレーン数 常に 1
  add16(1);
  // biBitCount 2 byte 1 画素あたりのデータサイズ (bit)
  add16(24);
  // biCopmression 4 byte 圧縮形式 0 - BI_RGB (無圧縮)
  add32(0);
  // biSizeImage 4 byte 画像データ部のサイズ (byte) 96dpi ならば3780
  add32(0);
  // biXPixPerMeter 4 byte 横方向解像度 (1mあたりの画素数) 96dpi ならば3780
  add32(0);
  // biYPixPerMeter 4 byte 縦方向解像度 (1mあたりの画素数) 96dpi ならば3780
  add32(0);
  // biClrUsed 4 byte 格納されているパレット数 (使用色数) 0 の場合もある
  add32(0);
  // biCirImportant 4 byte 重要なパレットのインデックス 0 の場合もある
  add32(0);
  
  int y=0;
  u16 * vram_src = (u8*)0x06010000;
  for(y=ScreenHeight-1;0<=y;y--){
    u16 * psrcbm=/*&VRAMBuf*/(u16 *)vram_src[y*ScreenWidth];
    int x = 0;
	for(x=0;x<ScreenWidth;x++){
      u16 col=*psrcbm++;
      u8 b=(col>>10)&0x1f;
      u8 g=(col>>5)&0x1f;
      u8 r=(col>>0)&0x1f;
      add8(b<<3);
      add8(g<<3);
      add8(r<<3);
    }
	
	for(x=0;x<(linelen-(ScreenWidth*3));x++){
      add8(0);
    }
  }
  
  
  writebuf2file("fat:/arm7.bmp",(u8*)&buf[0],sizeof(buf));
  
  return(pbuf);
}


int writebuf2file(char * filename,u8 * buf,int size){
    
	//w+
	FILE * fh_dump = fopen(filename,"w+");
	int sizewritten=fwrite((u8*)buf, 1, size, fh_dump);
	
	if(sizewritten > 0)
		printf("write ok!  ");
	else{
		printf("write was 0.. :(");
	}
	
	fclose(fh_dump);
    
    return sizewritten;
}