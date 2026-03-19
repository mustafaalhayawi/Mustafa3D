# Mustafa3D
A custom-built 3D graphics engine written in C++ running entirely on the CPU

## Visual Proof
<p align="center">
  <img src="https://github.com/user-attachments/assets/b76c4737-8fd1-481e-82a9-8e379e18c60f" alt="Monkey Head Demo">
  <br>
  <i>A monkey head in the graphics engine demonstrating Phong lighting, .obj loading, and camera movement.</i>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/6cd7874c-45b0-42b6-a952-d42103fe0c10" alt="Police Car Texture Demo">
  <br>
  <i>A low-poly police car demonstrating perspective-correct texture loading.</i>
</p>

Click [here](https://youtu.be/06yZVdD_Eck) for a short explanation of the graphics engine.

## Core features
- Z-buffering: Ensures triangles that are closer to the camera are rendered as being in front of other triangles to prevent overlapping
- Phong lighting: Lighting that includes ambient, diffuse and specular components to create relatively realistic lighting
- .obj loader: Loads .obj files of different types, including vertices with no normals/textures and creates normals if they don't exist and then storing it in a Mesh struct I created
- Texture mapping: Using UV mapping to accurately display a 2D texture on a 3D model

## Technical challenges
### Winding orders
I began the project by rendering simple spinning cubes. For a triangle to be rendered the order of its vertices must be anticlockwise otherwise it isn't rendered. This is intended and is called back-face culling which is a method in computer graphics to make rendering objects more efficient. Implementing the order of the vertices of the cube's faces was a long process where I made a few mistakes that were difficult to solve as I didn't know which faces were right and wrong in the code.
### Camera movement
In computer graphics if you want to move the camera you don't really move the camera but instead shift the world relative to the camera which was something I learnt. This however made sense and the maths of a moving camera was just a small extension to the maths I used previously for the stationary camera.
### Optimisations
At the start of the project I was using barycentric coordinates to render triangles. This process isn't the most efficient because it means I need to check every pixel in the rectangle that contains the triangle to determine if they lie in the triangle. I moved to using scanline rasterisation where I only loop over the pixels I know will be rendered and this gave a noticable performance boost. But as I moved to Phong lighting I couldn't keep using scanline rasterisation as it made perspective-correct interpolation of normals incredibly difficult. So I returned to using barycentric coordinates which made it mathematically easier to interpolate but at the cost of raw performance.

## How to build and run
- Prerequisites: C++ and Visual Studio
- Dependencies: SDL3 and stb
- Make sure to update Visual Studio project settings to link the .h files and the include folders

## Controls
- W/S: move forward/backward
- A/D: move left/right
- Q/E: move down/up
- Arrow keys to look left, right, up, and down
- Movement forward backward left and right is done relative to the direction the camera is facing, not the world axes.

## Next steps
- Implement near-plane clipping
- More performance optimisations
- Create a scene manager to have multiple objects
- Lighting manager to support multiple lights
- Support different types of lights, e.g. directional, point, etc

## Credits
Built for Hack Club Flavortown
