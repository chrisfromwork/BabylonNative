#pragma once

#include "XrSupportedExtensions.h"
#include "XrSceneUnderstanding.h"

namespace xr
{
    struct SceneUnderstanding
    {
    public:
        struct InitOptions
        {
            const XrSession& Session;
            const XrSupportedExtensions& Extensions;
            const xr::DetectionBoundaryType DetectionBoundaryType;
            const float SphereRadius;
            const xr::Frustum Frustum;
            const xr::Vector3f BoxDimensions;
            const double UpdateIntervalInSeconds;

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions);

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions,
                const xr::DetectionBoundaryType detectionBoundaryType,
                const float sphereRadius,
                const xr::Frustum frustum,
                const xr::Vector3f boxDimensions,
                const double updateIntervalInSeconds);
        };

        struct UpdateFrameArgs
        {
            const XrSpace& SceneSpace;
            const XrSupportedExtensions& Extensions;
            const XrTime DisplayTime;
            std::vector<System::Session::Frame::SceneObject>& SceneObjects;
            std::vector<System::Session::Frame::Plane>& Planes;
            std::vector<System::Session::Frame::Plane::Identifier>& UpdatedPlanes;
            std::vector<System::Session::Frame::Plane::Identifier>& RemovedPlanes;
            std::vector<System::Session::Frame::Mesh>& Meshes;
            std::vector<System::Session::Frame::Mesh::Identifier>& UpdatedMeshes;
            std::vector<System::Session::Frame::Mesh::Identifier>& RemovedMeshes;
        };

        struct Mesh : SceneMesh
        {
            XrSceneMeshKeyMSFT key;
            XrSceneObjectKeyMSFT parentObjectKey;
        };

        struct Plane : ScenePlane
        {
            XrScenePlaneKeyMSFT key;
            XrSceneObjectKeyMSFT parentObjectKey;
        };

        struct SceneObject : public xr::System::Session::Frame::SceneObject
        {
            std::map<XrSceneMeshKeyMSFT, Mesh> Meshes;
            std::map<XrScenePlaneKeyMSFT, Plane> Planes;
            XrPosef Pose;
        };

        SceneUnderstanding();
        ~SceneUnderstanding();
        void Initialize(const InitOptions options) const;
        void UpdateFrame(UpdateFrameArgs args) const;
        System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const;
        System::Session::Frame::Mesh& TryGetMeshByID(const System::Session::Frame::Mesh::Identifier id) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}