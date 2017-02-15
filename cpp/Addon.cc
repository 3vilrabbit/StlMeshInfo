#include <nan.h>
#include <string>
#include "StlMesh.hpp"

void StlMeshInfo(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
    {
        Nan::ThrowTypeError("Wrong number of arguments");
        return;
    }
    
    if (!info[0]->IsString())
    {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }
    
    v8::String::Utf8Value param1(info[0]->ToString());
    std::string filename = std::string(*param1);
    
    std::string meshInfoString = StlMesh::StlMesh(filename).InfoJS();
    
    v8::Local<v8::String> meshInfo = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), meshInfoString.c_str());
    info.GetReturnValue().Set(meshInfo);
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("StlMeshInfo").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(StlMeshInfo)->GetFunction());
}

NODE_MODULE(addon, Init)
