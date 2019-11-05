/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	2048	/* Must be an integer multiple of 4. */
/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

OggVorbis_File vf;
int eof = 0;
int current_section;
char **oggComments;
vorbis_info *vi;
//int errno;			// __errno;
static int isPlaying;		// Set to true when a mod is being played
static int myChannel;
int fd = 0;

static int eos;

/*
#define PREBUFFER	2097152
static short outputBuffer[PREBUFFER];
u32 outputBufferLevel;
*/
//void OGGsetStubs(codecStubs * stubs)
//{
   // stubs->init = OGG_Init;
  //  stubs->load = OGG_Load;
  //  stubs->play = OGG_Play;
  //  stubs->pause = OGG_Pause;
  //  stubs->stop = OGG_Stop;
  //  stubs->end = OGG_End;
  //  stubs->time = OGG_GetTimeString;
  //  stubs->tick = NULL;
  //  stubs->eos = OGG_EndOfStream;
  //  memcpy(stubs->extension, "ogg\0" "\0\0\0\0", 2 * 4);
//}

int i;
bool mustAccelerate;
static void OGGCallback(void *_buf2, unsigned int numSamples, void *pdata)
{	
	int saut = 0;
	if(mustAccelerate){
		for(int j = 0; j<numSamples; j++){
			i++;
			if(i == 25){
				i = 0;
				saut++;
			}
		}
	}

    short *_buf = (short *)_buf2;
    static short tempmixbuf[PSP_NUM_AUDIO_SAMPLES * 2 * 2] __attribute__ ((aligned(64)));
    static unsigned long tempmixleft = 0;

    if (isPlaying == true) {	// Playing , so mix up a buffer
	while (tempmixleft < (numSamples+saut)) {	//  Not enough in buffer, so we must mix more
	    unsigned long bytesRequired = ((numSamples+saut) - tempmixleft) * 4;	// 2channels, 16bit = 4 bytes per sample
	    unsigned long ret = ov_read(&vf, (char *) &tempmixbuf[tempmixleft * 2], bytesRequired, &current_section);
	    if (ret == 0) {	//EOF
		eos = 1;
		if (ov_pcm_seek_page(&vf, 0) != 0) {
		    //printf("Could not seek to start of file\n");
		    OGG_End();
		}
		return;
	    } else if (ret < 0) {
		//printf("Error occured during ov_read: %d\n", ret);
		sceKernelDelayThread(500000);
		return;
	    }
	    tempmixleft += ret / 4;	// back down to sample num
	}
	if (tempmixleft >= (numSamples+saut)) {	//  Buffer has enough, so copy across
	    unsigned int count, count2;
	    short *_buf2;
	    for (count = 0; count < numSamples; count++) {
		count2 = count + count;
		_buf2 = _buf + count2;
		// Double up for stereo
		*(_buf2) = tempmixbuf[count2];
		*(_buf2 + 1) = tempmixbuf[count2 + 1];
	    }
	    //  Move the pointers
	    tempmixleft -= numSamples+saut;
	    //  Now shuffle the buffer along
	    for (count = 0; count < tempmixleft; count++)
		tempmixbuf[count] = tempmixbuf[numSamples+saut + count];
	}

    } else {			//  Not Playing , so clear buffer
	unsigned int count;
	for (count = 0; count < numSamples * 2; count++)
	    *(_buf + count) = 0;
    }
}

//////////////////////////////////////////////////////////////////////
// Functions - Local and not public
//////////////////////////////////////////////////////////////////////

size_t ogg_callback_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    return sceIoRead(*(int *) datasource, ptr, size * nmemb);
}
int ogg_callback_seek(void *datasource, ogg_int64_t offset, int whence)
{
    return sceIoLseek32(*(int *) datasource, (unsigned int) offset, whence);
}
long ogg_callback_tell(void *datasource)
{
    return sceIoLseek32(*(int *) datasource, 0, SEEK_CUR);
}
int ogg_callback_close(void *datasource)
{
    return sceIoClose(*(int *) datasource);
}

bool OGG_Load(char *filename)
{
    eos = 0;
    isPlaying = 0;
    ov_callbacks ogg_callbacks;

    ogg_callbacks.read_func = ogg_callback_read;
    ogg_callbacks.seek_func = ogg_callback_seek;
    ogg_callbacks.close_func = ogg_callback_close;
    ogg_callbacks.tell_func = ogg_callback_tell;

    if ((fd = sceIoOpen(filename, PSP_O_RDONLY, 0777)) <= 0) {
	//printf("could not open file %s\n", filename);
	//sceDisplayWaitVblankStart();
	//sceDisplayWaitVblankStart();
	//sceKernelDelayThread(500000);
	return false;
    }
    if (ov_open_callbacks(&fd, &vf, NULL, 0, ogg_callbacks) < 0) {
	//printf("Input does not appear to be an Ogg bitstream.\n");
	//sceKernelDelayThread(500000);
	return false;
    } else {
	oggComments = ov_comment(&vf, -1)->user_comments;
	vi = ov_info(&vf, -1);
	pspDebugScreenSetXY(0, 29);
/*	printf("%d channel, %lu kb/s %s OGG Vorbis audio stream at %ldHz\n", vi->channels, vi->bitrate_nominal / 1000,
	       vi->bitrate_upper == vi->bitrate_nominal ? "CBR" : "VBR", vi->rate); */
	//printf("Encoded by: %s\n", ov_comment(&vf, -1)->vendor);
    }
    return true;
}

void OGG_Init(int channel)
{
	i = 0;
	mustAccelerate = false;
    myChannel = channel;
    isPlaying = false;
    pspAudioSetChannelCallback(myChannel, OGGCallback,0);
}

void accelerate(){
	mustAccelerate = true;
}

void stopAccelerate(){
	mustAccelerate = false;
}

// This function initialises for playing, and starts
bool OGG_Play()
{
    // See if I'm already playing
    if (isPlaying)
	return false;

    isPlaying = true;
    return true;
}

void OGG_Stop()
{
    //stop playing
    isPlaying = false;
}

void OGG_Pause()
{
    isPlaying = !isPlaying;
}

void OGG_FreeTune()
{
    ov_clear(&vf);
    if (fd)
	sceIoClose(fd);
}

void OGG_End()
{
    OGG_Stop();
    pspAudioSetChannelCallback(myChannel, 0,0);
    OGG_FreeTune();
}

unsigned int OGG_GetTime()
{
//extern ogg_int64_t ov_time_tell(OggVorbis_File *vf);
    unsigned int time = (unsigned int) ov_time_tell(&vf);
	return time;
}

bool OGG_EndOfStream()
{
    if (eos == 1)
	return 1;
    return 0;
}
