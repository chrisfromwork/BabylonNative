#pragma warning ( push )
// Momentarily disable warnings related to consuming OpenXR-MixedReality to avoid build breaks
#pragma warning ( disable : 4003 ) 
#include "Windows/XrPlatform.h"
#pragma warning ( pop )

#ifndef XR_SUPPORTED_EXTENSIONS
#define XR_SUPPORTED_EXTENSIONS
namespace xr
{
    struct XrSupportedExtensions : ExtensionDispatchTable
    {
        XrSupportedExtensions()
            : Names{}
        {
            uint32_t extensionCount{};
            XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr));
            m_extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
            XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, m_extensionProperties.data()));

            // D3D11 extension is required for this sample, so check if it's supported.
            for (const char* extensionName : REQUIRED_EXTENSIONS)
            {
                if (!TryEnableExtension(extensionName))
                {
                    throw std::runtime_error{ "Required extension not supported" };
                }
            }

            // Additional optional extensions for enhanced functionality. Track whether enabled in m_optionalExtensions.
            DepthExtensionSupported = TryEnableExtension(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
            UnboundedRefSpaceSupported = TryEnableExtension(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME);
            SpatialAnchorSupported = TryEnableExtension(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME);
            SecondaryViewConfigurationSupported = TryEnableExtension(XR_MSFT_SECONDARY_VIEW_CONFIGURATION_EXTENSION_NAME);
            FirstPersonObserverSupported = TryEnableExtension(XR_MSFT_FIRST_PERSON_OBSERVER_EXTENSION_NAME);
        }

        bool TryEnableExtension(const char* extensionName)
        {
            if (m_supportedExtensionNames.count(extensionName) > 0)
            {
                return true;
            }

            for (const auto& extensionProperty : m_extensionProperties)
            {
                if (strcmp(extensionProperty.extensionName, extensionName) == 0)
                {
                    Names.push_back(extensionName);
                    m_supportedExtensionNames.insert(extensionName);
                    return true;
                }
            }
            return false;
        };

        bool IsExtensionSupported(const std::string& extensionName) const
        {
            return m_supportedExtensionNames.count(extensionName) > 0;
        }

        std::vector<const char*> Names{};
        bool DepthExtensionSupported{ false };
        bool UnboundedRefSpaceSupported{ false };
        bool SpatialAnchorSupported{ false };
        bool SecondaryViewConfigurationSupported{ false };
        bool FirstPersonObserverSupported{ false };

    private:
        std::vector<XrExtensionProperties> m_extensionProperties{};
        std::set<std::string> m_supportedExtensionNames;
    };
}
#endif