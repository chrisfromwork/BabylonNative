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
            float X{0};
            float Y{0};
            float Z{0};
        } Position;

        struct
        {
            float X{0};
            float Y{0};
            float Z{0};
            float W{1};
        } Orientation;
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
            class ReferenceSpace
            {
            public:
                typedef std::string Type;
                typedef void* NativeReferenceSpacePtr;

                static constexpr auto VIEWER{ "viewer" };
                static constexpr auto LOCAL{ "local" };
                static constexpr auto LOCAL_FLOOR{ "local-floor" };
                static constexpr auto BOUNDED_FLOOR{ "bounded-floor" };
                static constexpr auto UNBOUNDED{ "unbounded" };

                static bool IsValidType(const Type& type)
                {
                    return (type == VIEWER ||
                        type == LOCAL ||
                        type == LOCAL_FLOOR ||
                        type == BOUNDED_FLOOR ||
                        type == UNBOUNDED);
                }

                ReferenceSpace(System::Session::Impl&, const Type&, const Pose&);
                bool TryCreateReferenceSpaceAtOffset(const Pose&, std::shared_ptr<ReferenceSpace>&);
                Type GetType() const;
                NativeReferenceSpacePtr GetNativeComponent() const;

            private:
                struct Impl;
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

            bool TryGetReferenceSpace(const ReferenceSpace::Type&, std::shared_ptr<ReferenceSpace>&);
            bool TryCreateReferenceSpace(const ReferenceSpace::Type&, std::shared_ptr<ReferenceSpace>&);

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
