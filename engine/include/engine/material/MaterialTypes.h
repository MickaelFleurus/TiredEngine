#pragma once

namespace Material {
enum class EMaterialType {
    Unlit,    // Simple texture, no lighting
    Lit,      // Standard PBR lighting
    Text,     // Font rendering
    Sprite,   // 2D sprite rendering
    Particle, // Particle systems
    UI        // UI elements
};
} // namespace Material