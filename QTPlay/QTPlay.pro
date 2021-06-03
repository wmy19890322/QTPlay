QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FFMpegControl.cpp \
    ThreadAudio.cpp \
    ThreadVideo.cpp \
    VideoWidget.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Dexter.h \
    FFMpegControl.h \
    MainWindow.h \
    ThreadAudio.h \
    ThreadVideo.h \
    VideoWidget.h \
    include/libavcodec/ac3_parser.h \
    include/libavcodec/adts_parser.h \
    include/libavcodec/avcodec.h \
    include/libavcodec/avdct.h \
    include/libavcodec/avfft.h \
    include/libavcodec/bsf.h \
    include/libavcodec/codec.h \
    include/libavcodec/codec_desc.h \
    include/libavcodec/codec_id.h \
    include/libavcodec/codec_par.h \
    include/libavcodec/d3d11va.h \
    include/libavcodec/dirac.h \
    include/libavcodec/dv_profile.h \
    include/libavcodec/dxva2.h \
    include/libavcodec/jni.h \
    include/libavcodec/mediacodec.h \
    include/libavcodec/packet.h \
    include/libavcodec/qsv.h \
    include/libavcodec/vdpau.h \
    include/libavcodec/version.h \
    include/libavcodec/videotoolbox.h \
    include/libavcodec/vorbis_parser.h \
    include/libavcodec/xvmc.h \
    include/libavdevice/avdevice.h \
    include/libavdevice/version.h \
    include/libavfilter/avfilter.h \
    include/libavfilter/buffersink.h \
    include/libavfilter/buffersrc.h \
    include/libavfilter/version.h \
    include/libavformat/avformat.h \
    include/libavformat/avio.h \
    include/libavformat/version.h \
    include/libavutil/adler32.h \
    include/libavutil/aes.h \
    include/libavutil/aes_ctr.h \
    include/libavutil/attributes.h \
    include/libavutil/audio_fifo.h \
    include/libavutil/avassert.h \
    include/libavutil/avconfig.h \
    include/libavutil/avstring.h \
    include/libavutil/avutil.h \
    include/libavutil/base64.h \
    include/libavutil/blowfish.h \
    include/libavutil/bprint.h \
    include/libavutil/bswap.h \
    include/libavutil/buffer.h \
    include/libavutil/camellia.h \
    include/libavutil/cast5.h \
    include/libavutil/channel_layout.h \
    include/libavutil/common.h \
    include/libavutil/cpu.h \
    include/libavutil/crc.h \
    include/libavutil/des.h \
    include/libavutil/detection_bbox.h \
    include/libavutil/dict.h \
    include/libavutil/display.h \
    include/libavutil/dovi_meta.h \
    include/libavutil/downmix_info.h \
    include/libavutil/encryption_info.h \
    include/libavutil/error.h \
    include/libavutil/eval.h \
    include/libavutil/ffversion.h \
    include/libavutil/fifo.h \
    include/libavutil/file.h \
    include/libavutil/film_grain_params.h \
    include/libavutil/frame.h \
    include/libavutil/hash.h \
    include/libavutil/hdr_dynamic_metadata.h \
    include/libavutil/hmac.h \
    include/libavutil/hwcontext.h \
    include/libavutil/hwcontext_cuda.h \
    include/libavutil/hwcontext_d3d11va.h \
    include/libavutil/hwcontext_drm.h \
    include/libavutil/hwcontext_dxva2.h \
    include/libavutil/hwcontext_mediacodec.h \
    include/libavutil/hwcontext_opencl.h \
    include/libavutil/hwcontext_qsv.h \
    include/libavutil/hwcontext_vaapi.h \
    include/libavutil/hwcontext_vdpau.h \
    include/libavutil/hwcontext_videotoolbox.h \
    include/libavutil/hwcontext_vulkan.h \
    include/libavutil/imgutils.h \
    include/libavutil/intfloat.h \
    include/libavutil/intreadwrite.h \
    include/libavutil/lfg.h \
    include/libavutil/log.h \
    include/libavutil/lzo.h \
    include/libavutil/macros.h \
    include/libavutil/mastering_display_metadata.h \
    include/libavutil/mathematics.h \
    include/libavutil/md5.h \
    include/libavutil/mem.h \
    include/libavutil/motion_vector.h \
    include/libavutil/murmur3.h \
    include/libavutil/opt.h \
    include/libavutil/parseutils.h \
    include/libavutil/pixdesc.h \
    include/libavutil/pixelutils.h \
    include/libavutil/pixfmt.h \
    include/libavutil/random_seed.h \
    include/libavutil/rational.h \
    include/libavutil/rc4.h \
    include/libavutil/replaygain.h \
    include/libavutil/ripemd.h \
    include/libavutil/samplefmt.h \
    include/libavutil/sha.h \
    include/libavutil/sha512.h \
    include/libavutil/spherical.h \
    include/libavutil/stereo3d.h \
    include/libavutil/tea.h \
    include/libavutil/threadmessage.h \
    include/libavutil/time.h \
    include/libavutil/timecode.h \
    include/libavutil/timestamp.h \
    include/libavutil/tree.h \
    include/libavutil/twofish.h \
    include/libavutil/tx.h \
    include/libavutil/version.h \
    include/libavutil/video_enc_params.h \
    include/libavutil/xtea.h \
    include/libpostproc/postprocess.h \
    include/libpostproc/version.h \
    include/libswresample/swresample.h \
    include/libswresample/version.h \
    include/libswscale/swscale.h \
    include/libswscale/version.h

FORMS += \
    MainWindow.ui


INCLUDEPATH += "./include/"

LIBS += -L$$PWD/lib -lavutil -lavformat -lavcodec -lavdevice -lavfilter -lpostproc -lswresample -lswscale

TRANSLATIONS += \
    QTPlay_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
