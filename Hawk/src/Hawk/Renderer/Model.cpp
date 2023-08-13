#include "hwkPrecompiledHeader.h"
#include "Model.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

namespace Hawk {

	Model::Model(VulkanContext& context, const Model::Builder& builder) : _context(context)
	{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	Model::~Model()
	{
		vkDestroyBuffer(_context.getDevice(), _vertexBuffer, _context.getAllocator());
		vkFreeMemory(_context.getDevice(), _vertexBufferMemory, _context.getAllocator());

		if (hasIndexBuffer)
		{
			vkDestroyBuffer(_context.getDevice(), _indexBuffer, _context.getAllocator());
			vkFreeMemory(_context.getDevice(), _indexBufferMemory, _context.getAllocator());
		}
	}

	std::unique_ptr<Model> Model::createModelFromFile(VulkanContext& context, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		HWK_CORE_INFO("vertices: {0}", builder.vertices.size());
		return std::make_unique<Model>(context, builder);
	}

	void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		_vertexCount = static_cast<uint32_t>(vertices.size());

		HWK_CORE_ASSERT(_vertexCount >= 3 && "Vertex count must be at least 3");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		_context.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(_context.getDevice(), stagingBufferMemory);

		_context.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_vertexBuffer,
			_vertexBufferMemory);

		_context.copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

		vkDestroyBuffer(_context.getDevice(), stagingBuffer, _context.getAllocator());
		vkFreeMemory(_context.getDevice(), stagingBufferMemory, _context.getAllocator());
	}

	void Model::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		_indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = _indexCount > 0;
		
		if (!hasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		_context.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(_context.getDevice(), stagingBufferMemory);

		_context.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_indexBuffer,
			_indexBufferMemory);

		_context.copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(_context.getDevice(), stagingBuffer, _context.getAllocator());
		vkFreeMemory(_context.getDevice(), stagingBufferMemory, _context.getAllocator());

	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	void Model::Builder::loadModel(const std::string& filePath)
	{
		HWK_CORE_TRACE("Loading File: {0}", filePath);

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string error, warning;

		bool success = loader.LoadASCIIFromFile(&model, &error, &warning, filePath);

		if (!warning.empty()) {
			std::cerr << warning << std::endl;
		}

		if (!error.empty()) {
			std::cerr << "Error:::::" << error << std::endl;
		}

		if (!success) {
			std::cerr << "Failed to parse glTF file" << std::endl;
			return;
		}

		vertices.clear();
		indices.clear();

		int x = 0;
		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				x++;
				auto count = model.accessors.at(primitive.attributes.at("POSITION")).count;
				HWK_CORE_INFO("Mesh {0}: {1} vertices", x, count);
				for (size_t i = 0; i < count; ++i)
				{
					Vertex vertex{};

					{ //Vertex Positions
						const auto& index = primitive.attributes.at("POSITION");
						const auto& accessor = model.accessors.at(index);
						const auto& bufferView = model.bufferViews.at(accessor.bufferView);
						const auto& buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

						const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

						vertex.position =
						{
							positions[i * 3 + 0],
							positions[i * 3 + 1],
							positions[i * 3 + 2]
						};

					}


					{ //Vertex Normals
						const auto& index = primitive.attributes.at("NORMAL");
						const auto& accessor = model.accessors.at(index);
						const auto& bufferView = model.bufferViews.at(accessor.bufferView);
						const auto& buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

						const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

						vertex.normal =
						{
							normals[i * 3 + 0],
							normals[i * 3 + 1],
							normals[i * 3 + 2]
						};

					}

					vertices.push_back(vertex);
				}
				
			}
		}

		

	}
}