#ifndef MYTHVDPAUHELPER_H
#define MYTHVDPAUHELPER_H

// Qt
#include <QSize>
#include <QMutex>
#include <QFlags>

// MythTV
#include "mythframe.h"
#include "mythcodeccontext.h"
#include "videoouttypes.h"

// FFmpeg
extern "C" {
#include "libavutil/hwcontext_vdpau.h"
#include "vdpau/vdpau_x11.h"
#include "libavcodec/avcodec.h"
}

class MythXDisplay;
class VideoColourSpace;

class VDPAUCodec
{
  public:
    VDPAUCodec(MythCodecContext::CodecProfile Profile, QSize Size,
               uint32_t Macroblocks, uint32_t Level);
    bool Supported(int Width, int Height, int Level);
    QSize    m_maxSize        { 0, 0 };
    uint32_t m_maxMacroBlocks { 0 };
    uint32_t m_maxLevel       { 0 };
};

using VDPAUProfile = QPair<MythCodecContext::CodecProfile, VDPAUCodec>;
using VDPAUProfiles = QList<VDPAUProfile>;

class MythVDPAUHelper : public QObject
{
    Q_OBJECT

  public:
    enum VDPMixerFeature
    {
        VDPMixerNone     = 0x00,
        VDPMixerTemporal = 0x01,
        VDPMixerSpatial  = 0x02
    };

    Q_DECLARE_FLAGS(VDPMixerFeatures, VDPMixerFeature)

    static bool   HaveVDPAU(void);
    static bool   CheckH264Decode(AVCodecContext *Context);
    static const VDPAUProfiles& GetProfiles(void);
    static void   GetDecoderList (QStringList &Decoders);

    explicit MythVDPAUHelper(AVVDPAUDeviceContext *Context);
    ~MythVDPAUHelper(void) override;

    bool             IsValid(void) const;
    void             SetPreempted(void);
    bool             IsFeatureAvailable(uint Feature);
    VdpOutputSurface CreateOutputSurface(QSize Size);
    VdpVideoMixer    CreateMixer(QSize Size, VdpChromaType ChromaType = VDP_CHROMA_TYPE_420,
                                 MythDeintType Deinterlacer = DEINT_BASIC);
    void             MixerRender(VdpVideoMixer Mixer, VdpVideoSurface Source, VdpOutputSurface Dest,
                                 FrameScanType Scan, int TopFieldFirst, QVector<AVBufferRef*>& Frames);
    void             SetCSCMatrix(VdpVideoMixer Mixer, VideoColourSpace *ColourSpace);
    void             DeleteOutputSurface(VdpOutputSurface Surface);
    void             DeleteMixer(VdpVideoMixer Mixer);
    QSize            GetSurfaceParameters(VdpVideoSurface Surface, VdpChromaType &Chroma);

  signals:
    void   DisplayPreempted(void);

  protected:
    MythVDPAUHelper(void);

    bool   H264DecodeCheck  (VdpDecoderProfile Profile, AVCodecContext *Context);
    bool   HEVCSupported    (void);
    bool   ProfileCheck     (VdpDecoderProfile Profile, uint32_t &Level,
                             uint32_t &Macros, uint32_t &Width, uint32_t &Height);

  private:
    bool   InitProcs(void);

  private:
    bool                              m_valid                            { false   };
    bool                              m_createdDevice                    { false   };
    VdpDevice                         m_device                           { 0       };
    MythXDisplay                     *m_display                          { nullptr };

    VdpGetProcAddress                *m_vdpGetProcAddress                { nullptr };
    VdpGetErrorString                *m_vdpGetErrorString                { nullptr };
    VdpGetInformationString          *m_vdpGetInformationString          { nullptr };
    VdpDeviceDestroy                 *m_vdpDeviceDestroy                 { nullptr };
    VdpDecoderQueryCapabilities      *m_vdpDecoderQueryCapabilities      { nullptr };
    VdpDecoderCreate                 *m_vdpDecoderCreate                 { nullptr };
    VdpDecoderDestroy                *m_vdpDecoderDestroy                { nullptr };
    VdpVideoMixerCreate              *m_vdpVideoMixerCreate              { nullptr };
    VdpVideoMixerDestroy             *m_vdpVideoMixerDestroy             { nullptr };
    VdpVideoMixerRender              *m_vdpVideoMixerRender              { nullptr };
    VdpVideoMixerSetAttributeValues  *m_vdpVideoMixerSetAttributeValues  { nullptr };
    VdpVideoMixerSetFeatureEnables   *m_vdpVideoMixerSetFeatureEnables   { nullptr };
    VdpVideoMixerQueryFeatureSupport *m_vdpVideoMixerQueryFeatureSupport { nullptr };
    VdpOutputSurfaceCreate           *m_vdpOutputSurfaceCreate           { nullptr };
    VdpOutputSurfaceDestroy          *m_vdpOutputSurfaceDestroy          { nullptr };
    VdpVideoSurfaceGetParameters     *m_vdpVideoSurfaceGetParameters     { nullptr };
    VdpPreemptionCallbackRegister    *m_vdpPreemptionCallbackRegister    { nullptr };
};

#endif // MYTHVDPAUHELPER_H
