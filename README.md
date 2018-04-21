[![Build Status](https://travis-ci.org/ayoubserti/js-mysql.svg?branch=master)](https://travis-ci.org/ayoubserti/js-mysql)

# About
`js-mysql` is a set of MySQL UDF (user-defined function) functions to execute JavaScript within an SQL query.

# Example

Start MySQL Server using `--plugins_dir` argument

````
mysqld --plugin_dir=path/to/plugin/directory
````

Then add `javascript` function to  MySQL Server

````sql
mysql> CREATE FUNCTION javascript RETURNS STRING SONAME "libmysql-js.so";
````
```
Query OK, 0 rows affected (0,00 sec)
```
Then you may use any exported function form `script.js` file.

````javascript
//script.js
exports.myJSFunc = function (fname,lname)
{
    return lname.charAt(0)+'.'+fname; 
}
````
```sql
mysql> SELECT javascript("myJSFunc" , FirstName , LastName) FROM Persons;
```

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

 ````bash
 $ git clone https://github.com/ayoubserti/js-mysql.git
 $ ./build-v8.sh  
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make 
 ````

 # Install
 
 Install js-mysql into usual MySQL Server Plugin directory. 

 ````bash
 $ make install
 ````
 will copy `libmysql-js.so`, `icudtl.dat`, `Loader.js`, `script.js` ,`natives_blob.bin` and  `snapshot_blob.bin` into `/usr/local/mysql/lib/plugin/`.


 # Tools

 This repository came with a utility to deploy JS script into MySQL. First you need nodejs/npm installed. Install the utility script 

 ```bash
    npm install 
 ``` 

```bash
    $ deployjs --help
```
```
    Usage: deployJS [options] <filename.js>
           --src               optional; for filename.js
           --help      -h      show this help
           --user      -u      mysql user name or current os user if ommited
           --password  -p      mysql password or prompt for it
           --database  -d      mysql database or test if ommited
``` 

Then deploying JS script become straightforward:

```bash
    $ deployjs -u root -d test --src script.js -p
    $ Enter password: 
```

You will be prompted to enter MySQL user password. 

*Tips:* 
 - the MySQL user need to have right to alter `mysql.func` table
 - the current system user need write permission on mysql plugin directory


 # Tests

 Coming 




 
