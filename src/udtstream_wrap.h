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

#ifndef SRC_UDT_STREAM_WRAP_H_
#define SRC_UDT_STREAM_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "handle_wrap.h"
#include "stream_base.h"
#include "v8.h"

#include "uvudt.h"

namespace node {

class Environment;

class UDTStreamWrap : public HandleWrap, public StreamBase {
 public:
  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);

  int GetFD() override;
  bool IsAlive() override;
  bool IsClosing() override;
  bool IsIPCPipe() override;

  // JavaScript functions
  int ReadStart() override;
  int ReadStop() override;

  // Resource implementation
  int DoShutdown(ShutdownWrap* req_wrap) override;
  int DoTryWrite(uv_buf_t** bufs, size_t* count) override;
  int DoWrite(WriteWrap* w,
              uv_buf_t* bufs,
              size_t count,
              uv_stream_t* send_handle) override;

  inline uvudt_t* stream() const {
    return stream_;
  }

  ShutdownWrap* CreateShutdownWrap(v8::Local<v8::Object> object) override;
  WriteWrap* CreateWriteWrap(v8::Local<v8::Object> object) override;

  static UDTStreamWrap* From(Environment* env, v8::Local<v8::Object> object);

 protected:
  UDTStreamWrap(Environment* env,
          v8::Local<v8::Object> object,
          uvudt_t* stream,
          AsyncWrap::ProviderType provider);

  AsyncWrap* GetAsyncWrap() override;

  static v8::Local<v8::FunctionTemplate> GetConstructorTemplate(
      Environment* env);

  void OnClose() override { uvudt_close(stream_, NULL); }

 private:
  static void GetWriteQueueSize(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void SetBlocking(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Callbacks for libuv
  void OnUvAlloc(size_t suggested_size, uv_buf_t* buf);
  void OnUvRead(ssize_t nread, const uv_buf_t* buf);

  static void AfterUvWrite(uvudt_write_t* req, int status);
  static void AfterUvShutdown(uvudt_shutdown_t* req, int status);

  uvudt_t* const stream_;
};


}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_UDT_STREAM_WRAP_H_
