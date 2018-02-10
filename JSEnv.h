
#ifndef JS_ENV_H
#define JS_ENV_H


#include "include/libplatform/libplatform.h"
#include "include/v8.h"

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
    
   
};

#endif
