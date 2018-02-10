#include <string>
#include "Context.h"

using JSMySQL::Context;
using v8::Persistent;
using v8::Local;
using v8::String;
using v8::Isolate;
using v8::Value;
using v8::MaybeLocal;


bool _ReadFile(const std::string& name,std::string& outContent);

Context::Context(Isolate* isolate)
{
    m_isolate = isolate;
    
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(m_isolate);
    
    global->Set(v8::String::NewFromUtf8(
                                        m_isolate, "readFile", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(m_isolate, ReadFile));
    
    global->Set(v8::String::NewFromUtf8(
                                        m_isolate, "require", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(m_isolate, Require));
    
    m_v8Context.Reset(m_isolate, v8::Context::New(m_isolate,NULL,global));
}

Context::~Context()
{
    m_v8Context.SetWeak();
    m_v8Context.Reset();
}

void Context::BindNativeFunction(const std::string& name, std::function<void(const v8::FunctionCallbackInfo<v8::Value>&)>& inFunction)
{
    
    
}

void Context::LoadScript(const std::string& script_path)
{
    v8::HandleScope handle_scope(m_isolate);
    v8::Local<v8::Context> context = m_v8Context.Get(m_isolate);
    
    v8::Context::Scope context_scope(context);
    
    // Create a string containing the JavaScript source code.
    std::string filecontent;
    if ( _ReadFile(script_path,filecontent))
    {
        v8::Local<v8::String> source =
        v8::String::NewFromUtf8(m_isolate, filecontent.c_str(),
                                v8::NewStringType::kNormal)
        .ToLocalChecked();
        
        // Compile the source code.
        v8::TryCatch trycatch(m_isolate);
        v8::Local<v8::Script> script =
        v8::Script::Compile(context, source).ToLocalChecked();
        
        script->Run(context);
        
        if(trycatch.HasCaught())
        {
            auto exception  = trycatch.Exception();
            auto exceptionStr = exception->ToString();
            String::Utf8Value utf8(exceptionStr);
            if(*utf8 != NULL)
            {
                printf("JS Exception : %s",*utf8 );
            }
        }
    }
    
    
}

bool Context::ExecuteFunction(const std::string& name, std::vector<Local<Value> >& arguments,std::string& result )
{
    bool r = false;
    if(name=="") return false;
    
    auto context = m_v8Context.Get(m_isolate);
    auto globalObj = context->Global();
    v8::Context::Scope context_scope(context);
    
    v8::HandleScope scope(m_isolate);
    
    auto funcStr = String::NewFromUtf8(m_isolate,name.c_str(),v8::NewStringType::kNormal).ToLocalChecked();
    if(globalObj->Has(funcStr))
    {
        auto funcVal = globalObj->Get(funcStr);
        if(funcVal->IsFunction())
        {
            v8::Local<v8::Object> funcObj = funcVal.As<v8::Object>();
            v8::Local<v8::Value> resultVal = v8::Number::New(m_isolate,0);
            if ( arguments.size() == 0 )
                resultVal = funcObj->CallAsFunction(context, resultVal, 0,nullptr).ToLocalChecked();
            else
                resultVal = funcObj->CallAsFunction(context, resultVal, arguments.size(),&arguments[0]).ToLocalChecked();
            r = true;
            String::Utf8Value utf8(resultVal);
            if( *utf8 != NULL)
                result = *utf8;
        }
        
    }
    return r;
}



//== static
//FIXME: use only one definition
 bool _ReadFile(const std::string& name,std::string& outContent)
{
    FILE* file = fopen(name.c_str(), "rb");
    if (file == NULL) return false;
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    
    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            outContent = chars;
        }
    }
    fclose(file);
    
    outContent = chars;
    delete [] chars;
    
    return true;
}

/*
 ** ReadFile read file synchronously and return its content
 */
void Context::ReadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    if(args.Length() > 0 && args[0]->IsString())
    {
        std::string fileContent;
        const char* str = *(String::Utf8Value(args[0]->ToString()));
        if(_ReadFile(str,fileContent))
        {
            auto content = String::NewFromUtf8(isolate, fileContent.c_str(),v8::NewStringType::kNormal).ToLocalChecked();
            args.GetReturnValue().Set(content);
        }
    }
    else{
        Local<String> strException =String::NewFromUtf8(isolate, "Wrong argument list. argument 1 should be string",v8::NewStringType::kNormal).ToLocalChecked();
        isolate->ThrowException(strException);
    }
    
}

/*
  ** Require a module and expose the export object
  ** @discuss require create a new js context, compile source inside run and return export object
  **
 */
//using require function
/*
 @myModule.js
 export.myFunction() = function(arg1,arg2){
 return arg1 + '.' + arg2;
 }
 
 @main.js
 var module = require('myModule.js');
 module.myFunction('Ayoub', 'Serti')
 
 */

void Context::Require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = args.GetIsolate();
    v8::HandleScope handlesope(isolate);
    v8::Local<v8::ObjectTemplate> current_global = v8::ObjectTemplate::New(isolate);
    
    auto exportStr = String::NewFromUtf8(isolate,"exports",v8::NewStringType::kNormal).ToLocalChecked();
    current_global->Set(exportStr, v8::ObjectTemplate::New(isolate));
    
    Local<v8::Context> context = v8::Context::New(isolate,nullptr,current_global);
    v8::Context::Scope scope(context);
    
    if(args.Length() > 0 && args[0]->IsString())
    {
        std::string fileContent;
        const char* str = *(String::Utf8Value(args[0]->ToString()));
        if(_ReadFile(str,fileContent))
        {
            auto source = String::NewFromUtf8(isolate, fileContent.c_str(),v8::NewStringType::kNormal).ToLocalChecked();
            
            v8::TryCatch tryCach(isolate);
            {
            // Compile the source code.
            MaybeLocal<v8::Script> script_maybe =
                v8::Script::Compile(context, source);
                if(script_maybe.IsEmpty() ) return;
            
           (void)script_maybe.ToLocalChecked()->Run(context); //ignore return
            }
            if ( tryCach.HasCaught() )
            {
                //exception caught;
            }
            
            auto globalObj = context->Global();
            
            if(globalObj->Has(exportStr))
            {
                args.GetReturnValue().Set( globalObj->Get(exportStr));
                return;
            }
            
        }
    }
    else{
        Local<String> strException =String::NewFromUtf8(isolate, "Wrong argument list. argument 1 should be string",v8::NewStringType::kNormal).ToLocalChecked();
        isolate->ThrowException(strException);
    }
}
