#include "Core/ECS/Systems/DebugEditorSystem.h"
#include "imgui.h"
#include "Components/AABB2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Scene/SceneManager.h"

void Core::ECS::Systems::DebugEditorSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform>(this);
}

void Core::ECS::Systems::DebugEditorSystem::UpdateSystem(const float deltaTime)
{
    auto gameScene = Application::GetSceneManager().GetCurrentScene();
    auto& gameObjectsInScene = gameScene->GetGameObjectsInScene();

    ImGui::Begin("Debug");
    ImGui::Text("FRAME RATE:");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\n",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Number of Entities: %i\n", gameObjectsInScene.size());
    ImGui::Separator();
    ImGui::Text("GAMEOBJECT LIST: ");

    int IDiterator = 0;

    for (auto& gameObject : gameObjectsInScene)
    {
        Components::Transform* transform = gameObject->GetComponent<Components::Transform>();

        if (transform != nullptr)
        {
            ImGui::PushID(IDiterator++);
            ImGui::Text(gameObject->m_name.c_str());
            ImGui::DragFloat2("Position", &transform->LocalPosition.x, 0.5f, -100.0f, 100.0f);
            ImGui::DragFloat2("Scale", &transform->LocalScale.x, 0.5f, -100.0f, 100.0f);
            ImGui::DragFloat("Rotation", &transform->LocalRotation, 0.5f, -100.0f, 100.0f);
            ImGui::Separator();
            ImGui::PopID();
        }
    }
    ImGui::End();

    ImGui::Begin("Scene Options");
    if (ImGui::Button("Play/Pause", ImVec2(100, 30)))
    {
        gameScene->m_sceneIsPaused = !gameScene->m_sceneIsPaused;
    }
    ImGui::End();
}
