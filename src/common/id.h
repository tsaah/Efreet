#pragma once

#include "common.h"

namespace efreet::id {

using IdType = u32;

constexpr u32 generationBits{ 8 };
constexpr u32 indexBits{ sizeof(IdType) * 8 - generationBits };
constexpr IdType indexMask{ (IdType{ 1 } << indexBits) - 1 };
constexpr IdType generationMask{ (IdType{ 1 } << generationBits) - 1 };
constexpr IdType idMask{ IdType{ static_cast<IdType>(-1) } };
constexpr IdType invalid{ idMask };

using GenerationType = std::conditional_t<generationBits <= 16, std::conditional_t<generationBits <= 8, u8, u16>, u32>;

STATIC_ASSERT(sizeof(GenerationType) * 8 >= generationBits);
STATIC_ASSERT(sizeof(IdType) - sizeof(GenerationType) > 0);

inline constexpr b32 isValid(IdType id) {
    return id != idMask;
}

inline constexpr IdType index(IdType id) {
    return id & indexMask;
}

inline constexpr GenerationType generation(IdType id) {
    return (id >> indexBits) & generationMask;
}

inline constexpr IdType newGeneration(IdType id) {
    const auto generation{ id::generation(id) + 1 };
    E_ASSERT(generation < 255);
    return index(id) | (generation << generationBits);
}

#ifdef _DEBUG

namespace internal {

struct IdBase {
    inline constexpr explicit IdBase(IdType id): id_{ id } {}
    inline constexpr operator IdType() const { return id_; }
private:
    IdType id_{ invalid };
};

} // namespace internal

#define DEFINE_TYPED_ID(name) \
    struct name final: public id::internal::IdBase { \
        inline constexpr explicit name(id::IdType id): IdBase{ id } {} \
        inline constexpr name(): IdBase{ id::idMask } {} \
    }

#else

#define DEFINE_TYPED_ID(name) using name = id::IdType

#endif

} // namespace efreet::id