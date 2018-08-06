# StoryRender
A simple interactive story render engine.

To build just use make 

To try the cheesy sample story use ./story_render adv2test.txt

Symbols in the story text are as follows
   ":" define a lable. Ex :go_north
   "?" define a question. Ex ?North:go_north
   "#" define a goto. Ex #go_north

Look at the example story for more info.

Labels are skipped. Watch for infinite loops with gotos (#). No spaces around labels and control characters must be first character on the line.

Typical format below

:start
Some text to display
Show again?

?yes:start
?no:end

:end

The end!
