#ifndef __JS_INTERNAL_CONTEXT_H__
#define __JS_INTERNAL_CONTEXT_H__


#include "v8.h"
#include <functional>

namespace JSMySQL
{
    class Context 
    {
        public:
        Context(v8::Isolate* );
        ~Context();
        
        void LoadScript(const std::string& script_path);
        
        void BindNativeFunction(const std::string& name,std::function<void(const v8::FunctionCallbackInfo<v8::Value>&)>& inFunction);
        
        bool ExecuteFunction(const std::string& name, std::vector<v8::Local<v8::Value> >& arguments, std::string& result);
        
    private:
        v8::Persistent<v8::Context> m_v8Context;
        
        v8::Isolate* m_isolate;
        
        static void ReadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
        
        static void Require(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}


#endif
