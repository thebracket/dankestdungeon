# Dankest Dungeon

Dankest Dungeon is a 7-Day Roguelike, written in Emscripten (a C++ to JavaScript compiler) and NodeJS. It started out as a tech demo, but it has proven mildly popular - so I may well keep hacking away at it.

You can play it on my server, [http://dankest.bracketproductions.com/](http://dankest.bracketproductions.com/)

**How to play**

Go to [http://dankest.bracketproductions.com/](http://dankest.bracketproductions.com/), and create an account (or login if you have one). The main menu should be self-explanatory (keys or mouse) - (1) to create a dungeon, (2) to play one.

*Designing a Dungeon*

The red options on the right are categories, which activate palette entries such as floors, walls and toadstools. Whatever is highlighted on the right pane will draw onto the map on the left when you click it. You *must* have an *Amulet of Winning* and a *Player* on the map (1 of each). Other than that, you can do whatever you like. It's a 100x100 canvas, so you should be able to build a convoluted dungeon if you want to.

Map Elements:

* Floors (`.`) are walkable.
* Walls (`▒`), Toadstools (`♠`), Vegetation (`♣`) block visibility and progress and cannot be penetrated.
* Pools (`~`) cannot be crossed, but do not block line-of-sight.
* Doors (`+`) block line of sight, but can be destroyed with 1 action.
* Iron Doors (`!`) block line of sight and progress, but can be destroyed IF you have an Iron Key (`∞`).
* Gold Doors (`≡`) block line of sight and progress, but can be destroyed IF you have a Gold Key (`φ`).
* Kobolds (`k`) are a nuisance. They hit for minimal damage, and can be easily slain. They path towards you if they can see you, otherwise they remain stationary.
* Orcs (`o`) are more of a threat. They hit for moderate damage, but can be easily slain. They path towards you if they can see you, otherwise they remain stationary.
* Dragons (`D`) kill you instantly if they get to you! They cannot be slain. They path towards you if they can see you, otherwise they remain stationary.
* Pit Traps (`⌂`) and Blade Traps (`±`) inflict damage if you walk into them. They are invisible unless you fall into one, or WAIT for a turn with them in your visibility map.
* Mapping Scrolls (`═`) reveal the entire map.
* Healing potions (`σ`) give you some hit points back.

When you are ready, hit *playtest* (or `.`).

*Playtesting*

Within playtest mode, you are playing the game. You can move around with the cursor keys (or vi keys, or numpad). Diagonal movement works. Press `.` to wait, which also reveals any traps within your visibile area. If you want to change your map, hit `ESC` - and return to the level editor. You can cycle between these modes as much as you like - but you will restart the dungeon each time you playtest.

When you hit the amulet (¥), and win the game. It kicks you back to the designer with a message that you have beaten the dungeon and can save it by pressing `ESC`. This prompts you to name your dungeon, and it is now available for other players to try.

*Playing People's Maps*

Option 2 on the main menu presents you with a list of every submitted map. You can use page up/down if more than a screen-load arrive. Click a map to play it. Play is exactly the same as playtesting above, but when you finish you are asked to submit a rating from 0-5 for how much you liked the map.

## Setting up your own dungeon server

The build scripts are all Windows-based, since that what I was using. You also need a PostgreSQL database server available.

The file `postgres.sql` contains the database schema. You'll want to edit `node/dankest_dungeon/routes/database.js` to provide the URL to your database.

Next, install [Emscripten](http://kripken.github.io/emscripten-site/). Take note of where it is installed, and modify `cpp/build.bat` to point to the appropriate path for `emsdk_env.bat`.

You can now `cd` to the `cpp` directory and build with `build.bat`. This compiles the game, and copies the results into your `node/dankest_dungeon/public` folder.

Finally, you can run the game by changing directory to `node/dankest_dungeon` and running `npm start`. (If you haven't already, run `npm install` first to grab packages). You can then go to your game at *http://localhost:9000/*
