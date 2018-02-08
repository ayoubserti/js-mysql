
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

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport) 
#else
#define DLLEXP
#endif

#ifdef HAVE_DLOPEN


/* These must be right or mysqld will not find the symbol! */

C_MODE_START;
/*
 execute Javascript function
 */
DLLEXP my_bool javascript_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP void javascript_deinit(UDF_INIT *initid);
DLLEXP char *javascript(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
C_MODE_END;


my_bool javascript_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if(args->arg_count < 2)
    {
        strcpy(message,"Wrong arguments number; must be 2 or more arguments");
        return 1;
    }
    if(args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT)
    {
        strcpy(message,"Wrong arguments type; arg 1 and 2 must be strings");
        return 1;
    }
    
    JSEnv* jsEnv = JSEnv::Create(args->args[0]);
    
    initid->ptr = (char*)jsEnv;
    
    return 0;
}

void javascript_deinit(UDF_INIT *initid )
{
    //deinitialize JS context
   
    JSEnv* jsEnv = (JSEnv*)(initid->ptr);
    delete jsEnv;
    /*isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();*/
    
}

char *javascript(UDF_INIT *initid ,
               UDF_ARGS *args, char *result, unsigned long *length,
               char *is_null, char *error MY_ATTRIBUTE((unused)))
{
    JSEnv* jsEnv = (JSEnv*)(initid->ptr);
    // Run the script to get the result.
    
    std::string r = jsEnv->ExecuteScript( args->args[1],args);
    
    //printf("%s\n", r);
    strcpy(result, r.c_str());
    *length = r.size();
    return (char*)r.c_str();
}


#endif /* HAVE_DLOPEN */
