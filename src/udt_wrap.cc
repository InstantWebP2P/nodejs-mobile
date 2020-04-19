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

#include "udtconnection_wrap.h"
#include "env-inl.h"
#include "handle_wrap.h"
#include "node_buffer.h"
#include "node_internals.h"
#include "udtconnect_wrap.h"
#include "stream_base-inl.h"
#include "udtstream_wrap.h"
#include "util-inl.h"

#include <cstdlib>


namespace node {

using v8::Boolean;
using v8::Context;
using v8::EscapableHandleScope;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Integer;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::String;
using v8::Uint32;
using v8::Value;

MaybeLocal<Object> UDTWrap::Instantiate(Environment* env,
                                        AsyncWrap* parent,
                                        UDTWrap::SocketType type) {
  EscapableHandleScope handle_scope(env->isolate());
  AsyncHooks::DefaultTriggerAsyncIdScope trigger_scope(parent);
  CHECK_EQ(env->udt_constructor_template().IsEmpty(), false);
  Local<Function> constructor = env->udt_constructor_template()
                                    ->GetFunction(env->context())
                                    .ToLocalChecked();
  CHECK_EQ(constructor.IsEmpty(), false);
  Local<Value> type_value = Int32::New(env->isolate(), type);
  return handle_scope.EscapeMaybe(
      constructor->NewInstance(env->context(), 1, &type_value));
}


void UDTWrap::Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context,
                         void* priv) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  Local<String> udtString = FIXED_ONE_BYTE_STRING(env->isolate(), "UDT");
  t->SetClassName(udtString);
  t->InstanceTemplate()
    ->SetInternalFieldCount(StreamBase::kStreamBaseFieldCount);

  // Init properties
  t->InstanceTemplate()->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "reading"),
                             Boolean::New(env->isolate(), false));
  t->InstanceTemplate()->Set(env->owner_symbol(), Null(env->isolate()));
  t->InstanceTemplate()->Set(env->onconnection_string(), Null(env->isolate()));

  t->Inherit(UDTStreamWrap::GetConstructorTemplate(env));

  env->SetProtoMethod(t, "open", Open);
  env->SetProtoMethod(t, "bind", Bind);
  env->SetProtoMethod(t, "listen", Listen);
  env->SetProtoMethod(t, "connect", Connect);
  env->SetProtoMethod(t, "bind6", Bind6);
  env->SetProtoMethod(t, "connect6", Connect6);
  env->SetProtoMethod(t, "getsockname",
                      GetSockOrPeerName<UDTWrap, uvudt_getsockname>);
  env->SetProtoMethod(t, "getpeername",
                      GetSockOrPeerName<UDTWrap, uvudt_getpeername>);
  env->SetProtoMethod(t, "setNoDelay", SetNoDelay);
  env->SetProtoMethod(t, "setKeepAlive", SetKeepAlive);

  target->Set(env->context(),
              udtString,
              t->GetFunction(env->context()).ToLocalChecked()).Check();
  env->set_udt_constructor_template(t);

  // Create FunctionTemplate for UDTConnectWrap.
  Local<FunctionTemplate> cwt =
      BaseObject::MakeLazilyInitializedJSTemplate(env);
  cwt->Inherit(AsyncWrap::GetConstructorTemplate(env));
  Local<String> wrapString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "UDTConnectWrap");
  cwt->SetClassName(wrapString);
  target->Set(env->context(),
              wrapString,
              cwt->GetFunction(env->context()).ToLocalChecked()).Check();

  // Define constants
  Local<Object> constants = Object::New(env->isolate());
  NODE_DEFINE_CONSTANT(constants, SOCKET);
  NODE_DEFINE_CONSTANT(constants, SERVER);
  NODE_DEFINE_CONSTANT(constants, UVUDT_FLAG_IPV6ONLY);
  target->Set(context,
              env->constants_string(),
              constants).Check();
}


void UDTWrap::New(const FunctionCallbackInfo<Value>& args) {
  // This constructor should not be exposed to public javascript.
  // Therefore we assert that we are not trying to call this as a
  // normal function.
  CHECK(args.IsConstructCall());
  CHECK(args[0]->IsInt32());
  Environment* env = Environment::GetCurrent(args);

  int type_value = args[0].As<Int32>()->Value();
  UDTWrap::SocketType type = static_cast<UDTWrap::SocketType>(type_value);

  ProviderType provider;
  switch (type) {
    case SOCKET:
      provider = PROVIDER_UDTWRAP;
      break;
    case SERVER:
      provider = PROVIDER_UDTSERVERWRAP;
      break;
    default:
      UNREACHABLE();
  }

  new UDTWrap(env, args.This(), provider);
}


UDTWrap::UDTWrap(Environment* env, Local<Object> object, ProviderType provider)
    : UDTConnectionWrap(env, object, provider) {
  int r = uvudt_init(env->event_loop(), &handle_);
  CHECK_EQ(r, 0);  // How do we proxy this error up to javascript?
                   // Suggestion: uvudt_init() returns void.
}


void UDTWrap::SetNoDelay(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  int enable = static_cast<int>(args[0]->IsTrue());
  int err = uvudt_nodelay(&wrap->handle_, enable);
  args.GetReturnValue().Set(err);
}


void UDTWrap::SetKeepAlive(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  Environment* env = wrap->env();
  int enable;
  if (!args[0]->Int32Value(env->context()).To(&enable)) return;
  unsigned int delay = args[1].As<Uint32>()->Value();
  int err = uvudt_keepalive(&wrap->handle_, enable, delay);
  args.GetReturnValue().Set(err);
}


void UDTWrap::Open(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  int64_t val;
  if (!args[0]->IntegerValue(args.GetIsolate()->GetCurrentContext()).To(&val))
    return;
  uv_os_sock_t fd = static_cast<uv_os_sock_t>(val);
  int err = uvudt_open(&wrap->handle_, fd);

  args.GetReturnValue().Set(err);
}

template <typename T>
void UDTWrap::Bind(
    const FunctionCallbackInfo<Value>& args,
    int family,
    std::function<int(const char* ip_address, int port, T* addr)> uv_ip_addr) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  Environment* env = wrap->env();
  node::Utf8Value ip_address(env->isolate(), args[0]);
  int port;
  unsigned int flags = 0;
  if (!args[1]->Int32Value(env->context()).To(&port)) return;
  if (family == AF_INET6 &&
      !args[2]->Uint32Value(env->context()).To(&flags)) {
    return;
  }

  T addr;
  int err = uv_ip_addr(*ip_address, port, &addr);

  if (err == 0) {
    err = uvudt_bind(&wrap->handle_,
                     reinterpret_cast<const sockaddr*>(&addr),
                     flags & UVUDT_FLAG_REUSEADDR,
                     flags & UVUDT_FLAG_REUSEABLE);
  }
  args.GetReturnValue().Set(err);
}

void UDTWrap::Bind(const FunctionCallbackInfo<Value>& args) {
  Bind<sockaddr_in>(args, AF_INET, uv_ip4_addr);
}


void UDTWrap::Bind6(const FunctionCallbackInfo<Value>& args) {
  Bind<sockaddr_in6>(args, AF_INET6, uv_ip6_addr);
}


void UDTWrap::Listen(const FunctionCallbackInfo<Value>& args) {
  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));
  Environment* env = wrap->env();
  int backlog;
  if (!args[0]->Int32Value(env->context()).To(&backlog)) return;
  int err = uvudt_listen(reinterpret_cast<uvudt_t*>(&wrap->handle_),
                         backlog,
                         OnConnection);
  args.GetReturnValue().Set(err);
}


void UDTWrap::Connect(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[2]->IsUint32());
  int port = args[2].As<Uint32>()->Value();
  Connect<sockaddr_in>(args,
                       [port](const char* ip_address, sockaddr_in* addr) {
      return uv_ip4_addr(ip_address, port, addr);
  });
}


void UDTWrap::Connect6(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args[2]->IsUint32());
  int port;
  if (!args[2]->Int32Value(env->context()).To(&port)) return;
  Connect<sockaddr_in6>(args,
                        [port](const char* ip_address, sockaddr_in6* addr) {
      return uv_ip6_addr(ip_address, port, addr);
  });
}

template <typename T>
void UDTWrap::Connect(const FunctionCallbackInfo<Value>& args,
    std::function<int(const char* ip_address, T* addr)> uv_ip_addr) {
  Environment* env = Environment::GetCurrent(args);

  UDTWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap,
                          args.Holder(),
                          args.GetReturnValue().Set(UV_EBADF));

  CHECK(args[0]->IsObject());
  CHECK(args[1]->IsString());

  Local<Object> req_wrap_obj = args[0].As<Object>();
  node::Utf8Value ip_address(env->isolate(), args[1]);

  T addr;
  int err = uv_ip_addr(*ip_address, &addr);

  if (err == 0) {
    AsyncHooks::DefaultTriggerAsyncIdScope trigger_scope(wrap);
    UDTConnectWrap* req_wrap =
        new UDTConnectWrap(env, req_wrap_obj, AsyncWrap::PROVIDER_UDTCONNECTWRAP);
    err = req_wrap->Dispatch(uvudt_connect,
                             &wrap->handle_,
                             reinterpret_cast<const sockaddr*>(&addr),
                             AfterConnect);
    if (err)
      delete req_wrap;
  }

  args.GetReturnValue().Set(err);
}


}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(udt_wrap, node::UDTWrap::Initialize)
