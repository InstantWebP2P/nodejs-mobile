#ifndef UDT_CONNECT_WRAP_H_
#define UDT_CONNECT_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "req_wrap-inl.h"
#include "async_wrap.h"

#include "uvudt.h"

namespace node {

class UDTConnectWrap : public ReqWrap<uvudt_connect_t> {
 public:
  UDTConnectWrap(Environment* env,
              v8::Local<v8::Object> req_wrap_obj,
              AsyncWrap::ProviderType provider);

  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(UDTConnectWrap)
  SET_SELF_SIZE(UDTConnectWrap)
};

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // UDT_CONNECT_WRAP_H_
