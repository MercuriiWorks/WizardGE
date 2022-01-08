//
// Created by mecha on 08.01.2022.
//

#include "SceneViewport.h"
#include "imgui/imgui/imgui.h"
#include "core/FileExtensions.h"
#include "core/FileSystem.h"
#include "AssetBrowser.h"

namespace fairy {

    void fairy::SceneViewport::onRender(const engine::Time &dt) {
        // accept drag-drop item path
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_ITEM_TYPE)) {
                const auto* itemPath = (const wchar_t*) payload->Data;
                EDITOR_INFO("AssetBrowser: Accepting drag-drop data item path : {0}", STR_FROM_WCHAR(itemPath));
                auto draggedPath = engine::FileSystem::toPath(itemPath);
                auto itemExtension = draggedPath.extension().string();
                auto itemFileName = draggedPath.filename().string();
                SWITCH(itemExtension.c_str()) {
                    CASE(OBJ_EXT):
                        dragDropCallback->onObjDragged(itemFileName);
                        break;
                        CASE(JPG_EXT):
                        dragDropCallback->onImageDragged(itemFileName);
                        break;
                        CASE(PNG_EXT):
                        dragDropCallback->onImageDragged(itemFileName);
                        break;
                }
            }

            ImGui::EndDragDropTarget();
        }
    }

}