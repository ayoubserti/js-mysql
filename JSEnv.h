
#ifndef JS_ENV_H
#define JS_ENV_H

#include <cstdlib>
#include <cstring>
#include "libplatform/libplatform.h"
#include "v8.h"

typedef struct st_udf_args UDF_ARGS;

namespace JSMySQL {
    class Context;
}

class JSEnv{
public:
    
    static JSEnv* Create();
    
    std::string  ExecuteJSFunction( const std::string& inFuncName ,UDF_ARGS* args);
    inline v8::Isolate* GetIsolate();

    ~JSEnv();
    
private:
    
    static void Init();

    v8::Isolate* m_isolate;
    
    static std::unique_ptr<v8::Platform> sPlatform;
    
    JSMySQL::Context*  m_jsContext;
#if V8_MAJOR_VERSION==5
    class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
    public:
        virtual void* Allocate(size_t length) {
            void* data = AllocateUninitialized(length);
            return data == NULL ? data : memset(data, 0, length);
        }
        virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
        virtual void Free(void* data, size_t) { free(data); }
    };
    
    std::unique_ptr<ArrayBufferAllocator> m_allocator;

#endif
    
   
};

#endif
