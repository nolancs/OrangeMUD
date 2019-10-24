# OrangeMUD
Advanced multi-user dungeon (MUD) codebase built in C++. Features an extensible C-like scripting language, a full suite of online creation tools, and much more. Runs natively on Mac (OS X and pre OS X), Windows, and Unix. Custom built by Matthew Nolan for Age of Wonder MUD circa 2000.

## Features 

* Powerful and easy to use OLC (**O**n**L**ine **C**reation) tookit.
* Scripting through OLC using pure C syntax.
* Polymorphic object-oriented XML file system.
* Complete built-in ANSI color code and formatting support.
* Plural object and mobile system for condensed output.
* Quest system through scripting.
* Free-form text editor and width auto-formater (mimicks The Forest's Edge.)
* Complete part-based body maps with ACs and sizes for each.
* Custom builder-defined mobile attacks.
* Physical, Mind, Cold, Fire, Acid, and Electrical damage types.
* Noteboard and mail system.
* Much much more...


### C-Like Scripting Language...
Sample Dartboard script from the Charging Boar Tavern in Del'Shava. This script was written through the OLC system and can be changed and recompiled on the fly without having to reboot the MUD.

```
[ 1] INT throw, hit, score;
[ 3] STRING grabch, grabro, throwch, throwro;
[ 5] 
[ 7] grabch = "You grab the three darts and step back to the line on the floor.";
[ 9] grabro = "$n grabs three darts off $p and steps back to the line on the floor.";
[11] act_to_char(grabch, ch, obj, NULL);
[13] act_to_room(grabro, ch, obj, NULL);
[15] wait(1);
[17] throw = 1;
[19] score = 0;
[21] while(throw <= 3)
[23] {
[25]    if(throw == 1){
[27]       act_to_char("You throw the first dart.", ch, obj, NULL);
[29]       act_to_room("$n throws the first dart.", ch, obj, NULL);
[31]    }else
[33]    if(throw == 2){
[35]       act_to_char("You throw the second dart.", ch, obj, NULL);
[37]       act_to_room("$n throws the second dart.", ch, obj, NULL);
[39]    }else
[41]    if(throw == 3){
[43]       if((random(1, 3)) == 1){
[45]          act_to_char("You offer a prayer in hope of a good throw.", ch, obj, NULL);
[47]          act_to_room("$n mumbles a little prayer under $s breath.", ch, obj, NULL);
[49]          wait(1);
[51]       }
[53]       act_to_char("You throw the last dart.", ch, obj, NULL);
[55]       act_to_room("$n throws the last dart.", ch, obj, NULL);
[57]    }
[59] 
[61]    hit = random(1, 4);
[63]    if(hit == 1){
[65]       send_room("It hits the bullseye!! 10 points!", room);
[67]       score += 10;
[69]    }else
[71]    if(hit == 2){
[73]       send_room("It hits the inner ring, 5 points", room);
[75]       score += 5;
[77]    }else
[79]    if(hit == 3){
[81]       send_room("It hits the middle ring, 3 points", room);
[83]       score += 3;
[85]    }else
[87]    if(hit == 4){
[89]       send_room("It hits the outer ring, 1 lousy point", room);
[91]       score += 1;
[93]    }
[95]    if(throw != 3) wait(1);
[97]    throw = throw + 1;
[99] }
```

### Plural Object and Mobile System...
This system cuts down on spam by letting builders set special plural strings for mobiles and objects. The MUD uses these strings to generate a single line where many of the same would have previously been repeated. The before and after result is shown here...

Before
```
> get all
> You get a white silken shirt.
> You get a white silken shirt.
> You get a white silken shirt.
> You get a rabbit skin.
> You get a rabbit skin.
> You get a blackish wood tankard.
```

After
```
> get all
> You get three white silken shirts.
> You get two rabbit skins
> You get a blackish wood tankard.
```

# Installation

Requires the following software packages to be installed (for Unix/OSX)
* makedepend
* gcc/g++

To build
```
make
```

To run
```
./OrangeMUD
```

To connect, grab a dedicated MUD client or use
```
telnet localhost 8888
```

To login use
* Name: `admin`
* Password: `orangemud`

That's it! 

If you are unfamiliar with MUDs here are some basic commands. Enter them in full or partially (eg: east or just e)
* `north`, `south`, `east`, `west`, `up`, `down`: to move
* `commands`: to show a list of available commands (immortal/builder commands are in bold)
* `equip`: to see what you're wearing
* `score`: to show your character's score/stat information
* `look <target>`: to look at something
** `look` or `l` with no target will look at the room you're in.
** `look self` looks at your character
** `look ring` looks at a ring you're carrying/wearing, etc. 
*** You can look at specific spots on things if the builder has added them. `look ring gemstone` would look at the gemstone on the ring. 
* Targets can also be multi-word such as `look 'town guard'`, encased in either double quotes or single quotes.
