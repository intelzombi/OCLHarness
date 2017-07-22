#include <windows.h>
#pragma warning( disable : 4005 )
#include "dxgi.h"
#include "d3d11.h"
#pragma warning( default : 4005 )
#include <d3dx11.h>
#pragma warning( disable : 4800 )



#include "CL\cl.h"
#include "CL\cl_gl_ext.h"
#include "CL\cl_ext.h"
#include "CL\cl_gl.h"
#include "CL\cl_platform.h"
#include "CL\cl_d3d11.h"
#include "CL\opencl.h"

#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>


// a constant buffer to store mouse information
struct Color
{
    float red;
	float green;
    float blue;
	float alpha;
};

__declspec(align(16))
struct CB_Colors
{
	Color colors[16];
};

__declspec(align(16))
struct CB_WindowDim
{
	UINT width;
    UINT height;
};

__declspec(align(16))
struct CB_CSInfo
{
	UINT time;
	UINT reset;
    UINT windowX;
    UINT windowY;
};

struct WORK_DIMENSIONS
{
    cl_uint groupSizeX  ;
    cl_uint groupSizeY  ;
    cl_uint groupNumX   ;
    cl_uint groupNumY   ;
    cl_uint worksetSizeX;
    cl_uint worksetSizeY;

};

struct CA_STATE
{
    int state;
};

enum device_type
{
	gpu,
	cpu
};

struct CLKernelItem
{
	cl_kernel					kernel;
	cl_program					program;
    char                        entryName[256];
    char                        platformName[256];
    char                        deviceName[256];
    char                        buildRule[256];
	cl_context					context;
	cl_command_queue			cmdQueue;
	device_type					deviceType;
	cl_device_id				deviceID;
};

struct ComputeShaderItem 
{
    ID3D11ComputeShader*        cmpShader;
    TCHAR                       entryName[256];
    ID3DBlob*					shaderData;

};

struct PixelShaderItem
{
    ID3D11PixelShader*          pxlShader;
    TCHAR                       entryName[256];
    ID3DBlob*					shaderData;
};

struct VertexShaderItem
{
    ID3D11VertexShader*         vtxShader;
    ID3DBlob*					shaderData;
    TCHAR                       entryName[256];
};

struct BlendStateItem
{
    D3D11_BLEND_DESC            blendDesc;
    ID3D11BlendState*           blendState;
    TCHAR                       stateName[256];

};

struct RasterStateItem
{
    D3D11_RASTERIZER_DESC       rasterDesc;
    ID3D11RasterizerState*      rasterState;
    TCHAR                       rasterName[256];

};

struct DepthStencilStateItem
{
    D3D11_DEPTH_STENCIL_DESC    depthStencilDesc;
    ID3D11DepthStencilState*    depthStencilState;
    TCHAR                       depthStencilName[256];
};

struct ConstantBufferItem 
{
	D3D11_BUFFER_DESC           cbDesc;
	ID3D11Buffer*	            pBff;
	int				            size;
    TCHAR                       constantBufferName[256];
};

struct SamplerStateItem
{
    D3D11_SAMPLER_DESC          samplerDesc;
    ID3D11SamplerState*         samplerState;
    TCHAR                       samplerName[256];

};

struct InputLayoutItem 
{
    ID3D11InputLayout*          IPLO;
    TCHAR                       inputLayoutName[256];
}; 


HWND                        g_hWnd                  = NULL;
HINSTANCE                   g_hInst                 = NULL;
UINT                        g_WindowWidth           = 1600;
UINT                        g_WindowHeight          = 900;

UINT                        g_DCGroupsX             = 10;
UINT                        g_DCGroupsY             = 30;
UINT                        g_GroupSizeX            = 160;
UINT                        g_GroupSizeY            = 3;

bool					    g_WindowedMode          = false;
bool                        DX_CS                   = false;
bool                        OCL_GPU                 = true;
bool                        OCL_CPU                 = true;
bool                        g_boundaryTest          = false; // only used to set boundry colors of shared buffer for testing 
bool                        g_counterReset          = true;  // turn off counter and you will not reset your buffer values. Keeps counter set to 0

cl_uint                     resetImage              = 0;
cl_uint                     theTime;
CB_CSInfo                   g_CSInfo;
CB_Colors				    g_Colors;
CB_WindowDim				g_WindowDim;
CA_STATE *					m_pCAStateBuffer;
WORK_DIMENSIONS*            myDim;


ID3D11Device*               g_pd3dDevice            = NULL;
ID3D11DeviceContext *       g_pd3dImmediateContext  = NULL;
IDXGISwapChain*             g_pSwapChain            = NULL;

ID3D11Texture2D*            g_pDepthStencilBuffer;  // not used
ID3D11RenderTargetView*     g_pRenderTargetView     = NULL;
ID3D11DepthStencilView*     g_pDepthStencilView;

ID3D11Buffer*               g_cbColors;

//  These are the buffer and view pointers
ID3D11Buffer*               m_pCABuffer0;       // a general buffer for storing cell state information
ID3D11Buffer*               m_pCABuffer1;       // a general buffer for storing cell state information
cl_mem						m_cl_gpu_CABuffer0;
cl_mem						m_cl_gpu_CABuffer1;
cl_mem						m_cl_cpu_CABuffer0;
cl_mem						m_cl_cpu_CABuffer1;
cl_mem                      m_cl_gpu_dimensions;
cl_mem                      m_cl_cpu_dimensions;
ID3D11ShaderResourceView*   m_pCABuffer0SRV;    // a read-only view of the first cell state buffer
ID3D11ShaderResourceView*	m_pCABuffer1SRV;    // a read-write view of the first cell state buffer
ID3D11UnorderedAccessView*  m_pCABuffer0UAV;
ID3D11UnorderedAccessView*  m_pCABuffer1UAV;

ID3D11InputLayout*          m_pVertexLayout;        // vertex layout
ID3D11Buffer*               m_pVertexBuffer;        // vertex buffer
ID3D11VertexShader*         m_pVertexShader;        // vertex shader
ID3D11PixelShader*          m_pPixelShader;         // pixel shader
ID3D11ComputeShader*        m_pComputeShader;        // compute shader

RasterStateItem *		    rsi;
BlendStateItem *		    bsi;
ConstantBufferItem *	    pscbi;
ConstantBufferItem *	    pscbi2;
ConstantBufferItem *	    cscbi;
SamplerStateItem *		    ssi;                    // not utilized

DXGI_MODE_DESC              g_scModeDesc;           // not used here really.  I added it to debug a flickering problem I had.

//IVY Bridge available OCL Extensions
//  cl_intel_dx9_media_sharing 
//  cl_khr_3d_image_writes 
//  cl_khr_byte_addressable_store 
//  cl_khr_d3d10_sharing 
//  cl_khr_d3d11_sharing 
//  cl_khr_depth_images 
//  cl_khr_dx9_media_sharing 
//  cl_khr_gl_depth_images 
//  cl_khr_gl_event 
//  cl_khr_gl_msaa_sharing 
//  cl_khr_gl_sharing 
//  cl_khr_global_int32_base_atomics 
//  cl_khr_global_int32_extended_atomics 
//  cl_khr_icd 
//  cl_khr_image2d_from_buffer 
//  cl_khr_local_int32_base_atomics 
//  cl_khr_local_int32_extended_atomics 
//  cl_intel_accelerator 
//  cl_intel_motion_estimation 
//
// OCL D3D11 interop functions
// these are the declarations for the extention functions for D3D11 and OpenCL
// Not all are used in this program but the declarations are here for completeness
typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetDeviceIDsFromD3D11KHRFuncType)(
    cl_platform_id             platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void *                     d3d_object,
    cl_d3d11_device_set_khr    d3d_device_set,
    cl_uint                    num_entries,
    cl_device_id *             devices,
    cl_uint *                  num_devices) CL_API_SUFFIX__VERSION_1_2;
clGetDeviceIDsFromD3D11KHRFuncType clGetDeviceIDsFromD3D11KHRFunc;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11BufferKHRFuncType)(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D11Buffer * resource,
    cl_int *       errcode_ret) CL_API_SUFFIX__VERSION_1_2;
clCreateFromD3D11BufferKHRFuncType clCreateFromD3D11BufferKHRFunc;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture2DKHRFuncType)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;
clCreateFromD3D11Texture2DKHRFuncType clCreateFromD3D11Texture2DKHRFunc;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture3DKHRFuncType)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;
clCreateFromD3D11Texture3DKHRFuncType clCreateFromD3D11Texture3DKHRFunc;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueAcquireD3D11ObjectsKHRFuncType)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;
clEnqueueAcquireD3D11ObjectsKHRFuncType clEnqueueAcquireD3D11ObjectsKHRFunc;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueReleaseD3D11ObjectsKHRFuncType)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    cl_mem *         mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;
clEnqueueReleaseD3D11ObjectsKHRFuncType clEnqueueReleaseD3D11ObjectsKHRFunc;


cl_kernel					g_cl_gpu_kernelCA;
cl_kernel					g_cl_cpu_kernelCA;
cl_program					g_cl_gpu_programCA;
cl_program					g_cl_cpu_programCA;
cl_context					g_cl_gpu_platformContext;
cl_context					g_cl_cpu_platformContext;
cl_command_queue			g_cl_gpu_cmdQueue;
cl_command_queue			g_cl_cpu_cmdQueue;

HANDLE                      EvtOCLComplete; 
HANDLE                      EvtDXComplete ; 
HANDLE                      EvtGPUComplete ; 

std::vector<CLKernelItem *>	g_kernelItemList;


LPTSTR pCSFileLoc;
LPTSTR pPSFileLoc;
LPTSTR pVSFileLoc;
LPTSTR pCLFileLoc;
LPTSTR pCLEntry;
LPTSTR pCLGPUBuildRule;
LPTSTR pCLCPUBuildRule;
LPTSTR pCsvFileLoc;



template <class T> void swap( T* &x, T* &y )        // swap two pointers
    { T* t = x; x = y; y = t; }
