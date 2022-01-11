class GameEvent {
public:
    const char* GetName() {
        typedef const char* (__thiscall* oGetName)(void*);
        return GetVFunc<oGetName>(this, 1)(this);
    }

    int GetInt(const char* keyName, int defaultValue = 0) {
        typedef int(__thiscall* ogetInt)(void*, const char*, int);
        return GetVFunc<ogetInt>(this, 6)(this, keyName, defaultValue);
    }

    float GetFloat(const char* keyName, float defaultValue = 0.0f) {
        typedef float(__thiscall* ogetFloat)(void*, const char*, float);
        return GetVFunc<ogetFloat>(this, 8)(this, keyName, defaultValue);
    }

    const char* GetString(const char* keyName, const char* defaultValue = "") {
        typedef const char* (__thiscall* ogetString)(void*, const char*, const char*);
        return GetVFunc<ogetString>(this, 9)(this, keyName, defaultValue);
    }

    void SetString(const char* keyName, const char* value) {
        typedef void(__thiscall* osetString)(void*, const char*, const char*);
        return GetVFunc<osetString>(this, 16)(this, keyName, value);
    }
};

class GameEventListener {
public:
    virtual ~GameEventListener() {}
    virtual void FireGameEvent(GameEvent* event) = 0;
    virtual int GetEventDebugId() { return 42; }
};

class GameEventManager {
public:
    bool AddListener(GameEventListener* listener, const char* name) {
        typedef bool(__thiscall* oaddListener)(void*, GameEventListener*, const char*, bool);
        return GetVFunc<oaddListener>(this, 3)(this, listener, name, false);
    }

    void RemoveListener(GameEventListener* listener) {
        typedef void(__thiscall* oremoveListener)(void*, GameEventListener*);
        return GetVFunc<oremoveListener>(this, 5)(this, listener);
    }
};