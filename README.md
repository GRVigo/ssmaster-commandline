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

### Common parameters - ORIENTATION (-o[orientation])

Choose here the cube orientation for the required solves. Selections with many orientations (All, U & D, etc.) requires more processing time than specific orientations (U, UF, etc.)

The orientations refer to the position of the cube when you perform the scramble. For example, if you perform the scramble with the white face up and the green face in the front, the DR (down-right) orientation will be with yellow face up and red face in the front.

By default all orientations will be explored. You can specify the next orientations:

- U (U layer up)
- D (D layer up)
- F (F layer up)
- B (B layer up)
- R (R layer up)
- L (L layer up)
- UD (U & D layers up)
- FB (F & B layers up)
- RL (R & L layers up)
- UF
- UR
- UB
- UL
- DF
- DR
- DB
- DL
- FU
- FR
- FD
- FL
- BU
- BR
- BD
- BL
- RU
- RF
- RD
- RB
- LU
- LF
- LD
- LB

Example, CFOP solves with cross in U & D layers:

```
> ./ssmaster cfop -oUD
Selected CFOP method (parameter 'cfop')
Orientation search defined (parameter '-oUD')
Parameters summary:
        Selected method: CFOP
        Orientation(s): U & D layers up
```

### Common parameters - MAX. INSPECTIONS (-i[number])

Is the maximum number of solves that will be analyzed for each orientation.

It's possible not to reach the required number of inspections, as not all orientations provide the same amount of solves, or even not solves at all.

Example, CFOP solves with cross in D layer (U layer up) and a maximum of 4 different inspections (solves):

```
> ./ssmaster cfop -oU -i4
Selected CFOP method (parameter 'cfop')
Orientation search defined (parameter '-oU')
Number of inspections set to 4 (parameter '-i4')
```

Note: inspections parameter do not have influence in LBL method.

### Common parameters - METRIC

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

Example, LBL solve with 1.5 Half Turn Metric:

```
> ./ssmaster lbl 1.5htm
Selected LBL method (parameter 'lbl')
Using 1.5HTM metric (parameter '1.5htm')
```

See https://www.speedsolving.com/wiki/index.php/Metric for information about metrics.

### Layer-By-Layer method

### CFOP method

### Roux method

### Petrus method

### ZZ method
