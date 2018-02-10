
# About
`js-mysql` is a set of MySQL UDF (user-defined function) functions to execute JavaScript within an SQL query.

# Example

`mysql> SELECT javascript("myJSFunc" , FirstName , LastName) FROM Persons;`

# Dependencies
`js-mysql` is build as a shared library ( e.g libjs-mysql.so) and link with `mysqld` and a set of v8 static libraries. 

It depends on:
 - `v8` a JavaScript Engine developed by Google and used in Chrome and NodeJS.
 - `depot_tool` build tool used by v8
 - `CMake` projects generator

 # Build

 First, make sure you have MySQL Server and build toolchain installed. Install CMake tool.

 ## building v8

 `v8` come with a seperate build file. use `build-v8.sh` to build on Mac OSX and Linux. It will run for 15min to 1hour depending on your system configuration and Internet connexion.

 ## building js-mysql

 ````
 $ git clone https://github.com/ayoubserti/js-mysql.git
 $ ./build-v8.sh  
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make 
 ````

 # Install & Test 
 Comming 



 
