# ssmaster-commandline

## Command line version of SpeedSolving Master beta - a simple tutorial

### No installation needed

Unzip the release file in a directory with write permissions. If you list the files you'll see the executable 'ssmaster' (linux) or 'ssmaster.exe' (windows), and several xml files:

```
1LLL.xml  cancel2.xml  cancel3.xml  CMLL.xml  COLL.xml  EPLL.xml  OCLL.xml  OLL.xml  PLL.xml  ssmaster  ZBLL.xml
```

**cancel2.xml** and **cancel3.xml** are equivalences between algorithms for get cancellations, you should not edit this files. The other xml files are algsets used in the different methods. You could edit them to use your own algorithms, but be careful with the XML format.

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

### Layer-By-Layer method (LBL)

The LBL (Layer-By-Layer) method or beginners method is based on solving the layers of the cube one by one. The first layer is solved in two steps: first the cross, indicating the movements needed to solve each edge, and then the corners, one by one.

For the second layer, the movements required to complete each of its edges are shown.

The last layer is solved in four steps: orientation of the cross, permutation of the cross, corners permutation and, finally, corners orientation.

Each step of the last layer uses only one algorithm (shown in parentheses each time it appears).
### CFOP method (cfop)

The cross is searched on each of the specified orientations, with the specified number of moves (search depth **-d[number]**). If this parameter is very low (less than 6) solutions will hardly be found. With a value of 6 or 7, results should appear and with 8 it is very likely that an "XCross" will be found. The solution of the cross is chosen among all the possible ones evaluating its number of movements, but also taking into account that the arrangement of the pieces for F2L is as favorable as possible.

First two layers (F2L) search is fast, about 2 or 3 seconds per inspection on a modern processor, and it will try to get the shortest possible solution for each solve.

The next step is to get the appropriate algorithms for **OLL** and **PLL**. These algorithms are predefined and their search is very fast. It is also possible to complete the last layer in a single algorithm (**1LLL**), or even with edges orientation and **ZBLL** algorithms (EO+ZBLL) -edges orientation search lasts a bit more time-.

### CFOP example

CFOP search with cross layer D (up layer U), 4 inspections maximum, 1LLL algset for last layer, cancellations and regrips will be applied, best solve will be shown in detail, HTM metric will be used, search depth is set to 7, and search times will be shown:

```
> ./ssmaster cfop -oU -i4 1lll canc regrip best htm -d7 time
```

Will give the next output:

```
Selected CFOP method (parameter 'cfop')
Orientation search defined (parameter '-oU')
Number of inspections set to 4 (parameter '-i4')
Selected 1LLL algorithms collection (parameter '1lll')
Cancellations enabled (parameter 'canc')
Regrip enabled (parameter 'regrip')
Show best solve (parameter 'best')
Using HTM metric (parameter 'htm')
Primary depth set to 7 (parameter '-d7')
Show times (parameter 'time')
Parameters summary:
        Selected method: CFOP
        Orientation(s): U layer up
        Number of inspections per orientation: 4
        Metric: HTM
        Last layer in one look: 1LLL
        Search depth: 7
        Best solve will be shown
        Regrips will be added
        Cancellations will be applied
        Search times will be shown
        Cores: All
Scrambles file detected, using it for solves...
Solve 1 of 1
Searching CFOP crosses... F2L... 1LLL... Done!

CFOP search for Scramble (20): F B' R U F' D F' B2 U' R' F2 U D R2 U' B' R2 L D' U2
--------------------------------------------------------------------------------
[Cross D|52(52) HTM]: (z') (U R U L F R2 U') (z y' R U' R' F' U' F) (y2 U' F' U' F U R U R') (y' z R U R U') (z' y' U' R' U2 R U' R' U R) (F R U' R' U' R U2 R' U' R U' R' U' R U2 R' U' F') (U2)
[Cross D|51(50) HTM]: (U' R' B' R2 F L D') (F U2 F' U R U' R') (R' U' R L' U L) (x' F U' F U F' R' F R) (x z R U R U' R2 U R' U') (z' y' F R U R' U' R U R' F2 r U r2 F r) (U2)
[Cross D|52(52) HTM]: (z') (U R2 F R' U F R2) (z2 U' R2 U F' R2 F) (z' y U2 R' U R F' U' F) (y F' U F R' U' R) (U2 R U2 R' U F' U' F) (y' R' U' R' F R F' U R U' R' U' R' F R F' U R) (U')
[Cross D|53(53) HTM]: (z') (L F2 U R2 F U' R) (z y L U' L' R' U R) (y' U' R' U2 R F' U F) (U R' U R U2 F U F') (y U' R U' R' U R U R') (F R' F' R U2 R U2 R' U2 r U2 R' U' R U' r') (U)


Best solve - CFOP search with cross in layer D:
---------------------------------
Cross (7): U' R' B' R2 F L D'
F2L 1 (7): F U2 F' U R U' R'
F2L 2 (6): R' U' R L' U L
F2L 3 (8): x' F U' F U F' R' F R
F2L 4 (8): x z R U R U' R2 U R' U'
1LLL (14): z' y' F R U R' U' R U R' F2 r U r2 F r
AUF: U2

Solve metric: 51 HTM
Cross in layer D, in 7 or less movements
1LLL case: 2289

Cancellations (50 HTM): U' R' B' R2 F L D' F U2 F' U R U' R2 U' M x U L x' F U' F U F' R' F R x z R U R U' R2 U R' U' z' y' F R U R' U' R U R' F2 r U r2 F r U2

Total search time: 11.422779 s
Crosses search time: 5.554943 s
F2L search time: 5.790450 s
Last layer search time: 0.077387 s
Threads used: 12 of 12
```

### Roux method (roux)

The first block is searched for each possible orientations of the cube, using at most the number of moves specified for the first block (search depth **-d[number]**). If this parameter is very low (less than 6) you will hardly find solutions. With a value of 6 or 7, results should already appear, and with 8, a result should appear for each orientation. The solution for the first block is taken among all possible ones by evaluating its number of movements, but also taking into account that the arrangement of the pieces to form the second block is as favorable as possible.

The search for the second block will be done in a similar way to the first, trying to form it completely. If this is not possible, the search will be carried out in two steps, searching in each of them for the sub-blocks (squares) that form the second block. You can specify his own search depth (**-s[number]**).

The appropriate algorithm is then found to orient the upper corners (**CMLL** or **COLL**, as selected). These algorithms are predefined and their search is very fast.

Finally, the last six edges are solved using movements of the U and M layers. It can be obtained in a single step (**L6E** parameter) or can be divided into three searches (default):
- orientation of the last six edges
- resolution of the edges in the UR and UL positions
- final resolution.

### Roux example

Roux search with orientation with layers U & D up, 3 inspections maximum per orientation, COLL algset for last layer corners, last six edges solved in one look, best solve will be shown in detail, OBTM metric will be used, search depth is set to 7 for first block, 6 for second block, and search times will be shown:

```
> ./ssmaster roux -oUD -i3 coll l6e best obtm -d7 -s6 time
```

Will give the next output:

```
Selected Roux method (parameter 'roux')
Orientation search defined (parameter '-oUD')
Number of inspections set to 3 (parameter '-i3')
Selected COLL algorithms collection (parameter 'coll')
Roux L6E in one look (parameter 'l6e')
Show best solve (parameter 'best')
Using OBTM metric (parameter 'obtm')
Primary depth set to 7 (parameter '-d7')
Secondary depth set to 6 (parameter '-s6')
Show times (parameter 'time')
Parameters summary:
        Selected method: Roux
        Orientation(s): U & D layers up
        Number of inspections per orientation: 3
        Selected metric: OBTM
        Last layer corners will be solved with COLL algorithms
        Last six edges will be solved in one look
        First block search depth: 7
        Second block search depth: 6
        Best solve will be shown
        Search times will be shown
        Cores: All
Scrambles file detected, using it for solves...
Solve 1 of 1
Searching Roux first blocks... second blocks... COLL... L6E... Done!

Roux search for Scramble (20): F B' R U F' D F' B2 U' R' F2 U D R2 U' B' R2 L D' U2
--------------------------------------------------------------------------------
[UF|54 OBTM]: (U B R' F2 R D' F2) (r U M' F R2 F' M2) (R U2 R' U' R U R') (U' F R U' R' U R U R' U R U' R' F') (M' U' M U M' U M U M U2 M')
[UR|49 OBTM]: (y) (R' U B2 D' L F L) (r' F R2 F' U R2 M') (U R' U R U R' U2 R) (U R U2 R D R' U2 R D' R2) (U M U' M' U2 M' U M2 U' M' U')
[UR|44 OBTM]: (y) (L R2 F U2 B' L D) (U R B' R B R) (B U2 B' U R' U' R) (R' U2 R F U' R' U' R U F') (M2 U2 M U M' U2 M' U2 M')
[UL|51 OBTM]: (y') (R' U B' D' B2 L D2) (M' B' R B R F R' F' M) (U' B U2 B' U R' U' R) (R' U R U2 R' L' U R U' L) (U2 M' U' M U M2 U' M' U2 M')
[DB|47 OBTM]: (x2) (L2 U' B2 R' F' R2 D2) (B' R2 B U R2 U2 R) (U R U' R' U2 F' U' F) (U2 r U2 R2 F R F' R U2 r') (M2 U' M U2 M' U M' U' M' U2)
[DB|54 OBTM]: (x2) (D U' L U2 B2 R2 F) (U' M2 U M2 F R2 F') (U R U' R' F' U' F) (U2 R U R' U F' R U2 R' U2 R' F R) (U2 M' U M2 U M' U2 M' U M' U M2)


Best solve - Roux search with orientation UR:
---------------------------------
Inspection [UR]: y

First block (7): L R2 F U2 B' L D
Second block - square (6): U R B' R B R
Second block (7): B U2 B' U R' U' R
COLL (10): R' U2 R F U' R' U' R U F'
L6E (14): M2 U2 M U M' U2 M' U2 M'

Solve metric: 44 OBTM
COLL case: U6

Total search time: 7.719670 s
First blocks search time: 5.775885 s
Second blocks search time: 0.569311 s
COLL search time: 0.002474 s
L6E search time: 1.372000 s
Threads used: 12 of 12
```

### Petrus method (petrus)

The first block is searched for each possible orientations of the cube, using at most the number of moves specified for the first block (search depth **-d[number]**). If this parameter is very low (less than 6) you will hardly find solutions. With a value of 6 or 7, results should already appear, and with 8, a result should appear for each orientation. The solution for the first block is taken among all possible ones by evaluating its number of movements, but also taking into account that the arrangement of the pieces to expand the block is as favorable as possible.

The search for the expanded block will be done in a similar way. Then the edges orientation will be performed.

The next step is to complete the first two layers (F2L), you can specify his own search depth (**-s[number]**).

Finally the last layer will be solved using **ZBLL**, **COLL**+**EPLL** or **OCLL**+**PLL** algorithms. These algorithms are predefined and their search is very fast.

### Petrus example

```
> ./ssmaster petrus -oUF -i6 ocll best -d8 -s6
```

```
Selected Petrus method (parameter 'petrus')
Orientation search defined (parameter '-oUF')
Number of inspections set to 6 (parameter '-i6')
Selected OCLL algorithms collection (parameter 'ocll')
Show best solve (parameter 'best')
Primary depth set to 8 (parameter '-d8')
Secondary depth set to 6 (parameter '-s6')
Parameters summary:
        Selected method: Petrus
        Orientation(s): UF
        Number of inspections per orientation: 6
        Selected metric: STM
        Last layer in two looks: OCLL + PLL
        Block search depth: 8
        F2L search depth: 6
        Best solve will be shown
        Cores: All
Scrambles file detected, using it for solves...
Solve 1 of 1
Searching Petrus blocks... expanding blocks... EO... F2L... OCLL... PLL... Done!

Petrus search for Scramble (20): F B' R U F' D F' B2 U' R' F2 U D R2 U' B' R2 L D' U2
--------------------------------------------------------------------------------
[UF|52 STM]: (F U' F R2 F B2 R' D) (R2 F2 R U' R2 F) (B U' B' S' U S U' F R F') (U R U2 R' U R U' R') (y R' F' L' F R F' L F) (R' U2 R U R' U2 L U' R U L' U2)
[UF|59 STM]: (R' U' F' R' D L F' L) (R F2 U' F U' R F2) (R2 S' U' S B U' B' f' U2 f) (U' R' U2 R U' R' U R) (R U R' U R U' R' U R U2 R') (R U R' U' R' F R2 U' R' U' R U R' F' U2)
[UF|44 STM]: (U2 R' U F U' L D' L) (F2 U' F R' F2) (U' F R' F' B' R B) (U2 R U' R') (y2 R' U L U' R U L') (y R' U' R y R2 u R' U R U' R u' R2 U2)
[UF|54 STM]: (U B R' L' F L F D') (F R U R' F2 R U F') (B U' B' R F' U' F S' U2 S) (U' R' U' R U R' U2 R) (y L F R' F' L' F R F') (y R2 u R' U R' U' R u' R2 y' R' U R )
[UF|51 STM]: (R' U2 R' F U D' L D') (U' F R2 U' F2) (R' S' U' S B U B' f' U2 f) (U R' U' R U R' U2 R) (y' R' U L U' R U L') (y2 R' U' R y R2 u R' U R U' R u' R2 U2)
[UF|55 STM]: (F R L B2 D' F U L') (U2 F R' F R F U F') (U S R2 S' F R2 F' S R' S') (R' U R U' R' U R) (y' R2 D' R U2 R' D R U2 R) (y R2 u R' U R' U' R u' R2 y' R' U R U2)


Best solve - Petrus search with orientation UF:
---------------------------------
Block (8): U2 R' U F U' L D' L
Expanded block (5): F2 U' F R' F2
EO (7): U' F R' F' B' R B
F2L (4): U2 R U' R'
OCLL (7): y2 R' U L U' R U L'
PLL (13): y R' U' R y R2 u R' U R U' R u' R2 U2

Solve metric: 44 STM
OCLL case: Antisune
PLL case: Gb
```

### ZZ method (zz)

The search starts trying to find one of this structures: EOLine, EOArrow, EOCross, XEOLine, XEOCross and EO223 (search depth **-d[number]**). The solution is chosen for the most complex structure, among all the possible ones, evaluating also its number of movements and taking into account that the arrangement of the pieces for F2L is as favorable as possible.

First two layers (F2L) search is fast, about 2 or 3 seconds per inspection on a modern CPU, and it will try to get the shortest possible solution for each solve.

Finally the last layer will be solved using **ZBLL**, **COLL**+**EPLL** or **OCLL**+**PLL** algorithms. These algorithms are predefined and their search is very fast.

### ZZ example

```
> ./ssmaster zz -oBL -i4 zbll best -d8
```

```
Selected ZZ method (parameter 'zz')
Orientation search defined (parameter '-oBL')
Number of inspections set to 4 (parameter '-i4')
Selected ZBLL algorithms collection (parameter 'zbll')
Show best solve (parameter 'best')
Primary depth set to 8 (parameter '-d8')
Parameters summary:
        Selected method: ZZ
        Orientation(s): BL
        Number of inspections per orientation: 4
        Selected metric: STM
        Last layer in one look: ZBLL
        EOX search depth: 8
        Best solve will be shown
        Cores: All
Scrambles file detected, using it for solves...
Solve 1 of 1
Searching EOX... F2L... ZBLL... Done!

ZZ search for Scramble (20): F B' R U F' D F' B2 U' R' F2 U D R2 U' B' R2 L D' U2
--------------------------------------------------------------------------------
[BL|EOLine 60 STM]: (z x') (R B' D2 L2 D' F' L U') (L U' L' U' R' U' R L2 U' L2) (U R' U2 R U' L U L') (U' R' U' R U' R2 U' R2) (R U' R' U' R U2 R' U R U R') (y2 R U2 R2 U' R2 U' R' U R' U' R U R' U R )
[BL|EOLine 58 STM]: (z x') (R B' D2 L2 D' F' L U) (L U' L' U' R' U' R L2 U' L2) (U R' U R U' L U L') (R U2 R' U' R' U' R U' R2 U' R2) (R' U2 R U R' U' R) (y' D' R2 U R' U' R' U R' D U' R' U R U')


Best solve - ZZ search with cube in orientation BL:
---------------------------------
Inspection [BL]: z x'
EOLine (8): R B' D2 L2 D' F' L U
F2L 1 (10): L U' L' U' R' U' R L2 U' L2
F2L 2 (8): U R' U R U' L U L'
F2L 3 (11): R U2 R' U' R' U' R U' R2 U' R2
F2L 4 (7): R' U2 R U R' U' R
ZBLL (14): y' D' R2 U R' U' R' U R' D U' R' U R U'

Solve metric: 58 STM
ZBLL case: L 13
```

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


### Common parameters - CANCELLATIONS (canc)

If the cancellation option is enabled, cancellation of movements will be applyed to each solution in order to get the lower metric (STM by default). Cancellations metric will be shown surrounded by parentheses at the begining of each solve, near the regular metric. This metric will be used for evaluate the best solve, if the best solve option is enabled.

Note: Cancellations will not be applied in LBL method.


### Common parameters - BEST SOLVE (best)
	
If this option is enabled, the best solve (with lower metric) will be shown in detail.

Note: LBL solve is unique, so this paramater is unnecessary with this method.


### Common parameters - REGRIPS (regrip)

If regrips are enabled, turns will be added to the solutions to get more comfortable movements.

Note: Regrips will not be applied in LBL method.


### Common parameters - SEARCH DEPTH(S) (-d[number] -s[number])

With the depth(s) parameter(s) you control how deep is the search of the solves. With a value of 6 you should obtain some solves (not always). A value of 7 gives you more solves and a value of 8 should give you at least one solve in each orientation (usually many more). Take care, as the search time increases exponentially with this value, for example, in my computer a value of 6 lasts around 1-2 seconds, a value of 7 around 15-20 seconds, with a depth of 8 a few minutes, and with 9 (the maximum allowed) can be an hour or more. This times are relative, as many factors have influence.

CFOP and ZZ methods uses only the **-d** parameter (for the cross and EO X respectively). Roux and Petrus also need the **-s** parameter, to specify the second block and F2L search depth, respectively.

Note: Search depths will not be applied in LBL method.


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


### Common parameters - CORES (-t[number])

This parameter specifies the amount of CPU cores (threads) used in the search. A '0' value means all available cores will be used (default).



### Common parameters - TIME (time)

If this parameter is present, the solves search times will be shown.
