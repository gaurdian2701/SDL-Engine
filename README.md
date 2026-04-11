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

I currently have the following systems implemented:
- A Rendering System that uses SDL for rendering textures on an SDL_Rect. It makes use of the Transform and Renderer2D Components.
- A Particle System that renders and simulates particles. I'd thought I could implement this since I had to program spaceships in my game anyway.
- A Debug System which shows visual debugs. For now it only debugs the collider bounds for AABBs and Circle Colliders.
- Physics System. For now this only detects overlaps between colliders.

## Physics Implementation

Physics Code can be found in the following folders:\
[Physics Headers](include/Core/ECS/Systems/Physics)\
[Physics Source Files](src/Core/ECS/Systems/Physics)

- Currently I am implementing a naive broad phase that uses AABB testing to produce collision pairs.
- To make colliders more generic to work with each other, I am currently refactoring my collider component so that the user has to use only one collider component and then specify the type of collider inside of the collider struct.

(REFACTORING - NOT IMPLEMENTED YET)
- Each collider struct would simply store information about the collider type. The Physics system would own sparse sets of each type of collider. So instead of directly storing different types of colliders, I have decided to go with one collider component, whereas the physics engine would store the shape data(i.e. circle, box, polygon, etc.) in sparse sets.
- The user would have to directly use the Physics System to make changes to shape data. The collider component is simply a data handle to the shape data that the Physics System would own and manage.


