/**
 * Layout.cpp
 * UIのレイアウト管理
 */
#include "stdafx.h"
#include "Layout.h"
#include <fstream>
#include <sys/stat.h>


namespace
{
    /**
     * ==========================================
     * パース関連
     * ==========================================
     */
    Vector3 ParseVector3(const nlohmann::json& arr)
    {
        return Vector3(
            arr[0].get<float>(),
            arr[1].get<float>(),
            arr[2].get<float>()
        );
	}


    Vector4 ParseVector4(const nlohmann::json& arr)
    {
        return Vector4(
            arr[0].get<float>(),
            arr[1].get<float>(),
            arr[2].get<float>(),
            arr[3].get<float>()
        );
	}


    Vector4 ParseColor(const nlohmann::json& arr)
    {
        return Vector4(
            arr[0].get<float>() / 255.0f,
            arr[1].get<float>() / 255.0f,
            arr[2].get<float>() / 255.0f,
            arr[3].get<float>() / 255.0f
        );
    }


    Quaternion ParseRotation(const float rotation)
    {
        Quaternion q;
        q.SetRotationDegZ(rotation);
        return q;
    }


    /**
     * ==========================================
     * 初期化関連
     * ==========================================
     */
    template <typename T>
    void InitializeUIParts(T* parts, const nlohmann::json& item)
    {
		K2_ASSERT(false, "未実装\n");
    }

    void InitializeUIParts(app::ui::UIIcon* image, const nlohmann::json& item)
    {
		const std::string assetName = item["asset"].get<std::string>();
		const float w = item["width"].get<float>();
		const float h = item["height"].get<float>();
		const Vector3 position = ParseVector3(item["position"]);
		const Vector3 scale = ParseVector3(item["scale"]);
		const Quaternion rotation = ParseRotation(item["rotation"].get<float>());

        image->Initialize(assetName.c_str(), w, h, position, scale, rotation);
    }
}


namespace app
{
	namespace ui
	{
		
		void Layout::Update()
        {
			menu_->Update();

#ifdef APP_ENABLE_LAYOUT_HOTRELOAD
            // ホットリロードチェック
            struct stat st;
            if (stat(filePath_.c_str(), &st) == 0) {
                if (lastUpdateTime_ != st.st_mtime) {
                    lastUpdateTime_ = st.st_mtime;
                    Reload();
                }
            }
#endif // APP_ENABLE_LAYOUT_HOTRELOAD
        }


        void Layout::Render(RenderContext& rc)
        {
            menu_->Render(rc);
        }


        void Layout::Reload()
        {
            std::ifstream file(filePath_);
            if (!file.is_open()) return;

            nlohmann::json j;
            file >> j;

            // すでにMenuやCanvasがある場合は作り直しを行う
            if (menu_->GetCanvas() == nullptr) {
                menu_->SetCanvas(new UICanvas());
            }

            auto* canvas = menu_->GetCanvas();
            auto& elements = j["canvas"]["elements"];

            for (auto& item : elements) {
                std::string type = item["type"];
                std::string name = item["name"];

                // すでに存在するUIならパラメータ更新のみ
				const uint32_t key = Hash32(name.c_str());
                if (menu_->HasUI(key)) {
					menu_->UnregisterUI(key);
                    canvas->RemoveUI(key);
                }
                auto* ui = CreateUI(canvas, type, key, item);
                menu_->RegisterUI(key, ui);
            }

            menu_->InitializeLogic();
        }


        UIBase* Layout::CreateUI(UICanvas* canvas, const std::string& type, const uint32_t key, const nlohmann::json& item)
        {
            if (type == "UIIcon") {
                canvas->CreateUI<UIIcon>(key);
                auto* image = canvas->FindUI<UIIcon>(key);
			    InitializeUIParts(image, item);
                return image;
            }
            //if (type == "UIButton") return canvas->CreateUI<UIButton>(key);
            //if (type == "UIDigit")  return canvas->CreateUI<UIDigit>(key);
            //if (type == "UIGauge")  return canvas->CreateUI<UIGauge>(key);
            return nullptr;
        }
	}
}