#include "Mesh.h"
#include "Vertex.h"
#include <stdio.h>
#include <vector>


Mesh::Mesh(char* file, Demo* demo)
{
	// Part 1
	// Initialize variables and pointers

	// array of vertices
	// This will stay NULL until part 3
	VertexStructure* vertList;

	// File that we open, the OBJ file
	FILE *f = fopen(file, "r");

	// This is temporary data, we will use this to hold
	// data that we get from each line of code
	float x[3];
	unsigned short y[9];

	// this holds all possible positions,
	// texture coordinates, and normals
	// that are scattered throughout the flie
	std::vector<float>pos;
	std::vector<float>uvs;
	std::vector<float>norms;

	// This tells us which position, uv, and normal
	// belong together to form each vertex
	std::vector <std::vector<unsigned short>> vertices;

	// This holds each line that we read
	char line[100];

	// We make a vector of indices
	// These indices will determine which vertices
	// to connect for each triangle. It will connect
	// the first three indices into a triangle, and 
	// then the next three, and so on.
	std::vector<uint16_t> indices;


	// Part 2
	// Fill vectors for pos, uvs, norms, and faces

	// Keep checking lines until we finish checking
	// every line in the file
	while (fgets(line, sizeof(line), f))
	{
		// Take in the position if you detect a 'v'
		if (sscanf(line, "v %f %f %f", &x[0], &x[1], &x[2]) == 3)
			for (int i = 0; i < 3; i++)
				pos.push_back(x[i]);

		// Take in the texture coordinate if youd etect 'vt
		if (sscanf(line, "vt %f %f", &x[0], &x[1]) == 2)
			for (int i = 0; i < 2; i++)
				uvs.push_back(x[i]);

		// take in a normal if you detect a 'vn'
		if (sscanf(line, "vn %f %f %f", &x[0], &x[1], &x[2]) == 3)
			for (int i = 0; i < 3; i++)
				norms.push_back(x[i]);

		// create a face with three vertices when you detect an 'f'
		if (sscanf(line, "f %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu", &y[0], &y[1], &y[2], &y[3], &y[4], &y[5], &y[6], &y[7], &y[8]) == 9)
		{
			// three vertices in one face
			for (int i = 0; i < 3; i++)
			{
				// create a vector for each vertex
				std::vector<unsigned short> f;

				// use what is important to this vertex
				for (int j = 0; j < 3; j++)
					f.push_back(y[3 * i + j] - 1);

				// By default, our vertex is not found
				// in our vertex buffer, which means it will
				// be added to the vertex buffer as a new vertex
				bool found = false;

				// should be replaced with Binary Search
				// array should be in order of vertices[j][0]

				// This loop will check the entire vertex buffer,
				// every time it wants to add a vertex, to see if 
				// the vertex is already there. This is slow, but 
				// by removing repeating vertices in our vertex buffer,
				// it will take less time for the vertex shader to draw,
				// and draw our model more efficiently

				// If anyone finds a faster algorithm, PLEASE post it

				// If you want, you can remove this 'for' loop, and
				// the model will draw just the same as it does now,
				// except the frame-rate will take a hit, and your
				// vertex buffer will be larger. With one OBJ, it is not
				// noticable, but with 1000 OBJs, FPS does get impacted


				for (int j = 0; j < vertices.size(); j++)
				{
					if (vertices[j][0] == f[0])			// if positions are equal
						if (vertices[j][1] == f[1])		// if uv coords are equal
							if (vertices[j][2] == f[2]) // if normals are equal
							{
								// we found that the vertex already exists
								found = true;

								// add this index to the index buffer
								// to reuse this vertex
								indices.push_back(j);
								break;
							}
				}

				if (!found)
				{
					// add to the index buffer, give it the last
					// member of the current vertex buffer, because
					// we are putting our vertex at the end
					indices.push_back((int)vertices.size());

					// To Do:
					// do binary search to find where
					// this should be placed, so that
					// the vector in order of f[0]

					// For now:
					// Put it at the end
					vertices.push_back(f);
				}
			}
		}
	}

	// Part 3
	// Initialize more variables and pointers

	vertList = new VertexStructure[vertices.size()];

	// Part 4
	// Build Vertex List

	for (int i = 0; i < (int)vertices.size(); i++)
	{
		// make a blank vertex
		VertexStructure v;

		// Set the position of this vertex
		for (int k = 0; k < 3; k++)
		{
			int coordIndex = 3 * vertices[i][0] + k;
			v.position[k] = pos[coordIndex];
		}

		// Set the UV of this vertex
		for (int k = 0; k < 2; k++)
		{
			int uvIndex = 2 * vertices[i][1] + k;
			v.uv[k] = uvs[uvIndex];
		}

		// Set the normal of the vertex
		for (int k = 0; k < 3; k++)
		{
			int normalIndex = 3 * vertices[i][2] + k;
			v.normal[k] = norms[normalIndex];
		}

		// flip the Y axis of the UV
		// for some reason, UVs in 
		// all OBJs are upside-down
		v.uv[1] = 1 - v.uv[1];

		// put our vertex into the array
		vertList[i] = v;
	}

	// close the file that we opened
	fclose(f);

	// Part 6
	// Build our buffers

	// Create empty creationInfo
	// we will be re-using this several times
	// Leave it empty for now
	VkBufferCreateInfo info = {};

	// Vertex Buffer
	//=====================================

	// The size of our Vertex Array, will be the amount of 
	// elements (36) multiplied by the size of one vertex
	uint32_t vertexArraySize = (uint32_t)vertices.size() * sizeof(VertexStructure);

	// Lets make a buffer that is designed to be
	// a source (TRANSFER_SRC), that is large enough
	// to hold our vertex buffer
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	info.size = vertexArraySize;

	// make the buffer, and store the data into the buffer
	// For more information on how this works, look at BufferCPU.cpp
	// Learning about BufferCPU is optional
	vertexDataCPU = new BufferCPU(demo->device, demo->memory_properties, info);
	vertexDataCPU->Store(vertList, vertexArraySize);

	// This next buffer will be on the GPU, it is designed to be a 
	// destination (TRANSFER_DST) for our data, and it will also be
	// a VERTEX_BUFFER, which allows us to bind this buffer as a
	// vertex buffer when we set up the pipeline
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	info.size = vertexArraySize;

	// build the buffer, and make a command to send the data from the CPU 
	// buffer to the GPU buffer. This command will be added to the initCmd
	// command buffer, and the data will finally be copied from CPU to GPU
	// when we execute the initCmd command buffer (later in prepare).
	// After that, we will delete the CPU buffer, becasue it won't be
	// needed once the data is copied to GPU.
	// For more information on how this works, look at BufferGPU.cpp
	// Learning about BufferGPU is optional
	vertexDataGPU = new BufferGPU(demo->device, demo->memory_properties, info);
	vertexDataGPU->Store(demo->initCmd, vertexDataCPU->buffer, vertexArraySize);

	// Index Buffer
	//=====================================

	// The size of this index array will be the number
	// of elements, multiplied by the size of one element
	uint32_t indexArraySize = (int)indices.size() * sizeof(uint16_t);

	// This is the Index CPU buffer, so we useTRANSFER_SRC 
	// because we will copy the buffer to GPU
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	info.size = indexArraySize;

	// Just like before, we make the buffer and we store data into it
	indexDataCPU = new BufferCPU(demo->device, demo->memory_properties, info);
	indexDataCPU->Store(indices.data(), indexArraySize);

	// This is the Index GPU buffer, so we use TRANSFER_DST 
	// because we will get data from CPU, just like the Vertex
	// GPU buffer
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	info.size = indexArraySize;

	// build the buffer, and add a command to the initCmd command buffer
	// to copy data from the CPU buffer to the GPU buffer
	indexDataGPU = new BufferGPU(demo->device, demo->memory_properties, info);
	indexDataGPU->Store(demo->initCmd, indexDataCPU->buffer, indexArraySize);

	// This will be saved to the demo class
	numIndices = (int)indices.size();
}


Mesh::~Mesh()
{
	delete vertexDataGPU;
	delete indexDataGPU;
}
