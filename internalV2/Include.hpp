#pragma once

#define _USE_MATH_DEFINES		 //to enable the use of mathmatecial constants   (like pi)

#include <time.h>
#include <Windows.h>

#include "Utils/Utils.hpp"	//	includes all utils...
#include "Interfaces.hpp"	//	includes all the interfaces..
#include "Netvars.hpp"		//	for grabbing netvars

// Semi-global variables for cheats
#include "Globals.hpp"

// Imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

// Hooks
#include "Hooks.hpp"            //hooking ingame functions