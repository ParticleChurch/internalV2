#pragma once
template <typename T>
static constexpr auto RelativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

class KeyValues {
public:
    static KeyValues* FromString(const char* name, const char* value)
    {
        static std::uintptr_t keyValuesFromStringF = RelativeToAbsolute<decltype(keyValuesFromStringF)>(FindPattern("client", "E8 ? ? ? ? 83 C4 04 89 45 D8") + 1);
        const auto keyValuesFromString = keyValuesFromStringF;
        KeyValues* keyValues;
        __asm {
            push 0
            mov edx, value
            mov ecx, name
            call keyValuesFromString
            add esp, 4
            mov keyValues, eax
        }
        return keyValues;
    }
    KeyValues* FindKey(const char* keyName, bool create)
    {
        static KeyValues* (__thiscall * keyValuesFindKeyF)(KeyValues * keyValues, const char* keyName, bool create) = RelativeToAbsolute<decltype(keyValuesFindKeyF)>(FindPattern("client", "E8 ? ? ? ? F7 45") + 1);
        return keyValuesFindKeyF(this, keyName, create);
    }
    void SetString(const char* keyName, const char* value)
    {
        static void(__thiscall * keyValuesSetStringF)(KeyValues * keyValues, const char* value) = RelativeToAbsolute<decltype(keyValuesSetStringF)>(FindPattern("client", "E8 ? ? ? ? 89 77 38") + 1);
        if (const auto key = FindKey(keyName, true))
            keyValuesSetStringF(key, value);
    }
};