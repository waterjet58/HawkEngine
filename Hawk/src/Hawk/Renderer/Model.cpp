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

		uint32_t vertexSize = sizeof(vertices[0]);

		BufferObject stagingBuffer
		{
		_context,
		vertexSize,
		_vertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<BufferObject>(
			_context, vertexSize, _vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		_context.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);

	}

	void Model::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		_indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = _indexCount > 0;
		
		if (!hasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

		uint32_t indexSize = sizeof(indices[0]);

		BufferObject stagingBuffer{
			_context,
			indexSize,
			_indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<BufferObject>(
			_context,
			indexSize,
			_indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		_context.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);

	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
										//location, binding, format, offset
		attributeDescriptions.push_back({ 0 , 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1 , 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)	});
		attributeDescriptions.push_back({ 2 , 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)	});
		attributeDescriptions.push_back({ 3 , 0, VK_FORMAT_R32G32_SFLOAT,	 offsetof(Vertex, uv)		});

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

				{ //Vertex Positions
					const auto& posIndex = primitive.attributes.at("POSITION");
					const auto& normIndex = primitive.attributes.at("NORMAL");
					const auto& uvIndex = primitive.attributes.at("TEXCOORD_0");

					auto& accessor = model.accessors.at(posIndex);
					auto& bufferView = model.bufferViews.at(accessor.bufferView);
					auto& buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

					const float* vertPositions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					for (size_t i = 0; i < accessor.count; i++)
					{
						Vertex vertex{};

						vertex.position =
						{
							vertPositions[(i * 3) + 0],
							vertPositions[(i * 3) + 1],
							vertPositions[(i * 3) + 2]
						};

						accessor = model.accessors.at(normIndex);
						bufferView = model.bufferViews.at(accessor.bufferView);
						buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

						const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

						vertex.normal =
						{
							normals[(i * 3) + 0],
							normals[(i * 3) + 1],
							normals[(i * 3) + 2]
						};

						accessor = model.accessors.at(uvIndex);
						bufferView = model.bufferViews.at(accessor.bufferView);
						buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

						const float* uvCoords = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

						vertex.uv =
						{
							uvCoords[(i * 2) + 0],
							uvCoords[(i * 2) + 1]
						};

						vertex.color = { .8f, .8f, .8f };

						vertices.push_back(vertex);
					}

				}
				

				{ //Vertex Positions
					const auto& accessor = model.accessors[primitive.indices];
					const auto& bufferView = model.bufferViews.at(accessor.bufferView);
					const auto& buffer = model.buffers.at(bufferView.buffer); // vector<unsigned char>

					const uint16_t* positions = reinterpret_cast<const uint16_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					for (size_t i = 0; i < accessor.count; i++)
					{
						indices.push_back(positions[i]);
					}

				}
			}
		}

		

	}
}