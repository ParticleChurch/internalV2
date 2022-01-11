#include "../Include.hpp"

DlightManager* dlightManager = new DlightManager();

#define DEBUG_DLIGHT true

void DlightManager::Run()
{
	return;
	if constexpr (DEBUG_DLIGHT) L::Debug("START");

	// make user press e :D
	if (!G::cmd) return;
	if (!G::IsInGame) return;
	if (!G::Localplayer) return;

	// slap on a ton of lights somewhere (with zero value on die)
	while (numLights <= MAX_DLIGHTS)
	{
		assignKey++;
		numLights++;
		if constexpr (DEBUG_DLIGHT) L::Debug("CL_AllocDlight check");
		dlight_t* temp = I::effects->CL_AllocDlight(G::Localplayer->GetIndex() + assignKey);
		if constexpr (DEBUG_DLIGHT) L::Debug("CL_AllocDlight success");
		temp->color.r = 0;
		temp->color.g = 0;
		temp->color.b = 255;
		temp->color.exponent = 10.f; // 0, 133, 255, 161 <= ideal col
		temp->radius = 50.f;
		temp->decay = 0.f;// 50.f;
		temp->m_Direction = Vector(0,0,0);
		temp->origin = Vector(0, 0, 0);
		temp->die = FLT_MAX;
		displayedLights.push_back(temp);
	}

	// dumb shit
	static std::vector < std::pair<dlight_t, float>> visible_dlights = {};
	static float LastTimeUpdate = I::globalvars->flCurrentTime;
	if (fabsf(I::globalvars->flCurrentTime - LastTimeUpdate) > 0.1)
	{
		visible_dlights.clear();
		visible_dlights.resize(0);
		for (auto a : storedLights)
		{
			Vector start = G::Localplayer->GetEyePosition();
			Trace_t trace;
			Ray_t ray(G::Localplayer->GetEyePosition(), a.origin);
			CTraceFilter filter((IHandleEntity*)G::Localplayer);
			I::enginetrace->TraceRay(ray, MASK_VISIBLE, &filter, &trace);

			Vector out;
			if (trace.flFraction > 0.97 && WorldToScreen(a.origin, out))
			{
				// add to visible
				float dist = Calc::CrosshairDist(Calc::CalculateAngle(a.origin));
				visible_dlights.push_back(std::pair(a, dist));
				I::surface->DrawSetColor(Color(255, 0, 0, 255));
				I::surface->DrawOutlinedCircle(out.x, out.y, 2, 5);



			}
		}

		LastTimeUpdate = I::globalvars->flCurrentTime;
		//Trade secret sort struct for epic blazin fast sorting
		struct {
			bool operator()(std::pair<dlight_t, float> a, std::pair<dlight_t, float> b) const
			{
				// otherwise sort by proper values
				return a.second > b.second;
			}
		} targetSort;

		// we do a bit of sortin
		std::sort(visible_dlights.begin(), visible_dlights.end(), targetSort);

		// do p rendering fix
		for (auto a : displayedLights)
			a->die = 0;
	}

	// now we have the needed lights, let's decide what to move/change
	int range = visible_dlights.size();
	for (int i = 0; i < visible_dlights.size(); i++)
	{
		// we've reached the end
		if (i >= MAX_DLIGHTS) break;

		// copy the value over
		CopyValuesOver(displayedLights[i], &visible_dlights[i].first);

		// custom coloring (more green closer to crosshair)
		displayedLights[i]->color.r = 255 - (255 * (float(i) / range));
		displayedLights[i]->color.g = (255 * (float(i) / range));

		Vector out;
		if (WorldToScreen(displayedLights[i]->origin, out))
		{
			std::string TEXT = std::to_string(i);
			static std::wstring wide_string;
			wide_string = std::wstring(TEXT.begin(), TEXT.end());
			static DWORD FONT = I::surface->FontCreate();
			static bool Once = true;
			if (Once)
			{
				Once = false;
				I::surface->SetFontGlyphSet(FONT, "Verdana", 16, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
			}
			I::surface->DrawSetTextFont(FONT);
			I::surface->DrawSetTextColor(Color(255, 255, 255, 255));
			I::surface->DrawSetTextPos(out.x, out.y);
			I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));
		}
	}
	

	// now do adding shit
	if (!(G::cmd->iButtons & IN_USE)) return;

	if constexpr (DEBUG_DLIGHT) L::Debug("GetVecOrigin");
	Vector viewangle = Vector(G::StartAngle.x, G::StartAngle.y);
	Vector traceStart, traceEnd;
	AngleVectors(viewangle, &traceEnd);

	traceStart = G::Localplayer->GetEyePosition();
	traceEnd = traceStart + (traceEnd * 8192.f);	Trace_t trace;
	Ray_t ray(G::Localplayer->GetEyePosition(), traceEnd);
	CTraceFilter filter((IHandleEntity*)G::Localplayer);
	I::enginetrace->TraceRay(ray, MASK_VISIBLE, &filter, &trace);

	// if it didn't hit, just return
	if (trace.flFraction > 0.98)
		return;
	if constexpr (DEBUG_DLIGHT) L::Debug("GetVecOrigin");

	// do location check
	if constexpr (DEBUG_DLIGHT) L::Debug("loc check");
	bool badDistance = false;
	for (auto a : storedLights)
	{
		// these are too close brugh
		if ((a.origin - trace.vecEnd).VecLength() < 30)
			badDistance = true;
	}
	if (badDistance) return;

	// we'll add it to the storedLights
	dlight_t StoredLight;
	StoredLight.color.r = 0;
	StoredLight.color.g = 0;
	StoredLight.color.b = 0;
	StoredLight.color.exponent = 10.f; // 0, 133, 255, 161 <= ideal col
	StoredLight.radius = 50.f;
	StoredLight.decay = 0.f;// 50.f;
	StoredLight.m_Direction = trace.vecStart + ((trace.vecEnd - trace.vecStart) * 0.96);
	StoredLight.origin = trace.vecStart + ((trace.vecEnd - trace.vecStart) * 0.96);
	StoredLight.die = FLT_MAX;// I::globalvars->flCurrentTime + 10.f;// 0.f < 0.1f ? I::globalvars->flCurrentTime + 0.2f : 0.f;
	storedLights.push_back(StoredLight);
}

void DlightManager::CopyValuesOver(dlight_t * dst, dlight_t * src)
{
	dst->color.r = src->color.r;
	dst->color.g = src->color.g;
	dst->color.b = src->color.b;
	
	dst->color.exponent = src->color.exponent;
	dst->radius = src->radius;
	dst->decay = src->decay;
	dst->m_Direction = src->m_Direction;
	dst->origin = src->origin;
	dst->die = src->die;
}

void DlightManager::Save()
{
}
