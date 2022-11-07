#include "libmp.h"
#include "libmp_impl.h"
#include "mediapipe/framework/formats/image_frame.h"

namespace mediapipe {

MP_CPP_EXPORT LibMP* LibMP::Create(const char* graph, const char* inputStream) 
{
    LibMPImpl* libMP = new LibMPImpl();
    absl::Status status = libMP->Init(graph, inputStream);
    if (status.ok()){
        return libMP;
    }    
    else{
        LOG(INFO) << "Error initializing graph " << status.ToString();
        delete libMP;
        return nullptr;
    } 
}

MP_CPP_EXPORT size_t LibMP::GetOutputImageSize(const void* outputPacketVoid){
    auto outputPacket = reinterpret_cast<const mediapipe::Packet*>(outputPacketVoid);
    const ImageFrame &output_frame = outputPacket->Get<mediapipe::ImageFrame>();
    return output_frame.PixelDataSizeStoredContiguously();
}

MP_CPP_EXPORT bool LibMP::WriteOutputImage(uint8_t* dst, const void* outputPacketVoid){
    auto outputPacket = reinterpret_cast<const mediapipe::Packet*>(outputPacketVoid);

    const ImageFrame &output_frame = outputPacket->Get<mediapipe::ImageFrame>();
    size_t output_bytes = output_frame.PixelDataSizeStoredContiguously();

    output_frame.CopyToBuffer(dst, output_bytes);
    return true;
}

MP_CPP_EXPORT const void* LibMP::GetOutputPacketProto(const void* outputPacketVoid){
    auto outputPacket = reinterpret_cast<const mediapipe::Packet*>(outputPacketVoid);
    return reinterpret_cast<const void*>(&(outputPacket->GetProtoMessageLite()));
}
MP_CPP_EXPORT const void* LibMP::GetOutputPacketProto(const void* outputPacketVoid, unsigned int idx){
    auto outputPacket = reinterpret_cast<const mediapipe::Packet*>(outputPacketVoid);
    absl::StatusOr<std::vector<const google::protobuf::MessageLite*>> statusOrVec = outputPacket->GetVectorOfProtoMessageLitePtrs();
    if (!statusOrVec.ok()){
        return nullptr;
    }
    return reinterpret_cast<const void*>(statusOrVec.value()[idx]);
}
MP_CPP_EXPORT size_t LibMP::GetOutputProtoVecSize(const void* outputPacketVoid){
    auto outputPacket = reinterpret_cast<const mediapipe::Packet*>(outputPacketVoid);
    absl::StatusOr<std::vector<const google::protobuf::MessageLite*>> statusOrVec = outputPacket->GetVectorOfProtoMessageLitePtrs();
    if (!statusOrVec.ok()){
        return -1;
    }
    return statusOrVec.value().size();
}
MP_CPP_EXPORT size_t LibMP::GetOutputProtoByteSize(const void* outputProtoVoid){
    auto outputProto = reinterpret_cast<const google::protobuf::MessageLite*>(outputProtoVoid);
    return outputProto->ByteSizeLong();
}
MP_CPP_EXPORT bool LibMP::WriteOutputProtoData(uint8_t* dst, const void* outputProtoVoid, int size){
    auto outputProto = reinterpret_cast<const google::protobuf::MessageLite*>(outputProtoVoid);
    return outputProto->SerializeToArray(dst, size);
}

}
