# attrtools
Small command-line app for changing file 'immutable' attribute

## Usage

```plain
Attrtools Usage:
  ./attrtools +i <path> ; Set Immutable
  ./attrtools -i <path> ; Unset Immutable
  ./attrtools <path> ; Is Immutable?
```

+ attrtools __+i__ _\<path\>_ to add 'immutable' flag for file
+ attrtools __-i__ _\<path\>_ to remove 'immutable' flag for file
+ attrtools _\<path\>_ to query file flags
  
## Examples

> sudo __./attrtools +i libe2.o__

`File: libe2.o`

> __./attrtools libe2.o__

```plain
File: libe2.o
16
+i
+a
```

> sudo __./attrtools -i libe2.o__

`File: libe2.o`

> sudo __./attrtools +i libe2__

`Cannot stat: libe2: No such file or directory`

> __./attrtools -i libe2.o__

```plain
File: libe2.o
Failed to chmod -i: Operation not permitted
```

## Exit code

Exit __1__ if __command line/file stat__ fails

Exit __2__ if __chattr__ fails
