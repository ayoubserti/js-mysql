#include <my_global.h>
#include <my_sys.h>
#include <mysql_com.h>

#include <new>
#include <vector>
#include <algorithm>

#if defined(MYSQL_SERVER)
#include <m_string.h>		/* To get my_stpcpy() */
#else
/* when compiled as standalone */
#include <string.h>
#define my_stpcpy(a,b) stpcpy(a,b)
#endif

#include "JSEnv.h"

#include <mysql.h>
#include <ctype.h>

#define _GNU_SOURCE
#include <dlfcn.h>


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
        Dl_info dl_info;
        dladdr((void*)&RuntimeLoadHook::onload, &dl_info);
        sLibPath = dl_info.dli_fname;
        sJSLoader.assign(sLibPath.begin(), sLibPath.begin() + sLibPath.rfind("/"));
        //sJSLoader += "/Loader.js";
        
    }
    static void onload(){}
    
};

static RuntimeLoadHook sHook;

//=== static function 
static std::string _ReadFile(const std::string& name)
{
    FILE* file = fopen(name.c_str(), "rb");
    if (file == NULL) return "";
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    
    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return "";
        }
    }
    fclose(file);
    
    std::string r = chars;
    delete [] chars;
    
    return r;
}

//== JSEnv class definition 

void JSEnv::Init()
{
        static bool isInit =false;
        if(!isInit)
        {
            
            v8::V8::InitializeICUDefaultLocation(sLibPath.c_str()); //load ICU data
            v8::V8::InitializeExternalStartupData(sLibPath.c_str()); //load JSVM external snapshot
            sPlatform = v8::platform::NewDefaultPlatform();
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
    create_params.array_buffer_allocator =
    v8::ArrayBuffer::Allocator::NewDefaultAllocator();
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


