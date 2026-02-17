//
// Created by RadU on 2/15/2026.
//

#include "ModelEntity.hpp"

#include "Components/MeshComponent.hpp"
#include "Components/MetadataComponent.hpp"
#include "Components/ParentComponent.hpp"

namespace VoxelEngine
{

static void CreateComponents(const Model& model, const entt::entity& entity, const tinygltf::Node& node)
{
    glm::vec3 T = node.translation.size() == 3
                      ? glm::vec3(node.translation[0], node.translation[1], node.translation[2])
                      : glm::vec3(0.0f);

    glm::quat R = node.rotation.size() == 4
                      ? glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2])
                      : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 S = node.scale.size() == 3
                      ? glm::vec3(node.scale[0], node.scale[1], node.scale[2])
                      : glm::vec3(1.0f);

    TransformComponent transform;
    transform.LocalPosition = T;
    transform.LocalRotation = glm::normalize(R);
    transform.LocalScale = S;
    transform.IsDirty = true;

    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    registry.emplace<TransformComponent>(entity, transform);

    if (node.mesh >= 0)
    {
        std::string meshName = model.GetRawModel().meshes[node.mesh].name;
        registry.emplace<MeshComponent>(entity, meshName, model.GetMeshPrimitives(node.mesh));
    }

    if (!node.name.empty())
        registry.emplace<MetadataComponent>(entity, node.name);
}

static void CreateNodes(const Model& model, const entt::entity& parentEntity, const tinygltf::Node& node)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const entt::entity childEntity = registry.create();
    ParentComponent parent(parentEntity);
    parent.AddChild(childEntity);
    registry.emplace<ParentComponent>(childEntity, parent);

    const tinygltf::Model& gltfModel = model.GetRawModel();

    CreateComponents(model, childEntity, node);

    for (const int32_t childNode: node.children)
        CreateNodes(model, childEntity, gltfModel.nodes[childNode]);
}

entt::entity CreateEntityFromModel(const Model& model)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const entt::entity entity = registry.create();
    registry.emplace<TransformComponent>(entity, TransformComponent{});

    const tinygltf::Model& gltfModel = model.GetRawModel();
    const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene];

    if (!scene.name.empty())
        registry.emplace<MetadataComponent>(entity, scene.name);

    for (const int32_t node: scene.nodes)
        CreateNodes(model, entity, gltfModel.nodes[node]);

    return entity;
}

}
