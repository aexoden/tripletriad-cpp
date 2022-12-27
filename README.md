# tripletriad

This program is primarily useful as a solver for Triple Triad games in Final
Fantasy VIII.

## Development History and Notes

This project began life as a project for an artificial intelligence class I took
in approximately 2004. The goal was to write an AI to play Othello. Later on,
during a Final Fantasy VIII playthrough, I decided to repurpose much of the code
to instead help me play Triple Triad.

Some of the early development did not take place under version control, so the
exact history is lost. Based on some of the commits, it looks like this was at
least a partial rewrite. Most work was completed in 2010, with only a few bug
fixes happening since then.

There is no real active development happening here, though if I discover a bug
in the implementation of the rules, I'll most likely fix it. Any future
development would likely be in the form of a complete rewrite.

## Building

This software uses a standard autoconf/automake build system, so compilation
should be relatively pain-free. SDL and SDL_gfx are both required. In order to
build, simply execute the following commands:

```
aclocal
autoconf
automake --add-missing
./configure
make
```

Any custom configuration is beyond the scope of this README.

## Usage

As this was designed for my own use, it is not a very user-friendly program. To
play a game, you must create a data file to describe the game. A sample one is
provided in the repository, and here is a brief description of each line.

The first line indicates which player should go first: B for blue, and R for
red. Note that without editing the code, the computer always controls blue.

The second line activates various rules. There are four fields, which should
each either be a 0 (to deactivate the rule) or 1 (to active the rule). The four
rules in order are same, plus, same wall and elemental.

The next line should be blank.

The next three lines describe the elements on the game board. If the elemental
rule is inactive, this section is irrelevant, but must be included. A hyphen
indicates no element. The remaining elements are F (fire), I (ice), T (thunder),
P (poison), E (earth), A (wind), W (water) and H (holy).

The next line should be blank.

The next five lines describe the blue player's cards. Each card is five fields.
The first four are the four numbers/letter of the card going from top, bottom,
left then right. The final field is the card's element.

After those five lines, there should be another blank line, and then five more
lines to describe the red player's cards.

Practically speaking, this tool is most useful for guaranteeing a win when
playing Triple Triad within Final Fantasy VIII. This is only effective with the
open rule active. No other rules particularly matter, though random can make it
more difficult to win (though having same/plus/same wall available can mitigate
that). The basic procedure is as follows:

1. Start the in-game match and select your cards (or have them selected in case
   of random). Once all the cards area available, it's time to create the data
   file.
2. Create the data file to describe the current match. You need to change who
   the first player was, update the rules, update any board elements, enter the
   computer player's cards, and update your cards if you selected different
   ones. You only need to bother entering elemental data if the elemental rule
   is active.
3. Run the program via ```./tripletriad FILENAME``` where FILENAME is the data
   file you've created.
4. On human turns, the program will automatically determine the optimal move.
   The game is simple enough to be completely solved within a few seconds, so
   you will know shortly what the final result will be (barring bugs or mistakes
   on your part). The number to watch is the Utility value at the highest ply
   level. A positive value is good for you, a zero means a draw, and a negative
   value is bad for you. The played card will be highlighted so you can see
   what was done or you can review the Move at the highest ply level.
5. On computer turns, you will need to enter the move made by the computer by
   clicking the appropriate card and then clicking the square the card was
   placed. The GUI is not very friendly, and if you click outside the correct
   bounds, the tool may crash. After placing the card, the tool will immediately
   begin determining the next optimal move. The first round will take the
   longest to analyze. The remaining ones should be almost instant.
6. When the game is over, you can press ```q``` to quit.

If it is possible to win, if you've entered the data correctly, and if you
execute the moves correctly, you are (barring a bug) guaranteed to win. I have
fixed some rare bugs with the combo rules in certain situations fairly recently,
so it's possible bugs still lurk, but I don't expect any more.

## Authors

* Jason Lynch (Aexoden) <jason@calindora.com>
