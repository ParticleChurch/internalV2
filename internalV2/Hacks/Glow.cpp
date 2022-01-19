#include "../Include.hpp"	

Glow* glow = new Glow();

void Glow::run()
{
	if (!G::Localplayer) return;

    for (int i = 0; i < I::glowobjectmanager->glowObjectDefinitions.Size(); i++) {
        GlowObjectDefinition& glowobject = I::glowobjectmanager->glowObjectDefinitions[i];

        auto entity = glowobject.entity;

        if (glowobject.isUnused() || !entity || entity->IsDormant())
            continue;

        auto applyGlow = [&glowobject](const Color& glow, int style) noexcept
        {
            glowobject.renderWhenOccluded = true;
            glowobject.glowAlpha = glow.color[3] / 255.f;
            glowobject.glowStyle = style;
            glowobject.glowAlphaMax = 0.6f;
            glowobject.glowColor = Vector(glow.color[0] / 255.f, glow.color[1] / 255.f, glow.color[2] / 255.f);
        };

        if(entity->GetClientClass()->nClassID == EClassIndex::CCSPlayer) {
            if (!entity->IsAlive())
                break;
            
            if (entity->GetTeam() != G::LocalplayerTeam && cfg->glow.EnemyGlow)
                applyGlow(cfg->glow.EnemyColor,cfg->glow.EnemyStyle);

            if (entity->GetTeam() == G::LocalplayerTeam && entity->GetIndex() != G::LocalplayerIndex && cfg->glow.FriendGlow)
                applyGlow(cfg->glow.FriendColor, cfg->glow.FriendStyle);

            if (entity->GetIndex() == G::LocalplayerIndex && cfg->glow.LocalGlow)
                applyGlow(cfg->glow.LocalColor, cfg->glow.LocalStyle);
            
        } 
    }
}
