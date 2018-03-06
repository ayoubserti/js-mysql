#if _WIN32
//#include <Windows.h>


#else
#define _GNU_SOURCE
#include <dlfcn.h>
#endif

#include <my_global.h>
#include <my_sys.h>
#include <mysql_com.h>

#include <memory>
#include "JSEnv.h"




#include "Context.h"

std::unique_ptr<v8::Platform> JSEnv::sPlatform;

static std::string  sLibPath;

static std::string  sJSLoader;


//hook the library load
class RuntimeLoadHook
{
public:
    RuntimeLoadHook()
    {
#if _WIN32
		//quick and dirty constants
		sLibPath = "C:\\Program\ Files\\MySQL\\MySQL\ Server\ 5.7\\lib\\plugin\\js-mysql.dll";
		sJSLoader.assign(sLibPath.begin(), sLibPath.begin() + sLibPath.rfind("\\"));

#else
        Dl_info dl_info;
        dladdr((void*)&RuntimeLoadHook::onload, &dl_info);
        sLibPath = dl_info.dli_fname;
        sJSLoader.assign(sLibPath.begin(), sLibPath.begin() + sLibPath.rfind("/"));
#endif
        //sJSLoader += "/Loader.js";
        
    }
    static void onload(){}
    
};

static RuntimeLoadHook sHook;


//== JSEnv class definition



void JSEnv::Init()
{
        static bool isInit =false;
        if(!isInit)
        {
#if V8_MAJOR_VERSION==5
            v8::V8::InitializeICU();
            sPlatform.reset(v8::platform::CreateDefaultPlatform());
#else
            v8::V8::InitializeICUDefaultLocation(sLibPath.c_str()); //load ICU data
           sPlatform = v8::platform::NewDefaultPlatform();
#endif
            v8::V8::InitializeExternalStartupData(sLibPath.c_str()); //load JSVM external snapshot
            
            v8::V8::InitializePlatform(sPlatform.get());
            v8::V8::Initialize();
            
            isInit = true;
        }
}

JSEnv* JSEnv::Create( )
{
    JSEnv *jsEnv = new JSEnv();
    JSEnv::Init();
    v8::Isolate::CreateParams create_params;
 
#if V8_MAJOR_VERSION==5
    jsEnv->m_allocator.reset( new ArrayBufferAllocator() );
    create_params.array_buffer_allocator = jsEnv->m_allocator.get();
#else
       create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
#endif
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    jsEnv->m_isolate = isolate;
    isolate->Enter();
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
    
    JSMySQL::Context* jscontext = new JSMySQL::Context(isolate);
    
    jsEnv->m_jsContext = jscontext;
    
    jscontext->LoadScript(sJSLoader);
    
    return jsEnv;
}

std::string JSEnv::ExecuteJSFunction( const std::string& inFuncName ,UDF_ARGS* args)
{
    v8::HandleScope handle_scope(m_isolate);
    int argc = args->arg_count -1;
    std::vector<v8::Local<v8::Value>> argv;
    
    std::string strResult;
    
    for (uint32_t i = 1; i< args->arg_count; ++i )
    {
        if(args->arg_type[i] ==STRING_RESULT )
        {
            v8::Local<v8::String> argsStr = v8::String::NewFromUtf8(m_isolate, args->args[i]);
            argv.push_back(argsStr);
        }
        else if (args->arg_type[i] == REAL_RESULT)
        {
            argv.push_back(v8::Number::New(m_isolate, *(double*)args->args[i]));
        }
        else if (args->arg_type[i] == INT_RESULT)
        {
            argv.push_back(v8::Integer::New(m_isolate, *(uint32_t*)args->args[i]));
        }
        else if (args->arg_type[i] == ROW_RESULT)
        {
            argc--;
        }
        else if (args->arg_type[i] == DECIMAL_RESULT)
        {
            v8::Local<v8::String> argsStr = v8::String::NewFromUtf8(m_isolate, args->args[i]);
            argv.push_back(argsStr);
        }
        else
        {
            argc--;
        }
        
    }
    
    m_jsContext->ExecuteFunction(inFuncName, argv, strResult);
    return strResult;
}

v8::Isolate* JSEnv::GetIsolate()
{
    return m_isolate;
}


JSEnv::~JSEnv()
{
    m_isolate->Exit();
    m_isolate->Dispose();
}


