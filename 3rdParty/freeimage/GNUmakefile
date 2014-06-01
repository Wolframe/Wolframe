TOPDIR = ../..

SUBDIRS = Wrapper

PACKAGE_NAME = wolframe
PACKAGE_VERSION = 3.15.4

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-DNO_LCMS \
	$(INCLUDE_FLAGS_LT)

INCLUDE_CFLAGS = \
	$(INCLUDE_CXXFLAGS)

ifeq "$(PLATFORM)" "SUNOS"
INCLUDE_CFLAGS += -D_XOPEN_SOURCE=600
endif

INCLUDE_DIRS = \
	-I. -ISource -ISource/Metadata -ISource/FreeImageToolkit \
	-ISource/LibJPEG -ISource/LibPNG -ISource/LibTIFF4 \
	-ISource/ZLib -ISource/LibOpenJPEG -ISource/OpenEXR \
	-ISource/OpenEXR/Half -ISource/OpenEXR/Iex -ISource/OpenEXR/IlmImf \
	-ISource/OpenEXR/IlmThread -ISource/OpenEXR/Imath \
	-ISource/LibRawLite -ISource/LibRawLite/dcraw -ISource/LibRawLite/internal \
	-ISource/LibRawLite/libraw -ISource/LibRawLite/src

INCLUDE_LDFLAGS = \

INCLUDE_LIBS = \

STATIC_LIB = libfreeimage.a

DYNAMIC_LIB = libfreeimage.so
DYNAMIC_LIB_MAJOR = 3
DYNAMIC_LIB_MINOR = 15
DYNAMIC_LIB_PATCH = 4

CPP_OBJS = \
	Source/FreeImage/BitmapAccess.o \
	Source/FreeImage/ColorLookup.o \
	Source/FreeImage/FreeImage.o \
	Source/FreeImage/FreeImageIO.o \
	Source/FreeImage/GetType.o \
	Source/FreeImage/MemoryIO.o \
	Source/FreeImage/PixelAccess.o \
	Source/FreeImage/J2KHelper.o \
	Source/FreeImage/MNGHelper.o \
	Source/FreeImage/Plugin.o \
	Source/FreeImage/PluginBMP.o \
	Source/FreeImage/PluginCUT.o \
	Source/FreeImage/PluginDDS.o \
	Source/FreeImage/PluginEXR.o \
	Source/FreeImage/PluginG3.o \
	Source/FreeImage/PluginGIF.o \
	Source/FreeImage/PluginHDR.o \
	Source/FreeImage/PluginICO.o \
	Source/FreeImage/PluginIFF.o \
	Source/FreeImage/PluginJ2K.o \
	Source/FreeImage/PluginJNG.o \
	Source/FreeImage/PluginJP2.o \
	Source/FreeImage/PluginJPEG.o \
	Source/FreeImage/PluginKOALA.o \
	Source/FreeImage/PluginMNG.o \
	Source/FreeImage/PluginPCD.o \
	Source/FreeImage/PluginPCX.o \
	Source/FreeImage/PluginPFM.o \
	Source/FreeImage/PluginPICT.o \
	Source/FreeImage/PluginPNG.o \
	Source/FreeImage/PluginPNM.o \
	Source/FreeImage/PluginPSD.o \
	Source/FreeImage/PluginRAS.o \
	Source/FreeImage/PluginRAW.o \
	Source/FreeImage/PluginSGI.o \
	Source/FreeImage/PluginTARGA.o \
	Source/FreeImage/PluginTIFF.o \
	Source/FreeImage/PluginWBMP.o \
	Source/FreeImage/PluginXBM.o \
	Source/FreeImage/PluginXPM.o \
	Source/FreeImage/PSDParser.o \
	Source/FreeImage/TIFFLogLuv.o \
	Source/FreeImage/Conversion.o \
	Source/FreeImage/Conversion16_555.o \
	Source/FreeImage/Conversion16_565.o \
	Source/FreeImage/Conversion24.o \
	Source/FreeImage/Conversion32.o \
	Source/FreeImage/Conversion4.o \
	Source/FreeImage/Conversion8.o \
	Source/FreeImage/ConversionFloat.o \
	Source/FreeImage/ConversionRGB16.o \
	Source/FreeImage/ConversionRGBF.o \
	Source/FreeImage/ConversionType.o \
	Source/FreeImage/ConversionUINT16.o \
	Source/FreeImage/Halftoning.o \
	Source/FreeImage/tmoColorConvert.o \
	Source/FreeImage/tmoDrago03.o \
	Source/FreeImage/tmoFattal02.o \
	Source/FreeImage/tmoReinhard05.o \
	Source/FreeImage/ToneMapping.o \
	Source/FreeImage/NNQuantizer.o \
	Source/FreeImage/WuQuantizer.o \
	Source/DeprecationManager/Deprecated.o \
	Source/DeprecationManager/DeprecationMgr.o \
	Source/FreeImage/CacheFile.o \
	Source/FreeImage/MultiPage.o \
	Source/FreeImage/ZLibInterface.o \
	Source/Metadata/Exif.o \
	Source/Metadata/FIRational.o \
	Source/Metadata/FreeImageTag.o \
	Source/Metadata/IPTC.o \
	Source/Metadata/TagConversion.o \
	Source/Metadata/TagLib.o \
	Source/Metadata/XTIFF.o \
	Source/FreeImageToolkit/Background.o \
	Source/FreeImageToolkit/BSplineRotate.o \
	Source/FreeImageToolkit/Channels.o \
	Source/FreeImageToolkit/ClassicRotate.o \
	Source/FreeImageToolkit/Colors.o \
	Source/FreeImageToolkit/CopyPaste.o \
	Source/FreeImageToolkit/Display.o \
	Source/FreeImageToolkit/Flip.o \
	Source/FreeImageToolkit/JPEGTransform.o \
	Source/FreeImageToolkit/MultigridPoissonSolver.o \
	Source/FreeImageToolkit/Rescale.o \
	Source/FreeImageToolkit/Resize.o \
	Source/OpenEXR/IlmImf/ImfAttribute.o \
	Source/OpenEXR/IlmImf/ImfB44Compressor.o \
	Source/OpenEXR/IlmImf/ImfBoxAttribute.o \
	Source/OpenEXR/IlmImf/ImfChannelList.o \
	Source/OpenEXR/IlmImf/ImfChannelListAttribute.o \
	Source/OpenEXR/IlmImf/ImfChromaticities.o \
	Source/OpenEXR/IlmImf/ImfChromaticitiesAttribute.o \
	Source/OpenEXR/IlmImf/ImfCompressionAttribute.o \
	Source/OpenEXR/IlmImf/ImfCompressor.o \
	Source/OpenEXR/IlmImf/ImfConvert.o \
	Source/OpenEXR/IlmImf/ImfCRgbaFile.o \
	Source/OpenEXR/IlmImf/ImfDoubleAttribute.o \
	Source/OpenEXR/IlmImf/ImfEnvmap.o \
	Source/OpenEXR/IlmImf/ImfEnvmapAttribute.o \
	Source/OpenEXR/IlmImf/ImfFloatAttribute.o \
	Source/OpenEXR/IlmImf/ImfFrameBuffer.o \
	Source/OpenEXR/IlmImf/ImfFramesPerSecond.o \
	Source/OpenEXR/IlmImf/ImfHeader.o \
	Source/OpenEXR/IlmImf/ImfHuf.o \
	Source/OpenEXR/IlmImf/ImfInputFile.o \
	Source/OpenEXR/IlmImf/ImfIntAttribute.o \
	Source/OpenEXR/IlmImf/ImfIO.o \
	Source/OpenEXR/IlmImf/ImfKeyCode.o \
	Source/OpenEXR/IlmImf/ImfKeyCodeAttribute.o \
	Source/OpenEXR/IlmImf/ImfLineOrderAttribute.o \
	Source/OpenEXR/IlmImf/ImfLut.o \
	Source/OpenEXR/IlmImf/ImfMatrixAttribute.o \
	Source/OpenEXR/IlmImf/ImfMisc.o \
	Source/OpenEXR/IlmImf/ImfOpaqueAttribute.o \
	Source/OpenEXR/IlmImf/ImfOutputFile.o \
	Source/OpenEXR/IlmImf/ImfPizCompressor.o \
	Source/OpenEXR/IlmImf/ImfPreviewImage.o \
	Source/OpenEXR/IlmImf/ImfPreviewImageAttribute.o \
	Source/OpenEXR/IlmImf/ImfPxr24Compressor.o \
	Source/OpenEXR/IlmImf/ImfRational.o \
	Source/OpenEXR/IlmImf/ImfRationalAttribute.o \
	Source/OpenEXR/IlmImf/ImfRgbaFile.o \
	Source/OpenEXR/IlmImf/ImfRgbaYca.o \
	Source/OpenEXR/IlmImf/ImfRleCompressor.o \
	Source/OpenEXR/IlmImf/ImfScanLineInputFile.o \
	Source/OpenEXR/IlmImf/ImfStandardAttributes.o \
	Source/OpenEXR/IlmImf/ImfStdIO.o \
	Source/OpenEXR/IlmImf/ImfStringAttribute.o \
	Source/OpenEXR/IlmImf/ImfStringVectorAttribute.o \
	Source/OpenEXR/IlmImf/ImfTestFile.o \
	Source/OpenEXR/IlmImf/ImfThreading.o \
	Source/OpenEXR/IlmImf/ImfTileDescriptionAttribute.o \
	Source/OpenEXR/IlmImf/ImfTiledInputFile.o \
	Source/OpenEXR/IlmImf/ImfTiledMisc.o \
	Source/OpenEXR/IlmImf/ImfTiledOutputFile.o \
	Source/OpenEXR/IlmImf/ImfTiledRgbaFile.o \
	Source/OpenEXR/IlmImf/ImfTileOffsets.o \
	Source/OpenEXR/IlmImf/ImfTimeCode.o \
	Source/OpenEXR/IlmImf/ImfTimeCodeAttribute.o \
	Source/OpenEXR/IlmImf/ImfVecAttribute.o \
	Source/OpenEXR/IlmImf/ImfVersion.o \
	Source/OpenEXR/IlmImf/ImfWav.o \
	Source/OpenEXR/IlmImf/ImfZipCompressor.o \
	Source/OpenEXR/Imath/ImathBox.o \
	Source/OpenEXR/Imath/ImathColorAlgo.o \
	Source/OpenEXR/Imath/ImathFun.o \
	Source/OpenEXR/Imath/ImathMatrixAlgo.o \
	Source/OpenEXR/Imath/ImathRandom.o \
	Source/OpenEXR/Imath/ImathShear.o \
	Source/OpenEXR/Imath/ImathVec.o \
	Source/OpenEXR/Iex/IexBaseExc.o \
	Source/OpenEXR/Iex/IexThrowErrnoExc.o \
	Source/OpenEXR/Half/half.o \
	Source/OpenEXR/IlmThread/IlmThread.o \
	Source/OpenEXR/IlmThread/IlmThreadMutex.o \
	Source/OpenEXR/IlmThread/IlmThreadPool.o \
	Source/OpenEXR/IlmThread/IlmThreadSemaphore.o \
	Source/LibRawLite/internal/dcraw_common.o \
	Source/LibRawLite/internal/dcraw_fileio.o \
	Source/LibRawLite/internal/demosaic_packs.o \
	Source/LibRawLite/src/libraw_c_api.o \
	Source/LibRawLite/src/libraw_cxx.o \
	Source/LibRawLite/src/libraw_datastream.o

OBJS = \
	Source/FreeImage/FreeImageC.o \
	Source/LibJPEG/jaricom.o \
	Source/LibJPEG/jcapimin.o \
	Source/LibJPEG/jcapistd.o \
	Source/LibJPEG/jcarith.o \
	Source/LibJPEG/jccoefct.o \
	Source/LibJPEG/jccolor.o \
	Source/LibJPEG/jcdctmgr.o \
	Source/LibJPEG/jchuff.o \
	Source/LibJPEG/jcinit.o \
	Source/LibJPEG/jcmainct.o \
	Source/LibJPEG/jcmarker.o \
	Source/LibJPEG/jcmaster.o \
	Source/LibJPEG/jcomapi.o \
	Source/LibJPEG/jcparam.o \
	Source/LibJPEG/jcprepct.o \
	Source/LibJPEG/jcsample.o \
	Source/LibJPEG/jctrans.o \
	Source/LibJPEG/jdapimin.o \
	Source/LibJPEG/jdapistd.o \
	Source/LibJPEG/jdarith.o \
	Source/LibJPEG/jdatadst.o \
	Source/LibJPEG/jdatasrc.o \
	Source/LibJPEG/jdcoefct.o \
	Source/LibJPEG/jdcolor.o \
	Source/LibJPEG/jddctmgr.o \
	Source/LibJPEG/jdhuff.o \
	Source/LibJPEG/jdinput.o \
	Source/LibJPEG/jdmainct.o \
	Source/LibJPEG/jdmarker.o \
	Source/LibJPEG/jdmaster.o \
	Source/LibJPEG/jdmerge.o \
	Source/LibJPEG/jdpostct.o \
	Source/LibJPEG/jdsample.o \
	Source/LibJPEG/jdtrans.o \
	Source/LibJPEG/jerror.o \
	Source/LibJPEG/jfdctflt.o \
	Source/LibJPEG/jfdctfst.o \
	Source/LibJPEG/jfdctint.o \
	Source/LibJPEG/jidctflt.o \
	Source/LibJPEG/jidctfst.o \
	Source/LibJPEG/jidctint.o \
	Source/LibJPEG/jmemmgr.o \
	Source/LibJPEG/jmemnobs.o \
	Source/LibJPEG/jquant1.o \
	Source/LibJPEG/jquant2.o \
	Source/LibJPEG/jutils.o \
	Source/LibJPEG/transupp.o \
	Source/LibPNG/png.o \
	Source/LibPNG/pngerror.o \
	Source/LibPNG/pngget.o \
	Source/LibPNG/pngmem.o \
	Source/LibPNG/pngpread.o \
	Source/LibPNG/pngread.o \
	Source/LibPNG/pngrio.o \
	Source/LibPNG/pngrtran.o \
	Source/LibPNG/pngrutil.o \
	Source/LibPNG/pngset.o \
	Source/LibPNG/pngtrans.o \
	Source/LibPNG/pngwio.o \
	Source/LibPNG/pngwrite.o \
	Source/LibPNG/pngwtran.o \
	Source/LibPNG/pngwutil.o \
	Source/LibTIFF4/tif_aux.o \
	Source/LibTIFF4/tif_close.o \
	Source/LibTIFF4/tif_codec.o \
	Source/LibTIFF4/tif_color.o \
	Source/LibTIFF4/tif_compress.o \
	Source/LibTIFF4/tif_dir.o \
	Source/LibTIFF4/tif_dirinfo.o \
	Source/LibTIFF4/tif_dirread.o \
	Source/LibTIFF4/tif_dirwrite.o \
	Source/LibTIFF4/tif_dumpmode.o \
	Source/LibTIFF4/tif_error.o \
	Source/LibTIFF4/tif_extension.o \
	Source/LibTIFF4/tif_fax3.o \
	Source/LibTIFF4/tif_fax3sm.o \
	Source/LibTIFF4/tif_flush.o \
	Source/LibTIFF4/tif_getimage.o \
	Source/LibTIFF4/tif_jpeg.o \
	Source/LibTIFF4/tif_luv.o \
	Source/LibTIFF4/tif_lzma.o \
	Source/LibTIFF4/tif_lzw.o \
	Source/LibTIFF4/tif_next.o \
	Source/LibTIFF4/tif_ojpeg.o \
	Source/LibTIFF4/tif_open.o \
	Source/LibTIFF4/tif_packbits.o \
	Source/LibTIFF4/tif_pixarlog.o \
	Source/LibTIFF4/tif_predict.o \
	Source/LibTIFF4/tif_print.o \
	Source/LibTIFF4/tif_read.o \
	Source/LibTIFF4/tif_strip.o \
	Source/LibTIFF4/tif_swab.o \
	Source/LibTIFF4/tif_thunder.o \
	Source/LibTIFF4/tif_tile.o \
	Source/LibTIFF4/tif_version.o \
	Source/LibTIFF4/tif_warning.o \
	Source/LibTIFF4/tif_write.o \
	Source/LibTIFF4/tif_zip.o \
	Source/ZLib/adler32.o \
	Source/ZLib/compress.o \
	Source/ZLib/crc32.o \
	Source/ZLib/deflate.o \
	Source/ZLib/gzclose.o \
	Source/ZLib/gzlib.o \
	Source/ZLib/gzread.o \
	Source/ZLib/gzwrite.o \
	Source/ZLib/infback.o \
	Source/ZLib/inffast.o \
	Source/ZLib/inflate.o \
	Source/ZLib/inftrees.o \
	Source/ZLib/trees.o \
	Source/ZLib/uncompr.o \
	Source/ZLib/zutil.o \
	Source/LibOpenJPEG/bio.o \
	Source/LibOpenJPEG/cidx_manager.o \
	Source/LibOpenJPEG/cio.o \
	Source/LibOpenJPEG/dwt.o \
	Source/LibOpenJPEG/event.o \
	Source/LibOpenJPEG/image.o \
	Source/LibOpenJPEG/j2k.o \
	Source/LibOpenJPEG/j2k_lib.o \
	Source/LibOpenJPEG/jp2.o \
	Source/LibOpenJPEG/jpt.o \
	Source/LibOpenJPEG/mct.o \
	Source/LibOpenJPEG/mqc.o \
	Source/LibOpenJPEG/openjpeg.o \
	Source/LibOpenJPEG/phix_manager.o \
	Source/LibOpenJPEG/pi.o \
	Source/LibOpenJPEG/ppix_manager.o \
	Source/LibOpenJPEG/raw.o \
	Source/LibOpenJPEG/t1.o \
	Source/LibOpenJPEG/t2.o \
	Source/LibOpenJPEG/tcd.o \
	Source/LibOpenJPEG/tgt.o \
	Source/LibOpenJPEG/thix_manager.o \
	Source/LibOpenJPEG/tpix_manager.o
 
-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
