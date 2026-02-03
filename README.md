# Maya L-System Plugin

## command based generation

For command based generation, registered a custom maya command (LSystemCmd) through plugin entry points (initializePlugin/unintializePlugin). 

The following command example accepts parameters such as angle(-a), step(-s), iteration count(-i), and grammar file path(-g). 

```
 SystemCmd -a 90 -s 1 -i 0 -g "F-F-F-F\nF->F-F+F+FF-F-F+F\n";
```

For easier access, go to top menu entry **(LSystem → Open…)** that opens a file picker and forwards the selected grammar path to the command, or falls back to a built-in default grammar if the user clicks OK without specifying a file.

## node based generation

For node based generation, the generated structure which updates as time progresses.

It exposes attributes including angle, step, grammarpath, time and outputmesh (used cylinder). 

For quicker access, go to top menu entry **(Lsystem Node -> Create Node)**, which will automatically generate a node, drag on timeline to see how it evolves over time. 

If you would prefer Mel command use:

```
createNode LSystemNode; 

createNode mesh -n receiverShape; 

connectAttr LSystemNode(n).outputMesh receiverShape.inMesh;
```

In attribute editor, you could change the angle, step, grammar file (string to the file using absolute path), and time. 


