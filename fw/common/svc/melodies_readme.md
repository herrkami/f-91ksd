# RTTTL (Ring Tone Text Transfer Language) format

An RTTTL ring tone can be specified in the following format:
```
<name>:d=<default duration>,o=<default octave>,b=<BPM>:
<duration><pitch><octave>,<duration><pitch><octave>,...
```

For example:
```
fifth:d=4,o=5,b=63:8P,8G5,8G5,8G5,2D#5
```

Allowed durations:
```
1, 2, 4, 8, 16, 32
```

Allowed octaves:
```
4, 5, 6, 7
```

Allowed BPMs:
```
25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90,
100, 112, 125, 140, 160, 180, 200, 225, 250, 285,
320, 355, 400, 450, 500, 565, 635, 715, 800, 900
```

Allowed pitches:
```
'p', 'c', 'c#', 'd', 'd#', 'e', 'f', 'f#', 'g', 'g#', 'a', 'a#', 'h', 'b'
```
