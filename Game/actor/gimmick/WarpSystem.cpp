#include "stdafx.h"
#include "WarpSystem.h"
#include "actor/Gimmick.h"


namespace app
{
	namespace gimmick
	{
		WarpSystem* WarpSystem::instance_ = nullptr;


        WarpSystem::WarpSystem()
        {
        }


        WarpSystem::~WarpSystem()
        {

        }


        void WarpSystem::RegisterPipe(EndpointId id, app::actor::PipeGimmick* pipe)
        {
            if (registMap_.find(id) != registMap_.end()) {
                // IDd•¡
                K2_ASSERT(false, "‚·‚Å‚É“o˜^‚³‚ê‚Ä‚¢‚Ü‚·\n");
                return;
            }
			registMap_.emplace(id, pipe);
        }


        void WarpSystem::UnregisterPipe(EndpointId id)
        {
            registMap_.erase(id);
        }


        app::actor::PipeGimmick* WarpSystem::FindPipe(EndpointId id) const
        {
            auto it = registMap_.find(id);
            if (it != registMap_.end()) {
                return it->second;
            }
            return nullptr;
        }


        void WarpSystem::AddLink(EndpointId from, EndpointId to)
        {
            if (linkMap_.find(from) != linkMap_.end()) {
				K2_ASSERT(false, "‚·‚Å‚ÉƒŠƒ“ƒN‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚·\n");
                return;
            }
			linkMap_.emplace(from, to);
        }


        bool WarpSystem::TryResolve(EndpointId fromId, EndpointId& outToId) const
        {
            auto it = linkMap_.find(fromId);
            if (it != linkMap_.end()) {
                outToId = it->second;
                return true;
            }
            return false;
        }
	}
}
