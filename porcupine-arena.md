# Mr. You\!\!\! on ice

# To AI helper:

Following is the game design document for my game porcupine arena. I want you to read and summarize what are the components and systems needed to develop it in a ECS pattern. Generate a list of milestones for the development of this game.

# Summary

Game jam Theme: you are the weapon

A top down circle fighter pushing in an icy sliding arena. Play as a porcupine battling with balloons in a top down view of the arena. Objective of the balloons is to push the porcupine out of the arena, while the porcupine can only move by the recoil of shooting his quills.   
Genres: action, reaction, physics, fighting

## More details:

Balloons come all sides and the porcupine can only move by shooting his quills. Enemies are balloons: green ones push you after touching, red ones shoot horizontal laser, blue ones shoot aimed slow laser shots. Porcupine and higher level balloons have health and knockback like smash brothers (taking damage ups %, then gets pushed harder and harder). Player loses if the porcupine leaves the arena area.

## Technical descriptions

## Screen

* 600x480 pixel wide

## Camera

* Mostly centered at the porcupine  
* Follows the porcupine with a small delay

## Arena

* Circular arena  
* Icy physics: all entities slide on top of it  
* Has 1080p radius  
* Player loses if porcupine leaves the arena

## Porcupine

* Player controlled by shooting his quills  
* Shoots quills on mouse left click  
* Shoots quills on mouse directions  
* Recoils on opposite direction from quill shooting  
* Has a % health bar that indicates knockback from enemies

## Enemies

All balloons when touching the porcupine explode and causes knockback

### Red balloon

* Basic enemy, just wanders around somewhat randomly  
* Only attack is touching the porcupine and exploding

### Green balloon

* Every second, shoots slow moving laser circle projectile in orthogonal or diagonal directions  
* Moves in a circle around the arena

### Blue balloon

* Every 3 seconds, shoot a quick projectile

# To AI helper

Now that you saw the game, do a summary of what the game is and generate the appropriate milestones for developing it.

Game Summary  
Porcupine Arena is a top-down physics-based arena fighter where the player controls a porcupine that can only move by shooting quills (recoil-based movement). The player battles against different types of balloon enemies while trying to stay within a circular icy arena. The game features a knockback percentage system similar to Super Smash Bros.  
