# Overview
This project is a scene demonstration programmed using OpenGL libraries in C++. The scene is based around a ship floating in the sea, demonstrating the use of model and texture loading, MVP transformations and fragment shader manipulation.

# YouTube demonstration
https://www.youtube.com/watch?v=QC8owuEBWY0

# GitHub link
https://github.com/tfoxcroftply/COMP3016_CW2

# Interaction
In the submission repository, there will be two folders, one containing the compiled files, and the visual studio source file. The release directory will contain a compiled executable of the program. The user must open this file, and then a scene will be displayed if all goes accordingly.
This program contains simple controls which allows the user to move and manipulate the scene. The movement is based around a “camera”, and it is up to the user to fly this camera around. 
There are basic “WASD” controls which transforms the camera left, right, forward, and backwards relative to its aimed position. There are additional “Q” and “E” controls which enables the user to fly the camera up and down the world’s Z plane.
To direct the camera to a given angle, the user must use the mouse. The mouse in unrestricted and not constrained to the window, so large movements can be made without the mouse exiting the window and causing problems.
To exit the game, the user can press the escape (ESC) key which will request the program to terminate.
This program is merely just a scene, with no game mechanics involved. The user is free to interact with this project as they wish.

# How the program works
The main program has two stages, the initialisation stage, and the render loop stage.
The initialisation stage is necessary as it defines any settings required, loads in shaders, models, textures, creates necessary objects from classes and more. It sets the program up for the render loop which is run continuously at the end.
Such objects include the camera and the models themselves. The camera contains its own position data and will be retrieved in the render loop to be sent to the fragment shader. Model objects have the necessary data required for rendering, such as VAO index, textures, textures to mix, and transformation. The model class does not create VAOs, VBOs or EBOs, as this is done by ProceduralGeneration.cpp, QuadLoader.cpp and ModelLoad.cpp. They simply return the referenceable “ObjectData” (struct) for storage. The model class also does not internally handle texture loading, it simply just stores the returned texture location from the “GenerateTexture” command from “Textures.cpp”. Encapsulation is used here, so the texture location cannot be written directly by the user, which could cause errors.
After initialisation, the render loop will run forever until interrupted by the user. This render loop iterates through all stored model objects from before and triggers a render for each. The render loop also is tasked with capturing keyboard commands and sending it to the camera object, which in turn will apply transformations to itself accordingly. Timed movements and transformations are calculated in the render loop based on frame times, to ensure movement is not fully reliant on frame rate.
As mentioned before, models retrieve their data from either one of three functions, QuadLoader, ModelLoad or ProceduralGeneration. These functions return the VAO and number of vertices associated with that model. These two values are required for drawing. The model loading operation was based off http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/. The models and textures were created by me.
Additional things include a logging solution, which creates log files relative to the executable’s directory. Although not required, I included it from another project of mine, as it is helpful. This helps me understand if there any errors present, and if so, what the error is.
I did not create the Shader loader, as this was provided in labs and was recommended to be used. The QuadLoader.cpp object data is derived from Lab 5.

# Ideas, speciality and baseline
I don’t consider my program to be truly special as it is rather simple, however some good code practices were applied to make it less repetitive and more efficient. The program makes use of “Object Oriented Programming”, as used for the camera and models. A frame limiter was added to ensure that the GPU isn’t pushed to its limits by rendering as many frames as possible in one given moment, so this also contributes to efficiency.
This project was initially based a demonstration shown in the first lesson of COMP3016, an underwater scene floating around a shipwreck. Having this as inspiration let me build up ideas over the weeks. As I was inexperienced with OpenGL programming at that time, having a scene as a project suited me. It was already proving to be a challenge to get things rendered, let alone having to implement game mechanics.
However, upon submitting the pitch, it conflicted with someone else’s project, so I had to reconsider. I adapted my pitch to have the signature, the ship, above water instead, which was approved.
My code started from Lab 5, having a simple quad loaded with a texture on top. I repurposed this to be the ocean surface, although this was later scrapped as my skills developed and I was able to procedurally generate instead. The procedural generation function generates as quads, as having a quad displayed is required to pass. Similar texture loading is used for the ocean and sand.
To make mine unique, I wanted to do a little bit of fragment shader manipulation. If the camera goes below water, the scene changes. The backdrop turns to a darker blue, and all textures will have a blue filter applied to them, mimicking an underwater view. 
Additionally, I added some simple movements to my project to make the scene less static. Every model loaded in has some sort of timed animation applied to it, an example being the ship wobbling in the water.
