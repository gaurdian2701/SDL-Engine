#include "Core/Editor.h"
#include "vec2.hpp"
#include "Application/Application.h"
#include "Components/Transform.h"
#include "Core/HelperFunctions.h"
#include "Scene/SceneManager.h"

#ifdef _DEBUG
void Core::Editor::Update(const float deltaTime)
{
    auto gameScene = Application::GetSceneManager().GetCurrentScene();

    if (gameScene == nullptr)
    {
        return;
    }

    const auto& gameObjectList = gameScene->GetGameObjectsInScene();

    //Drag and drop functionality
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        const glm::vec2 worldMousePos = ScreenToWorld(glm::vec2(io.MousePos.x, io.MousePos.y));

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            m_draggedEntity = INVALID_ENTITY_ID;
            m_isDragging = false;

            for (auto& gameObject : gameObjectList)
            {
                if (!m_isDragging && gameObject != nullptr)
                {
                    if (auto transform = gameObject->GetComponent<Components::Transform>())
                    {
                        //Are we clicking within the bounds of the transform area?
                        if (worldMousePos.x >= transform->LocalPosition.x - transform->LocalScale.x * 0.5f &&
                            worldMousePos.x <= transform->LocalPosition.x + transform->LocalScale.x * 0.5f &&
                            worldMousePos.y >= transform->LocalPosition.y - transform->LocalScale.y * 0.5f &&
                            worldMousePos.y <= transform->LocalPosition.y + transform->LocalScale.y * 0.5f)
                        {
                            m_draggedEntity = gameObject->m_entityID;
                            m_isDragging = true;
                            m_dragOffset = ImVec2(worldMousePos.x - transform->LocalPosition.x, worldMousePos.y - transform->LocalPosition.y);
                        }
                    }
                }
            }
        }

        //If we are dragging, change the position of the transform by the offset b/w the mouse position and the transform itself
        if (m_isDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            Components::Transform* draggedTransform =
                gameScene->GetGameObjectFromEntityID(m_draggedEntity)->GetComponent<Components::Transform>();

            if (draggedTransform)
            {
                draggedTransform->LocalPosition.x = worldMousePos.x - m_dragOffset.x;
                draggedTransform->LocalPosition.y = worldMousePos.y - m_dragOffset.y;
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            m_isDragging = false;
            m_draggedEntity = 0;
        }
    }

    //Displaying Objects in Scene
    ImGui::Begin("Debug");
    ImGui::Text("FRAME RATE:");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\n",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Number of Entities: %i\n", gameScene->GetGameObjectCount());
    ImGui::Separator();
    ImGui::Text("GAME OBJECT LIST: ");

    for (auto& gameObject : gameObjectList)
    {
        if (gameObject != nullptr)
        {
            if (auto transform = gameObject->GetComponent<Components::Transform>())
            {
                ImGui::PushID(gameObject->m_entityID);
                ImGui::Text("%s", gameScene->GetGameObjectFromEntityID(gameObject->m_entityID)->m_name.c_str());
                ImGui::DragFloat2("Position", &transform->LocalPosition.x, 0.5f, -100.0f, 100.0f);
                ImGui::DragFloat2("Scale", &transform->LocalScale.x, 0.5f, -100.0f, 100.0f);
                ImGui::DragFloat("Rotation", &transform->LocalRotation, 0.5f, -100.0f, 100.0f);
                ImGui::Separator();
                ImGui::PopID();
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Scene Options");
    if (ImGui::Button("Play/Pause", ImVec2(100, 30)))
    {
        gameScene->m_sceneIsPaused = !gameScene->m_sceneIsPaused;
    }
    ImGui::Text(gameScene->m_sceneIsPaused ? "Paused" : "Playing");
    ImGui::End();
}
#endif
