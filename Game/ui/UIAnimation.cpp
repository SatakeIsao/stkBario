/**
 * UIAnimation.cpp
 * UI用のアニメーション処理群
 */
#include "stdafx.h"
#include "UIAnimation.h"
#include "UIParts.h"


namespace app
{
	namespace ui
	{


		UIColorAnimation::UIColorAnimation()
		{
			SetFunc([&](Vector4 v)
				{
					ui_->color = v;
				});
		}




		/*******************************************************/


		UIScaleAnimation::UIScaleAnimation()
		{
			SetFunc([&](Vector3 s)
				{
					ui_->transform.localScale = s;
				});
		}




		/*******************************************************/


		UITranslateAniamtion::UITranslateAniamtion()
		{
			SetFunc([&](Vector3 s)
				{
					ui_->transform.localPosition = s;
				});
		}




		/*******************************************************/


		UITranslateOffsetAnimation::UITranslateOffsetAnimation()
		{
			SetFunc([&](Vector3 offset)
				{
					ui_->transform.localPosition.Add(offset);
				});
		}




		/*******************************************************/


		UIRotationAnimation::UIRotationAnimation()
		{
			SetFunc([&](float s)
				{
					ui_->transform.localRotation.SetRotationDegZ(s);
				});
		}
	}
}