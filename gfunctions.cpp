/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "gfunctions.h"

unsigned int __attribute__((aligned(16))) dList[262144];	
void *fbp0;	                // frame buffer
#ifdef ANAGLYPH
void *copyImg; //for anaglyph
bool leye;
#endif


#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

float zoom = DEFAUT_FONT_SIZE;

void InitGU( void )
{
	fbp0 = 0;
	#ifdef ANAGLYPH
	copyImg = malloc(BUF_WIDTH*SCR_HEIGHT*4);
	leye = true;
	#endif
	
	// Init GU
	sceGuInit();
	sceGuStart( GU_DIRECT, dList );
 
	// Set Buffers
	sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
	sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer( (void*)0x110000, BUF_WIDTH);
 
	sceGuOffset( 2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport( 2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange( 65535, 0);
 
	// Set Render States
	sceGuScissor( 0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable( GU_SCISSOR_TEST );
	sceGuFrontFace( GU_CW );
	sceGuEnable( GU_CULL_FACE );					
	sceGuShadeModel( GU_SMOOTH );
	sceGuEnable( GU_CLIP_PLANES );
	sceGuEnable( GU_BLEND );

	
	sceGuTexMode( GU_PSM_8888, 0, 0, 0 );
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter( GU_LINEAR, GU_LINEAR );	
	sceGuTexScale( 1.0f, 1.0f );			// No scaling
	sceGuTexOffset( 0.0f, 0.0f );

 
	// Blending Function to perform
sceGuEnable(GU_BLEND);
sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	//sceGuBlendFunc( GU_ADD, GU_FIX, GU_FIX, 0xffffffff, 0x00ffffff );
 
	sceGuFinish();
	sceGuSync(0,0);
 
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
	
	// finish
}unsigned int __attribute__((aligned(16))) list[262144];

void texMode(){
	sceGuTexMode( GU_PSM_8888, 0, 0, 0 );
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter( GU_NEAREST, GU_NEAREST );	// No Image filtering
	sceGuTexScale( 1.0f, 1.0f );			// No scaling
	sceGuTexOffset( 0.0f, 0.0f );
}

void perspectiveView(){
	sceGumLoadIdentity();
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective( 55.0f, 16.0f/9.0f, 0.5f, 1000.0f);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	ScePspFVector3 move = { 0.0f, -1.0f, -3.0f };
	sceGumTranslate( &move );
	#ifdef ANAGLYPH
	if(!leye){
		ScePspFVector3 move2 = { 0.05f, 0.0f, -0.0f };
		sceGumTranslate( &move2 );
	}
	#endif
	sceGumRotateX(-3.14f/3.5f);
	
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}

void orthoView(){
	sceGumLoadIdentity();
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(0,480,0,272,-10,10);
 
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

}
 
void SetupProjection( void )
{
	perspectiveView();
 
	sceGuClearColor( GU_COLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );  // Color to clear screen with
	sceGuClearDepth(0);
}

void startRender(){
	fbp0 = sceGuSwapBuffers();
	sceGuStart( GU_DIRECT, dList );
    
            	// clear screen
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);	// Clears the Color and Depth Buffer
	sceGumMatrixMode(GU_MODEL);				// Selects the Model Matrix
	sceGuCopyImage(GU_PSM_8888,0,0,480,272,512,fond->data,0,0,512,(void*)(0x04000000+(u32)fbp0));
	
	sceGuTexFilter( GU_LINEAR, GU_LINEAR );	
	
	#ifdef ANAGLYPH
	leye = true;
	#endif
}
#ifdef ANAGLYPH
void copyImage(){
	sceGuFinish();
	sceGuSync(0,0);
	
	memcpy(copyImg, (void*)(0x04000000+(u32)fbp0), BUF_WIDTH*SCR_HEIGHT*4);
	
	sceGuStart( GU_DIRECT, dList );
}

void putRed(){
	for(int i = 0; i < (SCR_WIDTH*4); i+= 4){
		for(int j = 0; j<SCR_HEIGHT; j++){
			( (char*) (0x04000000+(u32)fbp0))[i + BUF_WIDTH*j*4] = ( (char*) copyImg)[i + BUF_WIDTH*j*4];
		}
	}
	
	perspectiveView();
}

void clearRender(){
            	// clear screen
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);	// Clears the Color and Depth Buffer
	sceGumMatrixMode(GU_MODEL);				// Selects the Model Matrix
	sceGuCopyImage(GU_PSM_8888,0,0,480,272,512,fond->data,0,0,512,(void*)(0x04000000+(u32)fbp0));
	
	leye = false;
}
#endif
void stopRender(){
	/*u32* data = (u32*)(0x04000000+(u32)fbp0);
	int x,y,z;
	u8* u;
	data++;
	for(y = 0; y<272 ; y++, data += 512-478){
		for(x = 0; x<478 ; x++, data++){
			u = (u8*)data;
			for(z = 0; z<2; z++ , u++){
				*u = (*(u-4) + *u*3 + *(u+4))/5;
			}
		}
	}*/
	
	/*{orthoView();
	
	sceGumLoadIdentity();
	sceGuEnable(GU_TEXTURE_2D);
	sceGuColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 0.2f ));
	sceGuTexImage( 0, 512, 256, 512, (void*)(0x04000000+(u32)fbp0));

	ScePspFVector3 move = { 1.0f, 16.0f, 0.0f };
	sceGumTranslate( &move );
	ScePspFVector3 scale = { 512, 256, 1.0f };
	sceGumScale(&scale);

	sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
				3*2, 0, fireSquare );
	sceGuDisable(GU_TEXTURE_2D);
	perspectiveView();}

	{orthoView();
	
	sceGumLoadIdentity();
	sceGuEnable(GU_TEXTURE_2D);
	sceGuColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 0.2f ));
	sceGuTexImage( 0, 512, 256, 512, (void*)(0x04000000+(u32)fbp0));

	ScePspFVector3 move = { -1.0f, 16.0f, 0.0f };
	sceGumTranslate( &move );
	ScePspFVector3 scale = { 512, 256, 1.0f };
	sceGumScale(&scale);

	sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
				3*2, 0, fireSquare );
	sceGuDisable(GU_TEXTURE_2D);
	perspectiveView();}

	{orthoView();
	
	sceGumLoadIdentity();
	sceGuEnable(GU_TEXTURE_2D);
	sceGuColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 0.2f ));
	sceGuTexImage( 0, 512, 256, 512, (void*)(0x04000000+(u32)fbp0));

	ScePspFVector3 move = { 0.0f, 15.0f, 0.0f };
	sceGumTranslate( &move );
	ScePspFVector3 scale = { 512, 256, 1.0f };
	sceGumScale(&scale);

	sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
				3*2, 0, fireSquare );
	sceGuDisable(GU_TEXTURE_2D);
	perspectiveView();}

	{orthoView();
	
	sceGumLoadIdentity();
	sceGuEnable(GU_TEXTURE_2D);
	sceGuColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 0.2f ));
	sceGuTexImage( 0, 512, 256, 512, (void*)(0x04000000+(u32)fbp0));

	ScePspFVector3 move = { 0.0f, 17.0f, 0.0f };
	sceGumTranslate( &move );
	ScePspFVector3 scale = { 512, 256, 1.0f };
	sceGumScale(&scale);

	sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
				3*2, 0, fireSquare );
	sceGuDisable(GU_TEXTURE_2D);
	perspectiveView();}*/
	
	
	sceGuFinish();
	sceGuSync(0,0);
}

void imageRender(Image* image, int x, int y, int width, int height, unsigned int color){
		orthoView();
		
		sceGumLoadIdentity();
		sceGuEnable(GU_TEXTURE_2D);
		sceGuColor( color );
		sceGuTexImage( 0, image->textureWidth, image->textureHeight, image->textureWidth,(void*)image->data );

		ScePspFVector3 move = { x, y, 0.0f };
		sceGumTranslate( &move );
		ScePspFVector3 scale = { width, height, 1.0f };
		sceGumScale(&scale);
		
		sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
					3*2, 0, fireSquare );
		sceGuDisable(GU_TEXTURE_2D);
		perspectiveView();
}

void imageRender(Image* image, int x, int y, int width, int height){
	imageRender(image, x, y, width, height, GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

void imageRender(Image* image, int x, int y){
	imageRender(image, x, y + image->imageHeight - image->textureHeight, image->textureWidth, image->textureHeight, GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
}


void imageRender(Image* image, int x, int y, unsigned int color){
	imageRender(image, x, y + image->imageHeight - image->textureHeight, image->textureWidth, image->textureHeight, color);
}

void setFontZoom(float z){
	zoom = z*DEFAUT_FONT_SIZE;
}

void fontPrint(intraFont *font, float x, float y, bool hightLight, const char *text){
	unsigned int centerColor;
	if(hightLight){
		intraFontSetStyle(font, zoom, GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ), 0, 0);
	}else{
		intraFontSetStyle(font, zoom, GU_COLOR( 1.0f, 1.0f, 1.0f, 0.7f ), 0, 0);
	}
	intraFontPrint(font, x,y, text);
	sceGuDisable(GU_TEXTURE_2D);
	texMode();
}
void fontPrintf(intraFont *font, float x, float y, bool hightLight, const char *text, ...){
	if(!font)
		return ;

	char buffer[256];
	va_list ap;
	
	va_start(ap, text);
	vsnprintf(buffer, 256, text, ap);
	va_end(ap);
	buffer[255] = 0;
	
	fontPrint(font, x, y, hightLight, buffer);
}
void fontPrintRC(intraFont *font, float x, float y,float xmax,float yjump, bool hightLight, const char *text){
	char buff[128];
	int pos = 0;
	for(int i = 0; text[i] != 0; i++){
		buff[pos] = text[i];
		buff[pos+1] = 0;
		pos++;
		
		if(intraFontMeasureText(font,buff)>xmax){
			buff[pos] = 0;
			
			for(int j = 0; j<15 ;j++){
				if(text[i-j] == ' '){
					buff[pos-j] = 0;
					i -= j;
					break;
				}
			}
			
			fontPrint(font, x, y, hightLight, buff);
			y += yjump;
			pos = 0;
		}else if(text[i+1] == 0){
			fontPrint(font, x, y, hightLight, buff);
		}
	}
}

void printRC(const char *text){
	fontPrintRC(font, 10,30,460,18, 1,text);
}

void printRCf(const char *text, ...){
	char buffer[256];
	va_list ap;
	
	va_start(ap, text);
	vsnprintf(buffer, 256, text, ap);
	va_end(ap);
	buffer[255] = 0;
	
	printRC(buffer);
}

float textMotion;
float ysize;

void resetMotionMenu(){
	textMotion = 0.0f;
}

void motionUp(){
	textMotion -= ysize + FONT_SPACE;
}

void motionDown(){
	textMotion += ysize + FONT_SPACE;
}

/*int getNbLines(char* txt){
	int nblines = 1;
	for(int i = 0; txt[i] != '\0'; i++){
		if(txt[i] == '\n') nblines++;
	}
	return nblines;
}*/

void printMenuEntry(float basea, float baseb, int row, char* txt){
	if(row < 0){
		fontPrintf(font, 10,basea + (row+1)*(FONT_SPACE+ysize), false, txt);
	}else if( row == 0){
		setFontZoom(FONT_HIGHLIGHT_ZOOM);
		fontPrintf(font, 10,baseb + row*(FONT_SPACE+ysize), true, txt);
		setFontZoom(1.0f);
	}else{
		fontPrintf(font, 10,baseb + row*(FONT_SPACE+ysize), false, txt);
	}
}

void printMenu(std::vector<char*> menu,int index){
	textMotion *= TEXT_MOTION_VELOCITY;

	ysize = /*getNbLines(menu[0])**/FONT_HEIGHT;
	float basea = 136 - FONT_HIGHLIGHT_ZOOM*ysize/2 - FONT_SPACE + textMotion;
	float baseb = 136 + FONT_HIGHLIGHT_ZOOM*ysize/2 + textMotion;
	
	int i = 0;
	for(std::vector<char*>::iterator it = menu.begin(); it < menu.end(); it++){
		char* txt = *it;
		printMenuEntry(basea, baseb, i-index, txt);
		i++;
	}
}

void printMenuF(FolderEntry* menu){
	if(menu->list->empty()){
		fontPrintf(font, 10,FONT_HEIGHT, true, /*"dossier vide"*/ "empty folder");
		return;
	}
	int index = menu->selection;
	textMotion *= TEXT_MOTION_VELOCITY;

	ysize = 2*FONT_HEIGHT + FONT_SPACE;
	float basea = 136 - FONT_HIGHLIGHT_ZOOM*ysize/2 - FONT_SPACE + textMotion;
	float baseb = 136 + FONT_HIGHLIGHT_ZOOM*ysize/2 + textMotion;
	
	char tmp1[128];
	char tmp2[128];
	for(int i = ((index-10)>0 ? (index-10) : 0); (i<(index+10)) && (i<menu->list->size()); i++){
		if(menu->list->at(i)->type == FOLDER_ENTRY){
			FolderEntry* fentry = (FolderEntry*) menu->list->at(i);
			sprintf(tmp1,"%s",fentry->name);
			sprintf(tmp2,/*"dossier"*/ "folder");
		}else if(menu->list->at(i)->type == MUSIC_ENTRY){
			MusicEntry* mentry = (MusicEntry*) menu->list->at(i);
			sprintf(tmp1, "%s",mentry->name);
			sprintf(tmp2, "%s",mentry->artist);
		}
		
		if(i-index < 0){
			fontPrintf(font, 10,basea + (i-index+1)*(FONT_SPACE+ysize) - FONT_HEIGHT - FONT_SPACE, false, tmp1);
			fontPrintf(font, 10,basea + (i-index+1)*(FONT_SPACE+ysize), false, tmp2);
		}else if( i == index){
			setFontZoom(FONT_HIGHLIGHT_ZOOM);
			fontPrintf(font, 10,baseb - FONT_HIGHLIGHT_ZOOM*FONT_HEIGHT - FONT_SPACE, true, tmp1);
			fontPrintf(font, 10,baseb, true, tmp2);
			setFontZoom(1.0f);
		}else{
			fontPrintf(font, 10,baseb + (i-index)*(FONT_SPACE+ysize)- FONT_HEIGHT - FONT_SPACE, false, tmp1);
			fontPrintf(font, 10,baseb + (i-index)*(FONT_SPACE+ysize), false, tmp2);
		}
	}
}

//---------------- taken from graphics.cpp -------------------

static int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
}

Image* loadImage(const char* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;
	Image* image = (Image*) malloc(sizeof(Image));
	if (!image) return NULL;

	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		free(image);
		fclose(fp);
		return NULL;;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, (int *) NULL, (int *) NULL);
	if (width > 512 || height > 512) {
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return NULL;
	}
	image->imageWidth = width;
	image->imageHeight = height;
	image->textureWidth = getNextPower2(width);
	image->textureHeight = getNextPower2(height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	image->data = (Color*) memalign(16, image->textureWidth * image->textureHeight * sizeof(Color));
	//Color* imgData = (Color*) memalign(16, image->textureWidth * image->textureHeight * sizeof(Color));
	if (!image->data) {
	//if (!imgData) {
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return NULL;
	}
	line = (u32*) malloc(width * 4);
	if (!line) {
		free(image->data);
		//free(imgData);
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return NULL;
	}
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, (png_bytep)NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			image->data[x + y * image->textureWidth] =  color;
			//imgData[x + y * image->textureWidth] =  color;
		}
	}
	free(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	fclose(fp);
	
	//swizzle
	/*image->data = (Color*) memalign(16, image->textureWidth * image->textureHeight * sizeof(Color));
	swizzle_fast((u8*) image->data, (u8*) imgData, image->textureWidth, image->textureHeight);
	free(imgData);*/
	
	return image;
}

void freeImage(Image* image)
{
	free(image->data);
	free(image);
}

void bindTexure(Image* image){
	sceGuTexImage( 0, image->textureWidth, image->textureHeight, image->textureWidth,(void*)image->data );
}
//--------------------------------------------------------------

//from http://wiki.ps2dev.org/psp:ge_faq
void swizzle_fast(u8* out, const u8* in, unsigned int width, unsigned int height)
{
   unsigned int blockx, blocky;
   unsigned int i,j;
 
   unsigned int width_blocks = (width / 16);
   unsigned int height_blocks = (height / 8);
 
   unsigned int src_pitch = (width-16)/4;
   unsigned int src_row = width * 8;
 
   const u8* ysrc = in;
   u32* dst = (u32*)out;
 
   for (blocky = 0; blocky < height_blocks; ++blocky)
   {
      const u8* xsrc = ysrc;
      for (blockx = 0; blockx < width_blocks; ++blockx)
      {
         const u32* src = (u32*)xsrc;
         for (j = 0; j < 8; ++j)
         {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src += src_pitch;
         }
         xsrc += 16;
     }
     ysrc += src_row;
   }
}
