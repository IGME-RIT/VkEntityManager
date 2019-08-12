#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Demo.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// forward declaration
class Demo;
class Mesh;
class Texture;

class Entity
{
public:
	// Entity data
	BufferCPU* matrixBufferCPU;
	VkDescriptorSet descriptor_set;

	Mesh* mesh;
	Texture* texture;

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
	glm::mat4 modelMatrix;
	glm::mat4 parentModelMatrix = glm::mat4(1.0);
	float radius;

	glm::mat4 GetModelMatrix();
	glm::vec3 GetWorldPosition();

	Entity(Mesh* m, Texture* t, Demo* demo);
	~Entity();

	void CreateDescriptorSet(Demo* demo);
	void Draw(VkCommandBuffer cmd, VkPipelineLayout pipeline_layout);
	void Update(glm::mat4x4 vp);
};

