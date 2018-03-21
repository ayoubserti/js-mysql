
#include <my_global.h>
#include <my_sys.h>
#include <mysql_com.h>

#undef min
#undef max

#include "JSEnv.h"

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
    if(args->arg_count < 1)
    {
        strcpy(message,"Wrong arguments number; must be 1 or more arguments");
        return 1;
    }
    if(args->arg_type[0] != STRING_RESULT )
    {
        strcpy(message,"Wrong arguments type; arg 1 must be strings");
        return 1;
    }
    
    JSEnv* jsEnv = JSEnv::Create();
    
    initid->ptr = (char*)jsEnv;
    
    return 0;
}

void javascript_deinit(UDF_INIT *initid )
{
    //deinitialize JS context
   
    JSEnv* jsEnv = (JSEnv*)(initid->ptr);
    delete jsEnv;
}

char *javascript(UDF_INIT *initid ,
               UDF_ARGS *args, char *result, unsigned long *length,
               char *is_null, char *error MY_ATTRIBUTE((unused)))
{
    JSEnv* jsEnv = (JSEnv*)(initid->ptr);
    // Run the script to get the result.
    
    std::string r = jsEnv->ExecuteJSFunction( args->args[0],args);
    
    
    strcpy(result, r.c_str());
    *length = r.size();
    return result;
    
}


#endif /* HAVE_DLOPEN */
