#pragma once

#define _USE_MATH_DEFINES		 //to enable the use of mathmatecial constants   (like pi)

#include <time.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <cstddef>
#include <cmath> // used: isfinite, fmodf, sqrtf
#include <limits> // used: numeric_limits
#include <xutility> 
#include <algorithm> // used: clamp
#include <corecrt_math.h> // QAngle used
#include <d3d9.h>
#include <Psapi.h>
#include <cstdint>
#include <WinUser.h>
#include <winnt.h>
#include <intrin.h>				 //for _ReturnAddress() (intitially for hooks.cpp)
#include <functional>
#include <map>
#include <deque>
#include <chrono>

//dumbass warnings
#pragma warning(disable:4244)
#pragma warning(disable:4804)

#include "Utils/Utils.hpp"	//	includes all utils...
#include "Valve/Valve.hpp"	//	include all the Valve classes/structs
#include "Interfaces.hpp"	//	includes all the interfaces..
#include "Netvars.hpp"		//	for grabbing netvars

// Semi-global variables for cheats
#include "Entity.hpp"
#include "Globals.hpp"

// Imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

// Before the hacks, after the imgui stuff
#include "GUI/Config.hpp"
#include "GUI/Menu.hpp"

// lol
#include "Utils/Math.hpp"

// Hacks
#include "Hacks/Autowall.hpp"
#include "Hacks/Movement.hpp"
#include "Hacks/Lagcomp.hpp"
#include "Hacks/Backtrack.hpp"
#include "Hacks/ESP.hpp"
#include "Hacks/Aimbot.hpp"
#include "Hacks/Thirdperson.hpp"

// Hooks
#include "Hooks.hpp"            //hooking ingame functions