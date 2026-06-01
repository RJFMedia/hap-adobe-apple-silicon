#pragma once

// base class for different kinds of Hap encoders

#include <array>
#include <memory>
#include <vector>

#include "codec_registration.hpp"

#include "texture_converter.hpp"

// Placeholders for inputs, processing and outputs for encode process

class HapEncoderJob : public EncoderJob
{
public:
    HapEncoderJob(
        FrameSize frameSize,
        unsigned int count,
        HapChunkCounts chunkCounts,
        std::array<unsigned int, 2> textureFormats,
        std::array<unsigned int, 2> compressors,
        std::array<TextureConverter*, 2> converters,
        std::array<unsigned long, 2> sizes
        );
    ~HapEncoderJob() {}

private:
    virtual void doCopyExternalToLocal(
        const uint8_t* data,
        size_t stride,
        FrameFormat format) override;
    virtual void doEncode(EncodeOutput& out) override;

    size_t getMaxEncodedSize() const;

    FrameSize frameSize_;
    unsigned int count_;
    HapChunkCounts chunkCounts_;
    std::array<unsigned int, 2> textureFormats_;
    std::array<unsigned int, 2> compressors_;
    std::array<TextureConverter*, 2> converters_;
    std::array<unsigned long, 2> sizes_;

    std::vector<uint8_t> rgbaTopLeftOrigin_;       // for squish

    // not all of these are used by all codecs
    std::vector<uint8_t> ycocg_;                   // for ycog -> ycog_dxt
    std::array<std::vector<uint8_t>, 2> buffers_;  // for hap_encode
};

// Instantiate once per input frame definition
//

class HapEncoder : public Encoder
{
public:
    HapEncoder(std::unique_ptr<EncoderParametersBase>& params);
	~HapEncoder();

    virtual std::unique_ptr<EncoderJob> create() override;

private:
    static std::array<unsigned int, 2> getTextureFormats(Codec4CC subType);

	unsigned int count_;
	HapChunkCounts chunkCounts_;
	std::array<unsigned int, 2> textureFormats_;
	std::array<unsigned int, 2> compressors_;
	std::array<std::unique_ptr<TextureConverter>, 2> converters_;
    std::array<unsigned long, 2> sizes_;
};

class HapDecoderJob : public DecoderJob
{
public:
    HapDecoderJob(FrameDef frameDef);
    ~HapDecoderJob() {}

private:
    virtual void doDecode(std::vector<uint8_t>& in) override;
    virtual void doCopyLocalToExternal(uint8_t* data, size_t stride) const override;

    size_t textureStorageSize(unsigned int textureFormat) const;
    void forceOpaqueAlpha();
    void decodeSquishTextureToRGBA(const std::vector<uint8_t>& dxt, int squishFlags, bool forceOpaqueAlpha);
    void decodeYCoCgDXT(const std::vector<uint8_t>& dxt);
    void decodeAlphaRGTC1(const std::vector<uint8_t>& dxt, bool alphaOnly);

    FrameDef frameDef_;
    int roundedWidth_;
    int roundedHeight_;
    std::vector<uint8_t> rgbaTopLeftOrigin_;
    std::vector<uint8_t> scratch_;
};

class HapDecoder : public Decoder
{
public:
    HapDecoder(std::unique_ptr<DecoderParametersBase>& params);
    ~HapDecoder();

    virtual std::unique_ptr<DecoderJob> create() override;
};
