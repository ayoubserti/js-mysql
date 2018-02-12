[![Build Status](https://travis-ci.org/ayoubserti/js-mysql.svg?branch=master)](https://travis-ci.org/ayoubserti/js-mysql)

# About
`js-mysql` is a set of MySQL UDF (user-defined function) functions to execute JavaScript within an SQL query.

# Example

Start MySQL Server using `--plugins_dir` argument

````
mysqld --plugin_dir=path/to/plugin/directory
````

Then add `javascript` function to  MySQL Server

````
mysql> CREATE FUNCTION javascript RETURNS STRING SONAME "libmysql-js.so";
Query OK, 0 rows affected (0,00 sec)

````
Then you may use any exported function form `script.js` file.

````
//script.js
exports.myJSFunc = function (fname,lname)
{
    return lname.charAt(0)+'.'+fname; 
}
````

`mysql> SELECT javascript("myJSFunc" , FirstName , LastName) FROM Persons;`

`myJSFunc` catch `FirstName`and `LastName` from every row in `Persons` table and perform a concat operation and return.

# Dependencies
`js-mysql` is build as a shared library ( e.g libmysql-js.so) and link with `mysqld` and a set of v8 static libraries. 

It depends on:
 - `v8` a JavaScript Engine developed by Google and used in Chrome and NodeJS.
 - `depot_tool` build tool used by v8
 - `CMake` projects generator

 # Build

 First, make sure you have MySQL Server and build toolchain installed. Install CMake tool.

 ## building v8

 `v8` comes with a seperate build file. use `build-v8.sh` to build on Mac OSX and Linux. It will run for 15min to 1hour depending on your system configuration and Internet connexion.

 ## building js-mysql

 ````
 $ git clone https://github.com/ayoubserti/js-mysql.git
 $ ./build-v8.sh  
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make 
 ````

 # Install
 
 Install js-mysql into usual MySQL Server Plugin directory. 

 ````
 $ make install
 ````
 will copy `libmysql-js.so`, `icudtl.dat`, `Loader.js`, `script.js` ,`natives_blob.bin` and  `snapshot_blob.bin` into `/usr/local/mysql/lib/plugin/`.

 # Tests

 Coming 




 
