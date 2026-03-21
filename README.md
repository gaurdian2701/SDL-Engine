# Entity Component System-based SDL Wrapper with a 2D Physics Engine

This is a project made using C++ 20, SDL3 and CMake.\
The following are links to my **ECS Code** .h and .cpp files in this repository:\
[ECS Headers](include/Core/ECS)\
[ECS Source Files](src/Core/ECS)

If you want to take a look at the Components, [here is a link to them](include/Assets/Components).

The folders contain code for:
- An ECS Manager which handles creation, deletion and querying of components.
- Sparse Sets.
- Systems that use the ECS.
- A Component Factory. This simply creates component instances for each gameobject.
- Logic for an Asteroids game. I made this game as part of my assignment for my studies.

I currently have the following systems implemented:
- A Rendering System that uses SDL for rendering textures on an SDL_Rect. It makes use of the Transform and Renderer2D Components.
- A Particle System that renders and simulates particles. I'd thought I could implement this since I had to program spaceships in my game anyway.
- A Debug System which shows visual debugs. For now it only debugs the collider bounds for AABBs and Circle Colliders.
- Physics System. For now this only detects overlaps between colliders.

## Physics Implementation

Physics Code can be found in the following folders:
[Physics Headers](include/Core/ECS/Systems/Physics)\
[Physics Source Files](src/Core/ECS/Systems/Physics)

- Currently I only have a Narrow Phase which naively iterates through all the colliders, checks if they are in contact and then resolves collisions.
- Basic Circle Collider collision resolution is done right now. 


