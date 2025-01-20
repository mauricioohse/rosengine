# Mr Nuts flow

To Claude:  
I want to to help me build the following game. I will send you my game design document, and in the end of it, there will be a step by step of milestones. Do not generate any code yet. Just read the document, do a summary of the document and ready yourself to start working on the milestones one by one.

# Introduction

Mr Nuts flow is a game about a flying squirrel (Mr Nuts) that jumps from a helicopter and goes flying down dodging clouds to try to reach the ground in the fastest time. The game has vertical

It has some a little unconventional controls and is a game which can be speedrun. The playtime is expected to be about 1 minute for a very fast player, and about 5 minutes for a slow player.

# Main mechanics

## Squirrel

The squirrel starts the game dropping from a helicopter. 

Squirrel physics:

* Has transform and motion physics  
* Camera always centers the Squirrel  
* He has a max speed dependent on its state  
* He is affected by gravity  
* If his speed is greater than his max speed, he decelerates  
* When rotated, his speed rotates too

Squirrel has three states while flying:

* Open arms  
  * Open arms squirrel max speed is 100 px/s  
  * If left arrow is tapped, he rotates a bit to the left  
  * If right arrow is tapped, he rotates a bit to the right  
  * Holding right/left arrow has no effect, to rotate him, it requires tapping  
  * If space is pressed, he goes to closed arms state  
* Closed arms  
  * Closed arms max speed is 300 px/s  
  * No control of rotation   
  * He has an acceleration boost in the direction he is pointed to besides gravity  
  * If space is released, goes back to open arms  
* Wiggling  
  * When squirrel collides with a white cloud, he enters wiggle state  
  * While wiggling, he decelerates for one second, up to 50 px/s  
  * After leaving wiggling, he has a grace period of three seconds where he can't wiggle again

Besides the Squirrel, there is only one other game object, the cloud

## Cloud

There are multiple clouds spaced around the map. There are two types of clouds:

* White cloud  
  * Has 4 possible sizes  
  * Is static (don\`t move)  
  * When colliding with player, sets player to wiggle  
* Black cloud  
  * Has 4 possible sizes  
  * Is static  
  * When colliding with player, makes player bounce depending on hit direction

# Technical requirements

The game is written using my partially done RoseEngine. It follows ECS architecture and compiles to Windows via G++ and Web via Emscripten. 

I prefer writing code similar to C. It does not use ANY STL library. Repeat: DO NOT USE ANYTHING FROM C++ STL. This means no templates, none of the modern C++ features.

## Final message to Claude

Based on this document, write a summary of the game, the objects, the technical approach.