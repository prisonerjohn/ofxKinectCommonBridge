#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <objbase.h>
#include <NuiApi.h>				// check for Include Dir: $(KINECTSDK10_DIR)inc;$(KINECT_TOOLKIT_DIR)inc;

#ifdef _WIN32
    #ifdef DLL_EXPORTS
        #define KINECT_API __declspec(dllexport)
    #else
        #define KINECT_API __declspec(dllimport)
		#pragma comment (lib, "KinectWrapper.lib") // be sure to add the .dll/.lib folder to your Linker path
    #endif // DLL_EXPORTS
#endif //_WIN32

typedef HANDLE HKINECT;
#define KINECT_MAX_PORTID_LENGTH	250

// statuses that the KinectSensor wrapper uses to determine state
typedef enum _KinectSensorStatus
{
    KinectSensorStatusNone = 0x00000000,

    /// <summary>
    /// This NuiSensorChooser has a connected and started sensor.
    /// </summary>
    KinectSensorStatusStarted = 0x00000001,

    /// <summary>
    /// The available sensor is not powered.  If it receives power we
    /// will try to use it automatically.
    /// </summary>
    KinectSensorStatusNotPowered = 0x00000002,

    /// <summary>
    /// There is not enough bandwidth on the USB controller available
    /// for this sensor. Can recover in some cases
    /// </summary>
    KinectSensorStatusInsufficientBandwidth = 0x00000004,

    /// <summary>
    /// Available sensor is in use by another application.
	/// Will recover once the other application releases its sensor
    /// </summary>
    KinectSensorStatusConflict = 0x00000008,

    /// <summary>
    /// Don't have a sensor yet, a sensor is initializing, you may not get it
	/// Can't trust the state of the sensor yet
    /// </summary>
    KinectSensorStatusInitializing = 0x00000010,

    /// <summary>
    /// Available sensor is not genuine.
    /// </summary>
    KinectSensorStatusNotGenuine = 0x00000020,

    /// <summary>
    /// Available sensor is not supported
    /// </summary>
    KinectSensorStatusNotSupported = 0x00000040,

    /// <summary>
    /// Available sensor has an error
    /// </summary>
    KinectSensorStatusError = 0x00000080,
} KINECT_SENSOR_STATUS;

// Structure for the frame data for depth/color
// take note of cbBytesPerPixel 
typedef struct _KinectImageFrameFormat
{   
	DWORD dwStructSize;
	DWORD dwHeight;
	DWORD dwWidth;
	USHORT cbBytesPerPixel;
	ULONG cbBufferSize;
} KINECT_IMAGE_FRAME_FORMAT;

// Skeleton Selection mode
typedef enum _KINECT_SKELETON_SELECTION_MODE
{
    SkeletonSelectionModeDefault    = 0,
    SkeletonSelectionModeClosest1	= 1,
    SkeletonSelectionModeClosest2	= 2,
    SkeletonSelectionModeSticky1	= 3,
    SkeletonSelectionModeSticky2	= 4,
    SkeletonSelectionModeActive1	= 5,
    SkeletonSelectionModeActive2	= 6,
} KINECT_SKELETON_SELECTION_MODE;

// exported api's
extern "C"
{
	// For enumerating the sensors to find the PortID
	KINECT_API UINT APIENTRY KinectGetSensorCount();
	KINECT_API bool APIENTRY KinectGetPortIDByIndex( UINT index, ULONG cchPortID, _Out_cap_(cchPortID) WCHAR* pwcPortID );
	
	// Create instance of the Kinect Sensor
	// The device we be selected and the default streams initialized
	// Color & Depth - 640x480 / with PlayerIndex / Near Mode off
	//
	// In the event the sensor is not connected, you can connect it afterwards
	// but Initialization/Startup will occur on the Notifaction from Nui 
	// this may cause a hickup in the update loop, but should be expected given the 
	// sensor is getting plugged in
	KINECT_API HKINECT APIENTRY KinectOpenDefaultSensor();
	KINECT_API HKINECT APIENTRY KinectOpenSensor( _In_z_ const WCHAR* wcPortID );

	// close down resources for the sensor
	// handle will still be valid, but you will have to call "Open" again
	KINECT_API void APIENTRY KinectCloseSensor( _In_ HKINECT hKinect );

	// Sensor properties, gets the connection id the senosr is using
	KINECT_API const WCHAR* APIENTRY KinectGetPortID( _In_ HKINECT hKinect );
	// Get the NUI_ status value from the Sensor
	KINECT_API HRESULT APIENTRY KinectGetNUISensorStatus( _In_ HKINECT hKinect );
	// internal state of the wrapper
	KINECT_API KINECT_SENSOR_STATUS APIENTRY KinectGetKinectSensorStatus( _In_ HKINECT hKinect );

	// enable the different stream
	KINECT_API HRESULT APIENTRY KinectEnableIRStream( _In_ HKINECT hKinect, NUI_IMAGE_RESOLUTION resolution, _Out_opt_ KINECT_IMAGE_FRAME_FORMAT* pFrame );
	KINECT_API void APIENTRY KinectDisableIRStream( _In_ HKINECT hKinect );

	KINECT_API HRESULT APIENTRY KinectEnableColorStream( _In_ HKINECT hKinect, NUI_IMAGE_RESOLUTION resolution, _Out_opt_ KINECT_IMAGE_FRAME_FORMAT* pFrame );
	KINECT_API void APIENTRY KinectDisableColorStream( _In_ HKINECT hKinect );

	KINECT_API HRESULT APIENTRY KinectEnableDepthStream( _In_ HKINECT hKinect, bool bNearMode, NUI_IMAGE_RESOLUTION resolution, _Out_opt_ KINECT_IMAGE_FRAME_FORMAT* pFrame );
	KINECT_API void APIENTRY KinectDisableDepthStream( _In_ HKINECT hKinect );

	KINECT_API HRESULT APIENTRY KinectEnableSkeletalStream( _In_ HKINECT hKinect, bool bSeatedSkeltons, KINECT_SKELETON_SELECTION_MODE mode, _Out_opt_ const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothParams );
	KINECT_API void APIENTRY KinectDisableSkeletalStream( _In_ HKINECT hKinect );

	// function to get the frame data from the sensor
	KINECT_API bool APIENTRY KinectIsColorFrameReady( _In_ HKINECT hKinect );
	KINECT_API bool APIENTRY KinectIsDepthFrameReady( _In_ HKINECT hKinect );
	KINECT_API bool APIENTRY KinectIsSkeletonFrameReady( _In_ HKINECT hKinect );
	KINECT_API bool APIENTRY KinectAreAllFramesReady( _In_ HKINECT hKinect );
		
	// color frame
	KINECT_API HRESULT APIENTRY KinectGetColorFrameFormat( _In_ HKINECT hKinect, _Inout_ KINECT_IMAGE_FRAME_FORMAT* pFrame );
	KINECT_API HRESULT APIENTRY KinectGetColorFrame( _In_ HKINECT hKinect, ULONG cbBufferSize, _Out_cap_(cbBufferSize) BYTE* pColorBuffer, _Out_opt_ LONGLONG* liTimeStamp );
	
	// depth frame 
	KINECT_API HRESULT APIENTRY KinectGetDepthFrameFormat( _In_ HKINECT hKinect, _Inout_ KINECT_IMAGE_FRAME_FORMAT* pFrame );
	KINECT_API HRESULT APIENTRY KinectGetDepthFrame( _In_ HKINECT hKinect, ULONG cbBufferSize, _Out_cap_(cbBufferSize) BYTE* pDepthBuffer, _Out_opt_ LONGLONG* liTimeStamp );

	// skeletal frame
	KINECT_API HRESULT APIENTRY KinectGetSkeletonFrame( _In_ HKINECT hKinect, _Out_ NUI_SKELETON_FRAME* pSkeletons );
};

