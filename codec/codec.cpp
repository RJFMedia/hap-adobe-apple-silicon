#include <stdexcept>
#include <string>

#include "hap.h"
#include "squish.h"
#include "YCoCgDXT.h"

extern "C" {
#include "YCoCg.h"
}

#include "codec.hpp"
#include "util.hpp"

int roundUpToMultipleOf4(int n)
{
    return (n + 3) & ~3;
}

const Codec4CC kHapCodecSubType{ 'H' , 'a', 'p', '1' };
const Codec4CC kHapAlphaCodecSubType{ 'H', 'a', 'p', '5' };
const Codec4CC kHapYCoCgCodecSubType{ 'H', 'a', 'p', 'Y' };
const Codec4CC kHapYCoCgACodecSubType{ 'H', 'a', 'p', 'M' };
const Codec4CC kHapAOnlyCodecSubType{ 'H', 'a', 'p', 'A' };

const CodecDetails& CodecRegistry::details()
{
    CodecNamedSubTypes hapCodecSubtypes{
        CodecNamedSubType{kHapCodecSubType, "Hap"},
        CodecNamedSubType{kHapAlphaCodecSubType, "Hap Alpha"},
        CodecNamedSubType{kHapYCoCgCodecSubType,"Hap Q"},
        CodecNamedSubType{kHapYCoCgACodecSubType,"Hap Q Alpha"},
        CodecNamedSubType{kHapAOnlyCodecSubType, "Hap Alpha-Only"} };
    
    static CodecDetails details{
        "HAP", // productName
        "HAP Movie", // fileFormatName;
        "HAP", // fileFormatShortName;
        "mov",  // videoFileExt
        FileFormat{'p', 'a', 'h', '\0'}, // fileFormat
        VideoFormat{'Y', 'P', 'A', 'H'}, // videoFormat
        hapCodecSubtypes, // codecSubTypes
        kHapAlphaCodecSubType, // defaultSubType
        false,   // isHighBitDepth
        false,  // hasExplicitIncludeAlphaChannel
        true,   // hasChunkCount
        AlphaCodecDetails{
            true,
            { { kHapCodecSubType, withoutAlpha },
              { kHapAlphaCodecSubType, withAlpha },
              { kHapYCoCgCodecSubType, withoutAlpha },
              { kHapYCoCgACodecSubType, withAlpha },
              { kHapAOnlyCodecSubType, withAlpha } },
        },
        QualityCodecDetails{
            true,
            { { kHapCodecSubType, true },
              { kHapAlphaCodecSubType, true },
              { kHapYCoCgCodecSubType, false },
              { kHapYCoCgACodecSubType, false },
              { kHapAOnlyCodecSubType, false } },  // presentForSubtype
            { { kSquishEncoderFastQuality, "Fast" },
              {kSquishEncoderNormalQuality, "Normal" } }, // descriptions
            kSquishEncoderNormalQuality},  // defaultQuality
        6,                        // premiereParamsVersion
        "HAPSpecificCodecGroup",  // premiereGroupName
        std::string(),            // premiereIncludeAlphaChannelNmae
        "HAPChunkCount",          // premiereChunkCountName
        'HAPP',                   // premiereSig [for afterEffects, must differ from afterEffectsSig]
        'HAPA',                   // afterEffectsSig
        'DTEK',                   // afterEffectsCreator
        'HAP_',                   // afterEffectsType
        'HAP_'                    // afterEffectsMacType
    };

    return details;
}

CodecRegistry::CodecRegistry()
{
    logName_ = "HAP";

    createEncoder = [=](std::unique_ptr<EncoderParametersBase> parameters) -> UniqueEncoder
    {
        return UniqueEncoder(new HapEncoder(parameters),
            [](Encoder* encoder) { delete encoder; });
    };

    createDecoder = [=](std::unique_ptr<DecoderParametersBase> parameters) -> UniqueDecoder
    {
        return UniqueDecoder(new HapDecoder(parameters),
            [](Decoder* decoder) { delete decoder; });
    };
}

std::shared_ptr<CodecRegistry>& CodecRegistry::codec()
{
    static std::shared_ptr<CodecRegistry> codecRegistry = std::make_shared<CodecRegistry>();
    return codecRegistry;
}

double CodecRegistry::getPixelFormatSize(
        CodecAlpha alpha,
        Codec4CC subType,
        int quality)
{
    return 4; // !!! not correct, for bitrate estimation; should be moved to encoder
}

std::string CodecRegistry::logName_;
std::string CodecRegistry::logName()
{
    return logName_;
}

HapEncoder::HapEncoder(std::unique_ptr<EncoderParametersBase>& params)
    : Encoder(std::move(params)),
      count_(parameters().codec4CC == kHapYCoCgACodecSubType ? 2 : 1),
      chunkCounts_((parameters().chunkCounts == HapChunkCounts{ 0, 0 })
                   ? HapChunkCounts{ 1, 1 }
                   : parameters().chunkCounts),     // auto represented as 0, 0
      textureFormats_(getTextureFormats(parameters().codec4CC)),
      compressors_{ HapCompressorSnappy, HapCompressorSnappy }
{
    SquishEncoderQuality quality = (SquishEncoderQuality)parameters().quality;
    for (size_t i = 0; i < count_; ++i)
    {
        converters_[i] = TextureConverter::create(parameters().frameSize, textureFormats_[i], quality);
        sizes_[i] = (unsigned long)converters_[i]->size();
    }
}

HapEncoder::~HapEncoder()
{
}

std::unique_ptr<EncoderJob> HapEncoder::create()
{
    std::array<TextureConverter*, 2> converters;
    for (size_t i = 0; i < count_; ++i)
        converters[i] = converters_[i].get();   //!!! should probably move things like this back on HapEncoder

    return std::make_unique<HapEncoderJob>(
            parameters().frameSize,
            count_,
            chunkCounts_,
            textureFormats_,
            compressors_,
            converters,
            sizes_
        );
}

std::array<unsigned int, 2> HapEncoder::getTextureFormats(Codec4CC subType)
{
    if (subType == kHapCodecSubType) {
        return { HapTextureFormat_RGB_DXT1 };
    }
    else if (subType == kHapAlphaCodecSubType) {
        return { HapTextureFormat_RGBA_DXT5 };
    }
    else if (subType == kHapYCoCgCodecSubType) {
        return { HapTextureFormat_YCoCg_DXT5 };
    }
    else if (subType == kHapYCoCgACodecSubType) {
        return { HapTextureFormat_YCoCg_DXT5, HapTextureFormat_A_RGTC1 };
    }
    else if (subType == kHapAOnlyCodecSubType) {
        return { HapTextureFormat_A_RGTC1 };
    }
    else
        throw std::runtime_error("unknown codec");
}

HapEncoderJob::HapEncoderJob(
    FrameSize frameSize,
    unsigned int count,
    HapChunkCounts chunkCounts,
    std::array<unsigned int, 2> textureFormats,
    std::array<unsigned int, 2> compressors,
    std::array<TextureConverter*, 2> converters,
    std::array<unsigned long, 2> sizes)
    : frameSize_(frameSize),
      count_(count),
      chunkCounts_(chunkCounts),
      textureFormats_(textureFormats),
      compressors_(compressors),
      converters_(converters),
      sizes_(sizes)
{
}

void HapEncoderJob::doCopyExternalToLocal(
    const uint8_t *data, size_t stride, FrameFormat format)
{
    rgbaTopLeftOrigin_.resize(frameSize_.width * frameSize_.height * 4);

    // convert host format to rgba top left origin
    FrameDef frameDef(frameSize_, format);
    convertHostFrameTo_RGBA_Top_Left_U8(data, stride, frameDef, (uint8_t*)& rgbaTopLeftOrigin_[0], frameSize_.width * 4);
}

void HapEncoderJob::doEncode(EncodeOutput& out)
{
    // convert input texture from rgba to <subcodec defined> dxt [+ dxt]
    for (unsigned int i = 0; i < count_; ++i)
    {
        converters_[i]->convert(
            &rgbaTopLeftOrigin_[0],
            ycocg_,
            buffers_[i]);
    }

    // encode textures for output stream
    std::array<void*, 2> bufferPtrs;              // for hap_encode
    std::array<unsigned long, 2> buffersBytes;    // for hap_encode
    out.buffer.resize(getMaxEncodedSize());
    unsigned long outputBufferBytesUsed;
    for (unsigned int i = 0; i < count_; ++i)
    {
        bufferPtrs[i] = const_cast<uint8_t *>(&(buffers_[i][0]));
        buffersBytes[i] = (unsigned long)buffers_[i].size();
    }

    auto result = HapEncode(
        count_,
        const_cast<const void **>(&bufferPtrs[0]), const_cast<unsigned long *>(&buffersBytes[0]),
        const_cast<unsigned int *>(&textureFormats_[0]),
        const_cast<unsigned int *>(&compressors_[0]),
        const_cast<unsigned int *>(&chunkCounts_[0]),
        &out.buffer[0], (unsigned long)out.buffer.size(),
        &outputBufferBytesUsed);

    if (HapResult_No_Error != result)
    {
        throw std::runtime_error("failed to encode frame");
    }

    out.buffer.resize(outputBufferBytesUsed);
}

size_t HapEncoderJob::getMaxEncodedSize() const
{
    return HapMaxEncodedLength(count_, const_cast<unsigned long*>(&sizes_[0]), const_cast<unsigned int*>(&textureFormats_[0]), const_cast<unsigned int*>(&chunkCounts_[0]));
}

static void hapDecodeWorkSerial(HapDecodeWorkFunction function, void* p, unsigned int count, void*)
{
    for (unsigned int i = 0; i < count; ++i)
        function(p, i);
}

HapDecoder::HapDecoder(std::unique_ptr<DecoderParametersBase>& params)
    : Decoder(std::move(params))
{
}

HapDecoder::~HapDecoder()
{
}

std::unique_ptr<DecoderJob> HapDecoder::create()
{
    return std::make_unique<HapDecoderJob>(parameters().frameDef);
}

HapDecoderJob::HapDecoderJob(FrameDef frameDef)
    : frameDef_(frameDef),
      roundedWidth_(roundUpToMultipleOf4(frameDef.size.width)),
      roundedHeight_(roundUpToMultipleOf4(frameDef.size.height))
{
}

size_t HapDecoderJob::textureStorageSize(unsigned int textureFormat) const
{
    switch (textureFormat)
    {
    case HapTextureFormat_RGB_DXT1:
    case HapTextureFormat_A_RGTC1:
        return static_cast<size_t>(roundedWidth_) * static_cast<size_t>(roundedHeight_) / 2;
    case HapTextureFormat_RGBA_DXT5:
    case HapTextureFormat_YCoCg_DXT5:
        return static_cast<size_t>(roundedWidth_) * static_cast<size_t>(roundedHeight_);
    default:
        throw std::runtime_error("unknown Hap texture format");
    }
}

void HapDecoderJob::forceOpaqueAlpha()
{
    for (size_t i = 3; i < rgbaTopLeftOrigin_.size(); i += 4)
        rgbaTopLeftOrigin_[i] = 255;
}

void HapDecoderJob::decodeSquishTextureToRGBA(const std::vector<uint8_t>& dxt, int squishFlags, bool forceOpaque)
{
    rgbaTopLeftOrigin_.assign(static_cast<size_t>(frameDef_.size.width) * frameDef_.size.height * 4, 0);

    const int bytesPerBlock = (squishFlags & squish::kDxt1) ? 8 : 16;
    const uint8_t* sourceBlock = dxt.data();

    for (int y = 0; y < roundedHeight_; y += 4)
    {
        for (int x = 0; x < roundedWidth_; x += 4)
        {
            uint8_t blockRGBA[16 * 4] = {};
            squish::Decompress(blockRGBA, sourceBlock, squishFlags);

            for (int blockY = 0; blockY < 4; ++blockY)
            {
                const int dstY = y + blockY;
                if (dstY >= frameDef_.size.height)
                    continue;

                for (int blockX = 0; blockX < 4; ++blockX)
                {
                    const int dstX = x + blockX;
                    if (dstX >= frameDef_.size.width)
                        continue;

                    const size_t sourceOffset = static_cast<size_t>(blockY * 4 + blockX) * 4;
                    const size_t destOffset = (static_cast<size_t>(dstY) * frameDef_.size.width + dstX) * 4;
                    rgbaTopLeftOrigin_[destOffset + 0] = blockRGBA[sourceOffset + 0];
                    rgbaTopLeftOrigin_[destOffset + 1] = blockRGBA[sourceOffset + 1];
                    rgbaTopLeftOrigin_[destOffset + 2] = blockRGBA[sourceOffset + 2];
                    rgbaTopLeftOrigin_[destOffset + 3] = blockRGBA[sourceOffset + 3];
                }
            }

            sourceBlock += bytesPerBlock;
        }
    }

    if (forceOpaque)
        forceOpaqueAlpha();
}

void HapDecoderJob::decodeYCoCgDXT(const std::vector<uint8_t>& dxt)
{
    scratch_.resize(static_cast<size_t>(roundedWidth_) * roundedHeight_ * 4);
    rgbaTopLeftOrigin_.assign(static_cast<size_t>(frameDef_.size.width) * frameDef_.size.height * 4, 0);

    if (DeCompressYCoCgDXT5(
            reinterpret_cast<const byte*>(dxt.data()),
            reinterpret_cast<byte*>(scratch_.data()),
            frameDef_.size.width,
            frameDef_.size.height,
            roundedWidth_ * 4) != 0)
    {
        throw std::runtime_error("failed to decompress Hap YCoCg texture");
    }

    ConvertCoCg_Y8888ToRGB_(
        scratch_.data(),
        rgbaTopLeftOrigin_.data(),
        frameDef_.size.width,
        frameDef_.size.height,
        roundedWidth_ * 4,
        frameDef_.size.width * 4,
        1);

    forceOpaqueAlpha();
}

void HapDecoderJob::decodeAlphaRGTC1(const std::vector<uint8_t>& dxt, bool alphaOnly)
{
    if (rgbaTopLeftOrigin_.empty())
        rgbaTopLeftOrigin_.assign(static_cast<size_t>(frameDef_.size.width) * frameDef_.size.height * 4, 0);

    const uint8_t* sourceBlock = dxt.data();
    for (int y = 0; y < roundedHeight_; y += 4)
    {
        for (int x = 0; x < roundedWidth_; x += 4)
        {
            uint8_t blockRGBA[16 * 4] = {};
            squish::Decompress(blockRGBA, sourceBlock, squish::kRgtc1A);

            for (int blockY = 0; blockY < 4; ++blockY)
            {
                const int dstY = y + blockY;
                if (dstY >= frameDef_.size.height)
                    continue;

                for (int blockX = 0; blockX < 4; ++blockX)
                {
                    const int dstX = x + blockX;
                    if (dstX >= frameDef_.size.width)
                        continue;

                    const size_t sourceOffset = static_cast<size_t>(blockY * 4 + blockX) * 4;
                    const size_t destOffset = (static_cast<size_t>(dstY) * frameDef_.size.width + dstX) * 4;
                    const uint8_t alpha = blockRGBA[sourceOffset + 3];
                    if (alphaOnly)
                    {
                        rgbaTopLeftOrigin_[destOffset + 0] = alpha;
                        rgbaTopLeftOrigin_[destOffset + 1] = 0;
                        rgbaTopLeftOrigin_[destOffset + 2] = 0;
                        rgbaTopLeftOrigin_[destOffset + 3] = 255;
                    }
                    else
                    {
                        rgbaTopLeftOrigin_[destOffset + 3] = alpha;
                    }
                }
            }

            sourceBlock += 8;
        }
    }
}

void HapDecoderJob::doDecode(std::vector<uint8_t>& in)
{
    unsigned int textureCount = 0;
    if (HapGetFrameTextureCount(in.data(), static_cast<unsigned long>(in.size()), &textureCount) != HapResult_No_Error)
        throw std::runtime_error("failed to read Hap texture count");

    if (textureCount < 1 || textureCount > 2)
        throw std::runtime_error("unsupported Hap texture count");

    std::array<unsigned int, 2> textureFormats{0, 0};
    std::array<std::vector<uint8_t>, 2> dxtBuffers;

    for (unsigned int i = 0; i < textureCount; ++i)
    {
        if (HapGetFrameTextureFormat(in.data(), static_cast<unsigned long>(in.size()), i, &textureFormats[i]) != HapResult_No_Error)
            throw std::runtime_error("failed to read Hap texture format");

        dxtBuffers[i].resize(textureStorageSize(textureFormats[i]));
        if (HapDecode(
                in.data(),
                static_cast<unsigned long>(in.size()),
                i,
                hapDecodeWorkSerial,
                nullptr,
                dxtBuffers[i].data(),
                static_cast<unsigned long>(dxtBuffers[i].size()),
                nullptr,
                &textureFormats[i]) != HapResult_No_Error)
        {
            throw std::runtime_error("failed to decode Hap frame");
        }
    }

    switch (textureFormats[0])
    {
    case HapTextureFormat_RGB_DXT1:
        decodeSquishTextureToRGBA(dxtBuffers[0], squish::kDxt1, true);
        break;
    case HapTextureFormat_RGBA_DXT5:
        decodeSquishTextureToRGBA(dxtBuffers[0], squish::kDxt5, false);
        break;
    case HapTextureFormat_YCoCg_DXT5:
        decodeYCoCgDXT(dxtBuffers[0]);
        break;
    case HapTextureFormat_A_RGTC1:
        decodeAlphaRGTC1(dxtBuffers[0], true);
        break;
    default:
        throw std::runtime_error("unsupported Hap texture format");
    }

    if (textureCount > 1)
    {
        if (textureFormats[1] != HapTextureFormat_A_RGTC1)
            throw std::runtime_error("unsupported Hap auxiliary texture format");
        decodeAlphaRGTC1(dxtBuffers[1], false);
    }
}

void HapDecoderJob::doCopyLocalToExternal(uint8_t* data, size_t stride) const
{
    convertRGBA_Top_Left_U8_ToHostFrame(rgbaTopLeftOrigin_.data(), data, stride, frameDef_);
}
