PSPDIR=$(shell psp-config --psp-prefix)

TARGET = main
OBJS = main.o framebuffer.o intraFont.o gfunctions.o data.o midiloader.o callback.o shapes.o player.o oggplayer.o config.o pad.o webbrowser.o \
       jdkmidisrc/jdkmidi_driver.o\
       jdkmidisrc/jdkmidi_driverdump.o\
       jdkmidisrc/jdkmidi_driverwin32.o\
       jdkmidisrc/jdkmidi_edittrack.o\
       jdkmidisrc/jdkmidi_file.o\
       jdkmidisrc/jdkmidi_fileread.o\
       jdkmidisrc/jdkmidi_filereadmultitrack.o\
       jdkmidisrc/jdkmidi_fileshow.o\
       jdkmidisrc/jdkmidi_filewrite.o\
       jdkmidisrc/jdkmidi_filewritemultitrack.o\
       jdkmidisrc/jdkmidi_keysig.o\
       jdkmidisrc/jdkmidi_manager.o\
       jdkmidisrc/jdkmidi_matrix.o\
       jdkmidisrc/jdkmidi_midi.o\
       jdkmidisrc/jdkmidi_msg.o\
       jdkmidisrc/jdkmidi_multitrack.o\
       jdkmidisrc/jdkmidi_parser.o\
       jdkmidisrc/jdkmidi_process.o\
       jdkmidisrc/jdkmidi_queue.o\
       jdkmidisrc/jdkmidi_sequencer.o\
       jdkmidisrc/jdkmidi_showcontrol.o\
       jdkmidisrc/jdkmidi_showcontrolhandler.o\
       jdkmidisrc/jdkmidi_smpte.o\
       jdkmidisrc/jdkmidi_sysex.o\
       jdkmidisrc/jdkmidi_tempo.o\
       jdkmidisrc/jdkmidi_tick.o\
       jdkmidisrc/jdkmidi_track.o\
       matchingWifi/adhoc.o server.o\

INCDIR = /usr/include/freetype2
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -DDEBUG #-DAD_HOC -DANAGLYPH
ASFLAGS = $(CFLAGS)

LIBDIR = 
LIBS = -lmad -lvorbisidec -lpng -lz \
		-lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lm \
		-lpspmpeg -lpspaudiocodec -lstdc++ -lmikmod -lfreetype \
		-lpspnet_adhocctl -lpspnet_adhoc -lpspnet_adhocmatching -lpspwlan \
		-lpsputility -lpsphttp -lpspssl -logg
LDFLAGS = 

#pour le navigateur web
#BUILD_PRX = 1

PSP_FW_VERSION = 340 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 
PSP_EBOOT_ICON = ICON0.png
PSP_EBOOT_PIC1 = BACK.png 
#PSP_EBOOT_ICON1 = test.pmf

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak 
