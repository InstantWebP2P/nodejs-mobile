#ifndef UDT_CONNECTION_WRAP_H_
#define UDT_CONNECTION_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "udtstream_wrap.h"
#include "uvudt.h"

namespace node {

class Environment;

template <typename WrapType, typename UVType>
class UDTConnectionWrap : public UDTStreamWrap {
 public:
  static void OnConnection(uvudt_t* handle, int status);
  static void AfterConnect(uvudt_connect_t* req, int status);

 protected:
  UDTConnectionWrap(Environment* env,
                    v8::Local<v8::Object> object,
                    ProviderType provider);

  UVType handle_;
};

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // UDT_CONNECTION_WRAP_H_
