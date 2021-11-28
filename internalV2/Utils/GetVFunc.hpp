#pragma once

/*
* virtual function implementation
* returns native function of specified class at given index
*/
/*
template <typename T = void*>
constexpr T GetVFunc(void* thisptr, size_t nIndex)
{
	return (*static_cast<T**>(thisptr))[nIndex];
}
*/
template<typename Function>Function GetVFunc(void* _VMT, int Index)
{
	void*** _TVMT = (void***)_VMT;
	void** VMT = *_TVMT;
	void* _Address = VMT[Index];
	return(Function)(_Address);
}

/*
 * virtual function call implementation
 * calls function of specified class at given index
 * @note: references should be wrapped with std::ref call!
 */
template <typename T, typename ... args_t>
constexpr T CallVFunc(void* thisptr, size_t nIndex, args_t... argList)
{
	using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
	return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
}