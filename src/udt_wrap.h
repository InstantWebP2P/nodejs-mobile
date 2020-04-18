// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_UDT_WRAP_H_
#define SRC_UDT_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "async_wrap.h"
#include "udtconnection_wrap.h"

#include "uvudt.h"

namespace node {

class Environment;

class UDTWrap : public UDTConnectionWrap<UDTWrap, uvudt_t> {
 public:
  enum SocketType {
    SOCKET,
    SERVER
  };

  static v8::MaybeLocal<v8::Object> Instantiate(Environment* env,
                                                AsyncWrap* parent,
                                                SocketType type);
  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);

  SET_NO_MEMORY_INFO()
  SET_SELF_SIZE(UDTWrap)
  std::string MemoryInfoName() const override {
    switch (provider_type()) {
      case ProviderType::PROVIDER_UDTWRAP:
        return "UDTSocketWrap";
      case ProviderType::PROVIDER_UDTSERVERWRAP:
        return "UDTServerWrap";
      default:
        UNREACHABLE();
    }
  }

private:
 typedef uvudt_t HandleType;

 template <typename T, int (*F)(const typename T::HandleType*, sockaddr*, int*)>
 friend void GetSockOrPeerName(const v8::FunctionCallbackInfo<v8::Value>&);

 UDTWrap(Environment* env, v8::Local<v8::Object> object, ProviderType provider);

 static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void SetNoDelay(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void SetKeepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void Bind(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void Bind6(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void Listen(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void Connect(const v8::FunctionCallbackInfo<v8::Value>& args);
 static void Connect6(const v8::FunctionCallbackInfo<v8::Value>& args);
 template <typename T>
 static void Connect(
     const v8::FunctionCallbackInfo<v8::Value>& args,
     std::function<int(const char* ip_address, T* addr)> uv_ip_addr);
 static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
 template <typename T>
 static void Bind(
     const v8::FunctionCallbackInfo<v8::Value>& args,
     int family,
     std::function<int(const char* ip_address, int port, T* addr)> uv_ip_addr);      
};


}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_UDT_WRAP_H_
