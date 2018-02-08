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

std::unique_ptr<v8::Platform> JSEnv::sPlatform;

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
            v8::V8::InitializeICUDefaultLocation("/Users/mac/perso/mysql-server/builds/sql/Debug/");
            v8::V8::InitializeExternalStartupData("/Users/mac/perso/mysql-server/builds/sql/Debug/");
            sPlatform = v8::platform::NewDefaultPlatform();
            v8::V8::InitializePlatform(sPlatform.get());
            v8::V8::Initialize();
            
            isInit = true;
        }
}

JSEnv* JSEnv::Create(const std::string& inFileName )
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
    
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    
    jsEnv->m_context.Reset(isolate, context);
    
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    
    // Create a string containing the JavaScript source code.
    std::string filecontent = _ReadFile(inFileName);
    
    v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate, filecontent.c_str(),
                            v8::NewStringType::kNormal)
    .ToLocalChecked();
    
        // Compile the source code.
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
        
        // Run the script to get the result.
    v8::Local<v8::Value> r = script->Run(context).ToLocalChecked();
    
    
    return jsEnv;
}

std::string JSEnv::ExecuteScript( const std::string& inFuncName ,UDF_ARGS* args)
{
    //execute script in a new context
        v8::HandleScope handle_scope(m_isolate);
    
    v8::Local<v8::Context> context = m_context.Get(m_isolate);
    v8::Context::Scope context_scope(context);
    
    v8::Local<v8::Value> r;
    
    //execute a function into the current context
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(m_isolate,inFuncName.c_str());
    if ( global->Has(funcName) )
    {
        v8::Local<v8::Value> funcValue =  global->Get(funcName);
        v8::Local<v8::Object> funcObj = funcValue.As<v8::Object>();
        
        int argc = args->arg_count -2;
        std::vector<v8::Local<v8::Value>> argv;
        
        
        for (uint32_t i = 2; i< args->arg_count; ++i )
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
        r = v8::Number::New(m_isolate, 0);
        r =funcObj->CallAsFunction(context, r, argc,&argv[0]).ToLocalChecked();
    }
    
    v8::String::Utf8Value utf8 (r);
    if( *utf8 == NULL) return "";
    return std::string(*utf8);
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