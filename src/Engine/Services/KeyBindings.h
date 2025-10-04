#pragma once

#include <IService.h>
#include <Services/Keyboard.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

enum class KeyMoves
{
    None = 0,
    Forward = 1 << 0,
    Backward = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,
    Up = 1 << 4,
    Down = 1 << 5,
    Jump = 1 << 6,
    Crouch = 1 << 7,
    Sprint = 1 << 8,
    Walk = 1 << 9,
    Interact = 1 << 10,
    ToggleInventory = 1 << 11,
    ToggleCamera = 1 << 12,
};

class KeyBindings : public IService
{
public:
    KeyBindings();
    ~KeyBindings() override;

    HRESULT Init() override;
    void Shutdown() override;
    void Render() override {};
    void Update(float deltaTime) override {};

    const std::string& GetServiceName() const override {
        static const std::string name = "KeyBindings";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "KeyBindings";
        return name;
    }

    bool IsKeyPressed(KeyMoves keyMove);
    bool IsKeyDown(KeyMoves keyMove);
    bool IsKeyReleased(KeyMoves keyMove);

    void Bind(std::map<std::string, std::vector<std::string>> key_bindings);

    // Método para obtener las teclas asociadas a un movimiento
    //const std::vector<std::string>& GetKeysForMove(KeyMoves move) const;

    unsigned char TranslateKeyStringToCode(const std::string& key) const;

private:
    std::shared_ptr<Keyboard> m_keyboard;
    std::unordered_map<KeyMoves, std::vector<int>> m_keyBindings;
    std::unordered_map<KeyMoves, bool> m_keyBindingsState;
    KeyMoves m_keyMoves = KeyMoves::None;
};
