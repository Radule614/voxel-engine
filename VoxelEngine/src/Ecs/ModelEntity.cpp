//
// Created by RadU on 2/15/2026.
//

#include "ModelEntity.hpp"

#include "Components/AnimationComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/MetadataComponent.hpp"
#include "Components/ParentComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/TransformComponent.hpp"

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

static void CreateNodes(const Model& model,
                        const entt::entity& parentEntity,
                        const int32_t nodeIndex,
                        std::unordered_map<int32_t, entt::entity>& nodeEntityMap)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const entt::entity childEntity = registry.create();
    ParentComponent parent(parentEntity);
    parent.AddChild(childEntity);
    registry.emplace<ParentComponent>(childEntity, parent);

    const tinygltf::Model& gltfModel = model.GetRawModel();
    const tinygltf::Node& node = gltfModel.nodes[nodeIndex];

    CreateComponents(model, childEntity, node);

    nodeEntityMap[nodeIndex] = childEntity;

    for (const int32_t childNode: node.children)
        CreateNodes(model, childEntity, childNode, nodeEntityMap);
}

static void CreateAnimationComponent(const Model& model,
                                     const entt::entity& entity,
                                     std::unordered_map<int32_t, entt::entity>& nodeEntityMap)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    AnimationComponent animationComponent;

    for (const auto& [AnimationName, NodeAnimations]: model.GetAnimations())
    {
        EntityAnimation entityAnimation;
        entityAnimation.Name = AnimationName;

        for (const auto& [nodeIndex, nodeAnimation]: NodeAnimations)
        {
            entityAnimation.NodeAnimations[nodeEntityMap[nodeIndex]] = nodeAnimation;

            for (auto track: nodeAnimation.Tracks)
            {
                const float_t lastTime = track.Times.back();

                if (lastTime > entityAnimation.Duration)
                    entityAnimation.Duration = lastTime;
            }
        }

        animationComponent.Animations.emplace_back(entityAnimation);
    }

    registry.emplace<AnimationComponent>(entity, animationComponent);
}

void CreateSkinComponents(const Model& model, std::unordered_map<int32_t, entt::entity>& nodeEntityMap)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto& [RootIndexes, JointIndexes, InverseBindMatrices]: model.GetSkins())
    {
        SkinComponent skinComponent;
        skinComponent.InverseBindMatrices = InverseBindMatrices;
        skinComponent.JointMatrices.resize(InverseBindMatrices.size());

        for (auto jointIndex: JointIndexes)
            skinComponent.JointEntities.push_back(nodeEntityMap[jointIndex]);

        for (int32_t nodeIndex: RootIndexes)
        {
            const entt::entity rootEntity = nodeEntityMap[nodeIndex];
            registry.emplace<SkinComponent>(rootEntity, skinComponent);
        }
    }
}

entt::entity CreateEntityFromModel(const Model& model)
{
    entt::registry& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const entt::entity entity = registry.create();
    registry.emplace<TransformComponent>(entity, TransformComponent{});

    const tinygltf::Model& gltfModel = model.GetRawModel();
    const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene];

    std::unordered_map<int32_t, entt::entity> nodeEntityMap{};

    if (!scene.name.empty())
        registry.emplace<MetadataComponent>(entity, scene.name);

    for (const int32_t node: scene.nodes)
        CreateNodes(model, entity, node, nodeEntityMap);

    if (!model.GetAnimations().empty())
        CreateAnimationComponent(model, entity, nodeEntityMap);

    if (!model.GetSkins().empty())
        CreateSkinComponents(model, nodeEntityMap);

    return entity;
}

}
