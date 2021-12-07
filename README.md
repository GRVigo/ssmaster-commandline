# ssmaster-commandline

## Command line version of SpeedSolving Master beta - a simple tutorial

### No installation needed

Unzip the release file in a directory with write permissions. If you list the files you'll see the executable 'ssmaster' (linux) or 'ssmaster.exe' (windows), and several xml files:

```
1LLL.xml  cancel2.xml  cancel3.xml  CMLL.xml  COLL.xml  EPLL.xml  OCLL.xml  OLL.xml  PLL.xml  ssmaster  ZBLL.xml
```

**cancel2.xml** and **cancel3.xml** are equivalences between algorithms for get cancellations, you should not edit this files. The other xml files are algsets used in the different methods. You could edit them to use your own algorithms, but be careful with the format.

Note: some algsets have more than one algorithm per case, but only the first one will be used.

### Generating scrambles

First at all you need a scramble, so run the executable file without parameters:

```
> ./ssmaster 
No selected method, appending 1 random scramble(s) of 20 movements to 'scrambles.txt' file...Done!
```

A file called **scrambles.txt** will be created with a 20 movements scramble inside.

Using parameter **-n[number]** you will append the specified number of scrambles to the **scrambles.txt** file.

You can control the number of movements in the scrambles with **-l[number]** parameter.

The next example will append 100 scrambles of 25 movements to the **scrambles.txt** file:

```
> ./ssmaster -l25 -n100
Scramble length set to 25 (parameter '-l25')
Number of solves set to 100 (parameter '-n100')
No selected method, appending 100 random scramble(s) of 25 movements to 'scrambles.txt' file...Done!
```

**scrambles.txt** is a regular text file with one scramble per line. Of course, you can edit it and write your own scrambles.

To continue this tutorial, delete the **scrambles.txt** file and generate a new one with only a scramble.

### Layer-By-Layer method

### CFOP method

### Roux method

### Petrus method

### ZZ method

### Common parameters

By default, the STM metric (slice turn metric) will be used. You can specify another metric:

- HTM
- QTM
- STM
- QSTM
- ETM
- ATM
- PTM
- 1.5HTM
- OBTM
