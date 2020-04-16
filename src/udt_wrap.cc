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

#include "udt_wrap.h"
#include "env-inl.h"
#include "node_buffer.h"
#include "handle_wrap.h"
#include "req_wrap-inl.h"
#include "util-inl.h"

namespace node {

using v8::Array;
using v8::Context;
using v8::DontDelete;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::PropertyAttribute;
using v8::ReadOnly;
using v8::Signature;
using v8::String;
using v8::Uint32;
using v8::Undefined;
using v8::Value;

class WriteWrap : public ReqWrap<uvudt_write_t> {
 public:
  WriteWrap(Environment* env, Local<Object> req_wrap_obj, bool have_callback);
  inline bool have_callback() const;
  size_t msg_size;

  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(WriteWrap)
  SET_SELF_SIZE(WriteWrap)

 private:
  const bool have_callback_;
};


WriteWrap::WriteWrap(Environment* env,
                   Local<Object> req_wrap_obj,
                   bool have_callback)
    : ReqWrap(env, req_wrap_obj, AsyncWrap::PROVIDER_UDTWRITEWRAP),
      have_callback_(have_callback) {
}


inline bool WriteWrap::have_callback() const {
  return have_callback_;
}


UDTWrap::UDTWrap(Environment* env, Local<Object> object)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(&handle_),
                 AsyncWrap::PROVIDER_UDTWRAP) {
  int r = uvudt_init(env->event_loop(), &handle_);
  CHECK_EQ(r, 0);  // can't fail anyway
}


void UDTWrap::Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context,
                         void* priv) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  Local<String> udtString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "UDT");
  t->SetClassName(udtString);

  enum PropertyAttribute attributes =
      static_cast<PropertyAttribute>(ReadOnly | DontDelete);

  Local<Signature> signature = Signature::New(env->isolate(), t);

  Local<FunctionTemplate> get_fd_templ =
      FunctionTemplate::New(env->isolate(),
                            UDTWrap::GetFD,
                            env->as_callback_data(),
                            signature);

  t->PrototypeTemplate()->SetAccessorProperty(env->fd_string(),
                                              get_fd_templ,
                                              Local<FunctionTemplate>(),
                                              attributes);

  env->SetProtoMethod(t, "open", Open);
  env->SetProtoMethod(t, "bind", Bind);
  env->SetProtoMethod(t, "connect", Connect);
  env->SetProtoMethod(t, "send", Write);
  env->SetProtoMethod(t, "bind6", Bind6);
  env->SetProtoMethod(t, "connect6", Connect6);
  env->SetProtoMethod(t, "send6", Write6);
  env->SetProtoMethod(t, "disconnect", Disconnect);
  env->SetProtoMethod(t, "recvStart", ReadStart);
  env->SetProtoMethod(t, "recvStop", ReadStop);
  env->SetProtoMethod(t, "getpeername",
                      GetSockOrPeerName<UDTWrap, uvudt_getpeername>);
  env->SetProtoMethod(t, "getsockname",
                      GetSockOrPeerName<UDTWrap, uvudt_getsockname>);
  env->SetProtoMethod(t, "addMembership", AddMembership);
  env->SetProtoMethod(t, "dropMembership", DropMembership);
  env->SetProtoMethod(t, "addSourceSpecificMembership",
                      AddSourceSpecificMembership);
  env->SetProtoMethod(t, "dropSourceSpecificMembership",
                      DropSourceSpecificMembership);
  env->SetProtoMethod(t, "setMulticastInterface", SetMulticastInterface);
  env->SetProtoMethod(t, "setMulticastTTL", SetMulticastTTL);
  env->SetProtoMethod(t, "setMulticastLoopback", SetMulticastLoopback);
  env->SetProtoMethod(t, "setBroadcast", SetBroadcast);
  env->SetProtoMethod(t, "setTTL", SetTTL);
  env->SetProtoMethod(t, "bufferSize", BufferSize);

  t->Inherit(HandleWrap::GetConstructorTemplate(env));

  target->Set(env->context(),
              udtString,
              t->GetFunction(env->context()).ToLocalChecked()).Check();
  env->set_udt_constructor_function(
      t->GetFunction(env->context()).ToLocalChecked());

  // Create FunctionTemplate for WriteWrap
  Local<FunctionTemplate> swt =
      BaseObject::MakeLazilyInitializedJSTemplate(env);
  swt->Inherit(AsyncWrap::GetConstructorTemplate(env));
  Local<String> sendWrapString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap");
  swt->SetClassName(sendWrapString);
  target->Set(env->context(),
              sendWrapString,
              swt->GetFunction(env->context()).ToLocalChecked()).Check();

  Local<Object> constants = Object::New(env->isolate());
  NODE_DEFINE_CONSTANT(constants, UV_UDT_IPV6ONLY);
  target->Set(context,
              env->constants_string(),
              constants).Check();
}


void UDTWrap::New(const FunctionCallbackInfo<Value>& args) {
  CHECK(args.IsConstructCall());
  Environment* env = Environment::GetCurrent(args);
  new UDTWrap(env, args.This());
}


void UDTWrap::GetFD(const FunctionCallbackInfo<Value>& args) {
  int fd = UV_EBADF;
#if !defined(_WIN32)
  UDTWrap* wrap = Unwrap<UDTWrap>(args.This());
  if (wrap != nullptr)
    uv_fileno(reinterpret_cast<uv_handle_t*>(&wrap->handle_), &fd);
#endif
  args.GetReturnValue().Set(fd);
}

int sockaddr_for_family(int address_family,
                        const char* address,
                        const unsigned short port,
                        struct sockaddr_storage* addr) {
  switch (address_family) {
  case AF_INET:
    return uv_ip4_addr(address, port, reinterpret_cast<sockaddr_in*>(addr));
  case AF_INET6:
    return uv_ip6_addr(address, port, reinterpret_cast<sockaddr_in6*>(addr));
  default:
    CHECK(0 && "unexpected address family");
  }
}

void UDTWrap::DoBind(const FunctionCallbackInfo<Value>& args, int family) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  // bind(ip, port, flags)
  CHECK_EQ(args.Length(), 3);

  node::Utf8Value address(args.GetIsolate(), args[0]);
  Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
  uint32_t port, flags;
  if (!args[1]->Uint32Value(ctx).To(&port) ||
      !args[2]->Uint32Value(ctx).To(&flags))
    return;
  struct sockaddr_storage addr_storage;
  int err = sockaddr_for_family(family, address.out(), port, &addr_storage);
  if (err == 0) {
    err = uvudt_bind(&wrap->handle_,
                      reinterpret_cast<const sockaddr*>(&addr_storage),
                      flags);
  }

  args.GetReturnValue().Set(err);
}


void UDTWrap::DoConnect(const FunctionCallbackInfo<Value>& args, int family) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK_EQ(args.Length(), 2);

  node::Utf8Value address(args.GetIsolate(), args[0]);
  Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
  uint32_t port;
  if (!args[1]->Uint32Value(ctx).To(&port))
    return;
  struct sockaddr_storage addr_storage;
  int err = sockaddr_for_family(family, address.out(), port, &addr_storage);
  if (err == 0) {
    err = uvudt_connect(&wrap->handle_,
                         reinterpret_cast<const sockaddr*>(&addr_storage));
  }

  args.GetReturnValue().Set(err);
}


void UDTWrap::Bind(const FunctionCallbackInfo<Value>& args) {
  DoBind(args, AF_INET);
}


void UDTWrap::Bind6(const FunctionCallbackInfo<Value>& args) {
  DoBind(args, AF_INET6);
}


void UDTWrap::BufferSize(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK(args[0]->IsUint32());
  CHECK(args[1]->IsBoolean());
  bool is_recv = args[1].As<v8::Boolean>()->Value();
  const char* uv_func_name = is_recv ? "uv_recv_buffer_size" :
                                       "uv_send_buffer_size";

  if (!args[0]->IsInt32()) {
    env->CollectUVExceptionInfo(args[2], UV_EINVAL, uv_func_name);
    return args.GetReturnValue().SetUndefined();
  }

  uv_handle_t* handle = reinterpret_cast<uv_handle_t*>(&wrap->handle_);
  int size = static_cast<int>(args[0].As<Uint32>()->Value());
  int err;

  if (is_recv)
    err = uv_recv_buffer_size(handle, &size);
  else
    err = uv_send_buffer_size(handle, &size);

  if (err != 0) {
    env->CollectUVExceptionInfo(args[2], err, uv_func_name);
    return args.GetReturnValue().SetUndefined();
  }

  args.GetReturnValue().Set(size);
}


void UDTWrap::Connect(const FunctionCallbackInfo<Value>& args) {
  DoConnect(args, AF_INET);
}


void UDTWrap::Connect6(const FunctionCallbackInfo<Value>& args) {
  DoConnect(args, AF_INET6);
}


void UDTWrap::Disconnect(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK_EQ(args.Length(), 0);

  int err = uvudt_connect(&wrap->handle_, nullptr);

  args.GetReturnValue().Set(err);
}

#define X(name, fn)                                                            \
  void UDTWrap::name(const FunctionCallbackInfo<Value>& args) {                \
    UDTWrap* wrap = Unwrap<UDTWrap>(args.Holder());                            \
    Environment* env = wrap->env();                                            \
    CHECK_EQ(args.Length(), 1);                                                \
    int flag;                                                                  \
    if (!args[0]->Int32Value(env->context()).To(&flag)) {                      \
      return;                                                                  \
    }                                                                          \
    int err = wrap == nullptr ? UV_EBADF : fn(&wrap->handle_, flag);           \
    args.GetReturnValue().Set(err);                                            \
  }

X(SetTTL, uvudt_set_ttl)
X(SetBroadcast, uvudt_set_broadcast)
X(SetMulticastTTL, uvudt_set_multicast_ttl)
X(SetMulticastLoopback, uvudt_set_multicast_loop)

#undef X

void UDTWrap::SetMulticastInterface(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsString());

  Utf8Value iface(args.GetIsolate(), args[0]);

  const char* iface_cstr = *iface;

  int err = uvudt_set_multicast_interface(&wrap->handle_, iface_cstr);
  args.GetReturnValue().Set(err);
}

void UDTWrap::SetMembership(const FunctionCallbackInfo<Value>& args,
                            uv_membership membership) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK_EQ(args.Length(), 2);

  node::Utf8Value address(args.GetIsolate(), args[0]);
  node::Utf8Value iface(args.GetIsolate(), args[1]);

  const char* iface_cstr = *iface;
  if (args[1]->IsUndefined() || args[1]->IsNull()) {
      iface_cstr = nullptr;
  }

  int err = uvudt_set_membership(&wrap->handle_,
                                  *address,
                                  iface_cstr,
                                  membership);
  args.GetReturnValue().Set(err);
}


void UDTWrap::AddMembership(const FunctionCallbackInfo<Value>& args) {
  SetMembership(args, UV_JOIN_GROUP);
}


void UDTWrap::DropMembership(const FunctionCallbackInfo<Value>& args) {
  SetMembership(args, UV_LEAVE_GROUP);
}

void UDTWrap::SetSourceMembership(const FunctionCallbackInfo<Value>& args,
                                  uv_membership membership) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK_EQ(args.Length(), 3);

  node::Utf8Value source_address(args.GetIsolate(), args[0]);
  node::Utf8Value group_address(args.GetIsolate(), args[1]);
  node::Utf8Value iface(args.GetIsolate(), args[2]);

  if (*iface == nullptr) return;
  const char* iface_cstr = *iface;
  if (args[2]->IsUndefined() || args[2]->IsNull()) {
    iface_cstr = nullptr;
  }

  int err = uvudt_set_source_membership(&wrap->handle_,
                                         *group_address,
                                         iface_cstr,
                                         *source_address,
                                         membership);
  args.GetReturnValue().Set(err);
}

void UDTWrap::AddSourceSpecificMembership(
  const FunctionCallbackInfo<Value>& args) {
  SetSourceMembership(args, UV_JOIN_GROUP);
}


void UDTWrap::DropSourceSpecificMembership(
  const FunctionCallbackInfo<Value>& args) {
  SetSourceMembership(args, UV_LEAVE_GROUP);
}


void UDTWrap::DoWrite(const FunctionCallbackInfo<Value>& args, int family) {
  Environment* env = Environment::GetCurrent(args);

  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK(args.Length() == 4 || args.Length() == 6);
  CHECK(args[0]->IsObject());
  CHECK(args[1]->IsArray());
  CHECK(args[2]->IsUint32());

  bool sendto = args.Length() == 6;
  if (sendto) {
    // send(req, list, list.length, port, address, hasCallback)
    CHECK(args[3]->IsUint32());
    CHECK(args[4]->IsString());
    CHECK(args[5]->IsBoolean());
  } else {
    // send(req, list, list.length, hasCallback)
    CHECK(args[3]->IsBoolean());
  }

  Local<Object> req_wrap_obj = args[0].As<Object>();
  Local<Array> chunks = args[1].As<Array>();
  // it is faster to fetch the length of the
  // array in js-land
  size_t count = args[2].As<Uint32>()->Value();
  const bool have_callback = sendto ? args[5]->IsTrue() : args[3]->IsTrue();

  size_t msg_size = 0;

  MaybeStackBuffer<uv_buf_t, 16> bufs(count);

  // construct uv_buf_t array
  for (size_t i = 0; i < count; i++) {
    Local<Value> chunk = chunks->Get(env->context(), i).ToLocalChecked();

    size_t length = Buffer::Length(chunk);

    bufs[i] = uv_buf_init(Buffer::Data(chunk), length);
    msg_size += length;
  }

  int err = 0;
  struct sockaddr_storage addr_storage;
  sockaddr* addr = nullptr;
  if (sendto) {
    const unsigned short port = args[3].As<Uint32>()->Value();
    node::Utf8Value address(env->isolate(), args[4]);
    err = sockaddr_for_family(family, address.out(), port, &addr_storage);
    if (err == 0) {
      addr = reinterpret_cast<sockaddr*>(&addr_storage);
    }
  }

  uv_buf_t* bufs_ptr = *bufs;
  if (err == 0 && !UNLIKELY(env->options()->test_udt_no_try_send)) {
    err = uvudt_try_send(&wrap->handle_, bufs_ptr, count, addr);
    if (err == UV_ENOSYS || err == UV_EAGAIN) {
      err = 0;
    } else if (err >= 0) {
      size_t sent = err;
      while (count > 0 && bufs_ptr->len <= sent) {
        sent -= bufs_ptr->len;
        bufs_ptr++;
        count--;
      }
      if (count > 0) {
        CHECK_LT(sent, bufs_ptr->len);
        bufs_ptr->base += sent;
        bufs_ptr->len -= sent;
      } else {
        CHECK_EQ(static_cast<size_t>(err), msg_size);
        // + 1 so that the JS side can distinguish 0-length async sends from
        // 0-length sync sends.
        args.GetReturnValue().Set(static_cast<uint32_t>(msg_size) + 1);
        return;
      }
    }
  }

  if (err == 0) {
    AsyncHooks::DefaultTriggerAsyncIdScope trigger_scope(wrap);
    WriteWrap* req_wrap = new WriteWrap(env, req_wrap_obj, have_callback);
    req_wrap->msg_size = msg_size;

    err = req_wrap->Dispatch(uvudt_send,
                             &wrap->handle_,
                             bufs_ptr,
                             count,
                             addr,
                             OnWrite);
    if (err)
      delete req_wrap;
  }

  args.GetReturnValue().Set(err);
}


void UDTWrap::Write(const FunctionCallbackInfo<Value>& args) {
  DoWrite(args, AF_INET);
}


void UDTWrap::Write6(const FunctionCallbackInfo<Value>& args) {
  DoWrite(args, AF_INET6);
}


void UDTWrap::ReadStart(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  int err = uvudt_read_start(&wrap->handle_, OnAlloc, OnRead);
  // UV_EALREADY means that the socket is already bound but that's okay
  if (err == UV_EALREADY)
    err = 0;
  args.GetReturnValue().Set(err);
}


void UDTWrap::ReadStop(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  int r = uvudt_read_stop(&wrap->handle_);
  args.GetReturnValue().Set(r);
}


void UDTWrap::OnWrite(uvudt_write_t* req, int status) {
  std::unique_ptr<WriteWrap> req_wrap{static_cast<WriteWrap*>(req->data)};
  if (req_wrap->have_callback()) {
    Environment* env = req_wrap->env();
    HandleScope handle_scope(env->isolate());
    Context::Scope context_scope(env->context());
    Local<Value> arg[] = {
      Integer::New(env->isolate(), status),
      Integer::New(env->isolate(), req_wrap->msg_size),
    };
    req_wrap->MakeCallback(env->oncomplete_string(), 2, arg);
  }
}


void UDTWrap::OnAlloc(uv_handle_t* handle,
                      size_t suggested_size,
                      uv_buf_t* buf) {
  UDTWrap* wrap = static_cast<UDTWrap*>(handle->data);
  *buf = wrap->env()->AllocateManaged(suggested_size).release();
}

void UDTWrap::OnRead(ssize_t nread,
                     const uv_buf_t* buf_) {
  UDTWrap* wrap = static_cast<UDTWrap*>(handle->data);
  Environment* env = wrap->env();

  AllocatedBuffer buf(env, *buf_);
  if (nread == 0 && addr == nullptr) {
    return;
  }

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Object> wrap_obj = wrap->object();
  Local<Value> argv[] = {
    Integer::New(env->isolate(), nread),
    wrap_obj,
    Undefined(env->isolate()),
    Undefined(env->isolate())
  };

  if (nread < 0) {
    wrap->MakeCallback(env->onmessage_string(), arraysize(argv), argv);
    return;
  }

  buf.Resize(nread);
  argv[2] = buf.ToBuffer().ToLocalChecked();
  argv[3] = AddressToJS(env, addr);
  wrap->MakeCallback(env->onmessage_string(), arraysize(argv), argv);
}

void UDTWrap::OnConnection(uvudt_t* handle, int status) {
  WrapType* wrap_data = static_cast<WrapType*>(handle->data);
  CHECK_NOT_NULL(wrap_data);
  CHECK_EQ(&wrap_data->handle_, reinterpret_cast<UVType*>(handle));

  Environment* env = wrap_data->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  // We should not be getting this callback if someone has already called
  // uv_close() on the handle.
  CHECK_EQ(wrap_data->persistent().IsEmpty(), false);

  Local<Value> client_handle;

  if (status == 0) {
    // Instantiate the client javascript object and handle.
    Local<Object> client_obj;
    if (!WrapType::Instantiate(env, wrap_data, WrapType::SOCKET)
             .ToLocal(&client_obj))
      return;

    // Unwrap the client javascript object.
    WrapType* wrap;
    ASSIGN_OR_RETURN_UNWRAP(&wrap, client_obj);
    uvudt_t* client = reinterpret_cast<uvudt_t*>(&wrap->handle_);
    // uvudt_accept can fail if the new connection has already been closed, in
    // which case an EAGAIN (resource temporarily unavailable) will be
    // returned.
    if (uvudt_accept(handle, client)) return;

    // Successful accept. Call the onconnection callback in JavaScript land.
    client_handle = client_obj;
  } else {
    client_handle = Undefined(env->isolate());
  }

  Local<Value> argv[] = {Integer::New(env->isolate(), status), client_handle};
  wrap_data->MakeCallback(env->onconnection_string(), arraysize(argv), argv);
}

void UDTWrap::AfterConnect(uvudt_connect_t* req, int status) {
  std::unique_ptr<ConnectWrap> req_wrap(static_cast<ConnectWrap*>(req->data));
  CHECK_NOT_NULL(req_wrap);
  WrapType* wrap = static_cast<WrapType*>(req->handle->data);
  CHECK_EQ(req_wrap->env(), wrap->env());
  Environment* env = wrap->env();

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  // The wrap and request objects should still be there.
  CHECK_EQ(req_wrap->persistent().IsEmpty(), false);
  CHECK_EQ(wrap->persistent().IsEmpty(), false);

  bool readable, writable;

  if (status) {
    readable = writable = false;
  } else {
    readable = uv_is_readable(req->handle) != 0;
    writable = uv_is_writable(req->handle) != 0;
  }

  Local<Value> argv[5] = {Integer::New(env->isolate(), status),
                          wrap->object(),
                          req_wrap->object(),
                          Boolean::New(env->isolate(), readable),
                          Boolean::New(env->isolate(), writable)};

  req_wrap->MakeCallback(env->oncomplete_string(), arraysize(argv), argv);
}

MaybeLocal<Object> UDTWrap::Instantiate(Environment* env,
                                        AsyncWrap* parent,
                                        UDTWrap::SocketType type) {
  AsyncHooks::DefaultTriggerAsyncIdScope trigger_scope(parent);

  // If this assert fires then Initialize hasn't been called yet.
  CHECK_EQ(env->udt_constructor_function().IsEmpty(), false);
  return env->udt_constructor_function()->NewInstance(env->context());
}


}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(udt_wrap, node::UDTWrap::Initialize)
