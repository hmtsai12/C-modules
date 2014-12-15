C-modules
=========

Create function modules

<token>
transform string to argc, argv[]
1.alloc memory can't be return immediately, need tmp variable
2.strtok can't use strtok("111,111", ","). It cause seg fault
3.declare string can't use char *str. use char str[], because
array have terminating '\0'
