
(function(ctx){
    function require(modulename){
         return  requireNative(root_path + "/" + modulename);
    }

    
    ctx.require = require;
    var mainScript = require("/script.js");
    Object.assign(ctx,mainScript);
    

})(this)


