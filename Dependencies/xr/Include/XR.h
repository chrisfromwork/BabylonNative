#pragma once

#include <memory>
#include <string>
#include <vector>
#include <arcana/threading/task.h>

namespace xr
{
    enum class TextureFormat
    {
        RGBA8_SRGB,
        BGRA8_SRGB,
        D24S8
    };

    enum class SessionType
    {
        IMMERSIVE_VR,
        IMMERSIVE_AR,
        INLINE,
        INVALID
    };

    struct Size
    {
        size_t Width{};
        size_t Height{};
    };

    struct Pose
    {
        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Position;

        struct
        {
            float X{};
            float Y{};
            float Z{};
            float W{};
        } Orientation;
    };

    enum class NativeReferenceSpaceType
    {
        VIEWER,
        LOCAL,
        LOCAL_FLOOR,
        BOUNDED_FLOOR,
        UNBOUNDED
    };

    struct XRReferenceSpaceType
    {
        static constexpr auto VIEWER{ "viewer" };
        static constexpr auto LOCAL{ "local" };
        static constexpr auto LOCAL_FLOOR{ "local-floor" };
        static constexpr auto BOUNDED_FLOOR{ "bounded-floor" };
        static constexpr auto UNBOUNDED{ "unbounded" };

        inline static const std::unordered_map<std::string, NativeReferenceSpaceType> ReferenceSpaceTypeMap {
            {XRReferenceSpaceType::VIEWER, NativeReferenceSpaceType::VIEWER},
            {XRReferenceSpaceType::LOCAL, NativeReferenceSpaceType::LOCAL},
            {XRReferenceSpaceType::LOCAL_FLOOR, NativeReferenceSpaceType::LOCAL_FLOOR},
            {XRReferenceSpaceType::BOUNDED_FLOOR, NativeReferenceSpaceType::BOUNDED_FLOOR},
            {XRReferenceSpaceType::UNBOUNDED, NativeReferenceSpaceType::UNBOUNDED},
        };

        static bool IsValid(const std::string& type)
        {
            return (type == VIEWER ||
                type == LOCAL ||
                type == LOCAL_FLOOR ||
                type == BOUNDED_FLOOR ||
                type == UNBOUNDED);
        }

        static NativeReferenceSpaceType GetNativeTypeFromString(const std::string& xrReferenceSpaceType)
        {
            assert(ReferenceSpaceTypeMap.count(xrReferenceSpaceType) > 0);
            return ReferenceSpaceTypeMap.at(xrReferenceSpaceType);
        }
    };

    using NativeTrackablePtr = void*;
    struct HitResult
    {
        Pose Pose{};
        NativeTrackablePtr NativeTrackable{};
    };

    struct Ray
    {
        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Origin;

        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Direction;
    };

    using NativeAnchorPtr = void*;
    struct Anchor
    {
        Pose Pose{};
        NativeAnchorPtr NativeAnchor{};
        bool IsValid{true};
    };

    class System
    {
    public:
        static constexpr float DEFAULT_DEPTH_NEAR_Z{ 0.5f };
        static constexpr float DEFAULT_DEPTH_FAR_Z{ 1000.f };

        class Session
        {
            friend class System;
            struct Impl;

        public:
            struct ReferenceSpace
            {
            public:
                struct Impl;
                ReferenceSpace();

                bool TryCreateReferenceSpaceAtOffset(Pose, std::shared_ptr<ReferenceSpace>&);
                NativeReferenceSpaceType GetType() const;
                Pose GetTransform() const;
            private:
                std::unique_ptr<Impl> m_impl{};
            };

            class Frame
            {
            public:
                struct Space
                {
                    Pose Pose;
                };

                struct View
                {
                    Space Space{};

                    struct
                    {
                        float AngleLeft{};
                        float AngleRight{};
                        float AngleUp{};
                        float AngleDown{};
                    } FieldOfView;

                    TextureFormat ColorTextureFormat{};
                    void* ColorTexturePointer{};
                    Size ColorTextureSize;

                    TextureFormat DepthTextureFormat{};
                    void* DepthTexturePointer{};
                    Size DepthTextureSize;

                    float DepthNearZ{};
                    float DepthFarZ{};
                };

                struct InputSource
                {
                    using Identifier = size_t;

                    enum class HandednessEnum
                    {
                        Left = 0,
                        Right = 1
                    };

                    const Identifier ID{ NEXT_ID++ };
                    bool TrackedThisFrame{};
                    Space GripSpace{};
                    Space AimSpace{};
                    HandednessEnum Handedness{};

                private:
                    static inline Identifier NEXT_ID{ 0 };
                };

                std::vector<View>& Views;
                std::vector<InputSource>& InputSources;

                Frame(System::Session::Impl&);
                ~Frame();

                void GetHitTestResults(std::vector<HitResult>&, Ray) const;
                Anchor CreateAnchor(Pose, NativeAnchorPtr) const;
                void UpdateAnchor(Anchor&) const;
                void DeleteAnchor(Anchor&) const;

                std::shared_ptr<ReferenceSpace> GetReferenceSpace() const;

            private:
                struct Impl;
                std::unique_ptr<Impl> m_impl{};
            };

            static arcana::task<std::shared_ptr<Session>, std::exception_ptr> CreateAsync(System& system, void* graphicsDevice, void* window);
            ~Session();

            // Do not use, call CreateAsync instead. Kept public to keep compatibility with make_shared.
            // Move to private when changing to unique_ptr.
            Session(System& system, void* graphicsDevice, void* window);

            std::unique_ptr<Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)> deletedTextureCallback = [](void*){});
            void RequestEndSession();
            Size GetWidthAndHeightForViewIndex(size_t viewIndex) const;
            void SetDepthsNearFar(float depthNear, float depthFar);

            bool TryGetReferenceSpace(NativeReferenceSpaceType, std::shared_ptr<ReferenceSpace>&);
            bool TryCreateReferenceSpace(NativeReferenceSpaceType, std::shared_ptr<ReferenceSpace>&);

        private:
            std::unique_ptr<Impl> m_impl{};
        };

        System(const char* = "OpenXR Experience");
        ~System();

        bool IsInitialized() const;
        bool TryInitialize();
        static arcana::task<bool, std::exception_ptr> IsSessionSupportedAsync(SessionType);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
