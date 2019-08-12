Entity Managers can help us handle all the models and textures
Each Entity has its own Mesh, Texture, Descriptor Set, and Uniform.
Buffer. A lot was moved around to make this work.

Demo.cpp has never been easeir to follow, feel free to add
more entities into the world, add as many as you'd like.
Adding entities will require adjustments in prepare_descriptor_pool
to adjust for additions of buffers

One problem is that textures look grainy and staticy, this can be 
fixed with Mipmapping and Anisotropic Filtering, which is much more
difficult in Vulkan than the 5-line example in OpenGL. Another
problem is that our Skybox does not have its own pipeline (like OpenGL),
this will be fixed in future tutorials

vertexBufferCPU, vertexBufferGPU, indexBufferCPU, and 
indexBufferGPU have been moved into the Mesh class

textureBufferCPU and textureBufferGPU have been 
moved into the Texture class

matrixBufferCPU and descriptor_set have been moved to Entity.

Bonus Challenge:
Make a three vectors of pointers
One to hold all Entity pointers
One to hold all Texture pointers
One to hold all Mesh pointers
When you want to delete all meshes, or textures,
or when you want to draw all entities, or when you
want to delete all entities, you can use the vectors
to loop through every element, rather than calling
them one-by-one