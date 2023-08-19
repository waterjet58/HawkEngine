#pragma once

//for use by Hawk applications
#include "Hawk/Core/Application.h"
#include "Hawk/Core/Log.h"
#include "Hawk/Core/Layer.h"
#include "Hawk/ImGui/ImGUILayer.h"
#include "Hawk/Core/Input.h"
#include "Hawk/Core/MouseCodes.h"
#include "Hawk/Core/KeyCodes.h"
#include "Hawk/Core/Timestep.h"
#include "Hawk/Core/Camera.h"
#include "Hawk/ECS/ECSManager.hpp"
#include "Hawk/Renderer/Model.h"
#include "Hawk/ECS/Components/Mesh.h"
#include <Hawk/ECS/Components/Billboard.h>
#include <Hawk/ECS/Components/PointLight.h>
#include "Hawk/Renderer/FrameData.h"
#include "Platform/Vulkan/VulkanDescriptors.h"

//ALL SYSTEMS
#include "Hawk/ECS/Systems/SpriteRendererSystem.h"
#include "Hawk/ECS/Systems/MeshRendererSystem.h"
#include <Hawk/ECS/Systems/BillboardSystem.h>
#include "Hawk/ECS/Systems/EditorGridSystem.h"
#include "Hawk/ECS/Systems/PointLightSystem.h"
//ALL SYSTEMS

// --Entry Point--
#include "Hawk/Core/EntryPoint.h"
// ---------------
