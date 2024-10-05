### Theme: Tiny creatures
    - simulation type thing?
    - controlling many little characters swarming around mouse
    - maybe do boids? STAR
    - Mouse controls, for simplicity

# Idea dump:
    - combat game where tons of little tiny creatures fight against bigger creatures
        - levels would be harder to defeat enemies
        - Flaw: designing complex creatures is NOT my strength.
    - Game where little creatures have to all navigate within some limited space
        - Puzzle/resource management.
        - Challenges: level system & design
        - 1 room puzzles?
    - Maybe you're trying to AVOID tiny creatures? **
        - still REALLY want boid-like swarms
        - Maybe now it's a stealth game and you traverse a top-down geometric landscape trying to avoid swarms
        - Level-based stealth game

# Things to focus on:
- musical dynamic (strength)
- more code complexity (algorithms, etc.)
- SIMPLE art
- polish ONE thing to the EXTREME.
- For once, make a menu that isn't ASS.

# Things to avoid:
- ANYTHING REQUIRING DRAWN ANIMATION (WILL NOT TURN OUT WELL)
- COMPLEX ART
- *Anything* that a game engine can do easily but will take a looooong time here
    - Tiled level systems, advanced animators, bone animation, 3D

# Level system prototype format:
goalX, goalY
startX, startY
hitboxCount
hitbox1x, hitbox1y
... (continue)
boidSpawnersCount, totalBoids
spawner1x, spawner1y, count
... (continue)

# Goals of 10/5/25:
    - Finish level system
    - Player art, background art
    - Do as much w/ music & sound as I can
    - Start levels
