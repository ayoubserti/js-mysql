# About
`js-mysql` is a set of MySQL UDF (user-defined function) functions to execute JavaScript within an SQL query.

# Example

`mysql> SELECT javascript("myJSFunc" , FirstName , LastName) FROM Persons;`

# Dependencies
`js-mysql` is build as a shared library ( e.g libjs-mysql.so) and link with `mysqld` and a set of v8 static libraries. 

 
