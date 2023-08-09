#include "hwkPrecompiledHeader.h"
#include "Model.h"

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

}