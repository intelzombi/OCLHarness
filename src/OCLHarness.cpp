// OCLHarness.cpp : This Harness was created by Hugh Smith: Senior Software Engineer at Intel to learn the basics 
// of OpenCL and compare it to Direct Compute:cg
// The program uses both OpenCL GPU and CPU as well as DX11 Compute shaders.  This is meant to be a survey of basic
// methods and comparison for these paths.  The programs use structured buffers not images in the shaders/kernals.
// I did hijack some code from two sources: A Direct Compute Lab that was produced by Matt Sandy from Microsoft and
// Resource Sharing OpenCL sample that was created by by Intel.  While probably unrecognizable in this program I feel
// it is important to give credit to others work that helped inspire this effort.
// There is very little error handling in this code.  For me I want as clean a path as possible when learning a new
// API.  I've unrolled many of the helper functions into a single stream to keep from having to bounce back and forth
// on the initialization paths.


#include "OCLHarness.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
HRESULT             InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static HWND hWnd					= NULL;
HBITMAP bmpBackground;

bool randSet(){ 
    srand(/*time(NULL)*/ 1935);
    return true; }
bool setRand = randSet();


void LoadIniFile(LPWSTR lpCmdLine)
{
    //------Config File Initialization--------------------
        // grab vars from config file
    int iReadVal = 0;
	float fReadVal = 0.0f;
    float farray[4] = {1.0f, 1.0f, 1.0f ,1.0f};
    LPTSTR pConfigFileLoc;
    CINIFileReader CConfigReader;
    int iSizeBuf = 1024;

    pConfigFileLoc                  = new TCHAR[iSizeBuf];
    pCSFileLoc                      = new TCHAR[iSizeBuf];
    pPSFileLoc                      = new TCHAR[iSizeBuf];
    pVSFileLoc                      = new TCHAR[iSizeBuf];
    pCLFileLoc                      = new TCHAR[iSizeBuf];
    pCsvFileLoc                     = new TCHAR[iSizeBuf];
    pCLEntry                        = new TCHAR[iSizeBuf];;
    pCLGPUBuildRule                 = new TCHAR[iSizeBuf];;
    pCLCPUBuildRule                 = new TCHAR[iSizeBuf];;

    _stprintf_s(pConfigFileLoc, iSizeBuf, lpCmdLine); 
    CConfigReader.SetIniFile( pConfigFileLoc );


    iReadVal = CConfigReader.ReadString(L"INITVALS", L"VERTEXSHADER" , pVSFileLoc  , iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"PIXELSHADER"  , pPSFileLoc  , iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"OCLKERNEL"    , pCLFileLoc  , iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"COMPUTESHADER", pCSFileLoc  , iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"ENTRYNAME"    , pCLEntry    , iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"GPUBUILDRULE"    , pCLGPUBuildRule, iSizeBuf);
    iReadVal = CConfigReader.ReadString(L"INITVALS", L"CPUBUILDRULE"    , pCLCPUBuildRule, iSizeBuf);

    iReadVal = CConfigReader.ReadString(L"INITVALS", L"CSVLOC", pCsvFileLoc, iSizeBuf);

    if(( iReadVal == CConfigReader.GetStringErrorValue()) || (wcscmp(pCsvFileLoc, DEFAULT_STRING) == 0))
    {//if iReadInt test fails default to csv file 
        wsprintf(pCsvFileLoc, L".\\WickedWeasel.csv");        
    }


    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"RESOLUTIONWIDTH");

    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_WindowWidth = iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"RESOLUTIONHEIGHT");

    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_WindowHeight = iReadVal;
    }


    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"WINDOWEDMODE");

    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_WindowedMode = (bool) iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"DX_CS");  
                                                                   
    if (iReadVal != CConfigReader.GetIntErrorValue())              
    {
        DX_CS = (bool) iReadVal;
    }
     
    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"NUM_GROUPSX");
                                                                      
    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_DCGroupsX = iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"NUM_GROUPSY");
                                                                     
    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_GroupSizeX = iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"GROUPSIZEX");
                                                                      
    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_DCGroupsX = iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"GROUPSIZEY");
                                                                     
    if (iReadVal != CConfigReader.GetIntErrorValue())
    {
        g_GroupSizeY = iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"OCL_GPU");       
                                                                   
    if (iReadVal != CConfigReader.GetIntErrorValue())              
    {
        OCL_GPU = (bool) iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"OCL_CPU");  
                                                                   
    if (iReadVal != CConfigReader.GetIntErrorValue())              
    {
        OCL_CPU = (bool) iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"BOUNDARYTEST");     
                                                                     
    if (iReadVal != CConfigReader.GetIntErrorValue())              
    {
        g_boundaryTest = (bool) iReadVal;
    }

    iReadVal = CConfigReader.ReadInt(L"INITVALS", L"COUNTERRESET");  
                                                                   
    if (iReadVal != CConfigReader.GetIntErrorValue())              
    {
        g_counterReset = (bool) iReadVal;
    }

    delete pConfigFileLoc  ;   
    return;
}


HRESULT CreateSwapChain ()
{
    HRESULT hr = S_OK;

 
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        //D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );
    DXGI_RATIONAL dr;
    dr.Denominator = 0;
    dr.Numerator = 0;
    g_scModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    g_scModeDesc.RefreshRate = dr;
    g_scModeDesc.Width = g_WindowWidth;
    g_scModeDesc.Height = g_WindowHeight;
    g_scModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    //Render Target Buffer Description
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferDesc.Width  = g_WindowWidth  ;
    sd.BufferDesc.Height = g_WindowHeight ;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//_SRGB;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = true;//g_WindowedMode;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    D3D_FEATURE_LEVEL dfl[] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT    numberOfLevels = 1;
    D3D_FEATURE_LEVEL supportedFeature;
    UINT uCreationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;// hmmm... strange behavior on debug 
                                                             // stop goes to windowed mode and sometimes 
                                                             // need to hit stop twice.

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
      g_driverType = driverTypes[driverTypeIndex];
      hr = D3D11CreateDeviceAndSwapChain( NULL, 
                                          g_driverType, 
                                          NULL, 
                                          NULL,//uCreationFlags
                                          NULL,//dfl,
                                          0,//numberOfLevels,
                                          D3D11_SDK_VERSION, 
                                          &sd, 
                                          &g_pSwapChain, 
                                          &g_pd3dDevice,
                                          &supportedFeature,
                                          &g_pd3dImmediateContext);
      if( SUCCEEDED( hr ) )
          break;


    }

    return hr;
}

void CreateRenderTarget ()
{
    
	ID3D11Texture2D* pBackBuffer = NULL;
	g_pSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (LPVOID*) &pBackBuffer );
	g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    
	
    D3D11_VIEWPORT vp;
    vp.Width   = (float)g_WindowWidth;
    vp.Height  = (float)g_WindowHeight;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pd3dImmediateContext->RSSetViewports( 1, &vp );


	pBackBuffer->Release();
	
    rsi = new RasterStateItem();
    ZeroMemory( &rsi->rasterDesc, sizeof( D3D11_RASTERIZER_DESC ) );
    rsi->rasterDesc.FillMode = D3D11_FILL_SOLID;
    rsi->rasterDesc.CullMode = D3D11_CULL_NONE;
    rsi->rasterDesc.FrontCounterClockwise = FALSE;
    rsi->rasterDesc.DepthBias = 0;
    rsi->rasterDesc.DepthBiasClamp = 0;
    rsi->rasterDesc.SlopeScaledDepthBias = 0;
    rsi->rasterDesc.DepthClipEnable = FALSE;
    rsi->rasterDesc.ScissorEnable = FALSE;
    rsi->rasterDesc.MultisampleEnable = FALSE;
    rsi->rasterDesc.AntialiasedLineEnable = FALSE;
    g_pd3dDevice->CreateRasterizerState( &rsi->rasterDesc, &rsi->rasterState );
    wcscpy_s( rsi->rasterName, L"CullNone" );

	bsi = new BlendStateItem();
    bsi->blendState = NULL;
    ZeroMemory( &bsi->blendDesc, sizeof( D3D11_BLEND_DESC ) );
    bsi->blendDesc.IndependentBlendEnable = FALSE;
    bsi->blendDesc.AlphaToCoverageEnable = FALSE;
    bsi->blendDesc.RenderTarget[0].BlendEnable = FALSE;
    bsi->blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bsi->blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    bsi->blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bsi->blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bsi->blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bsi->blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bsi->blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    g_pd3dDevice->CreateBlendState( &bsi->blendDesc, &bsi->blendState );
    wcscpy_s( bsi->stateName, L"NoBlending" );
	
}

void CreateDXSharedBufferResources()
{
    HRESULT hr = S_OK;


    
    m_pCAStateBuffer = new CA_STATE[g_WindowWidth*g_WindowHeight];
	//srand(time(NULL));
    // initialize each cell to a random state
    for( unsigned int x = 0; x < g_WindowWidth; x++ )
    {
        for( unsigned int y = 0; y < g_WindowHeight; y++ )
        {
            if(g_boundaryTest)
            {
                if( x==0 && y == 0 ) {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 9;
                } else if( x == 0 && y == g_WindowHeight-1 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 12;
                } else if( x == g_WindowWidth -1 && y == 0 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 2;
                } else if( x == g_WindowWidth - 1 && y == g_WindowHeight - 1 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 3;
                } else if ( x==0 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 4;
                } else if ( x == g_WindowWidth - 1 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 5;
                } else if ( y == 0 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 6;
                } else if ( y == g_WindowHeight - 1 )
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 7;
                } else 
                {
			        m_pCAStateBuffer[ x + y * g_WindowWidth ].state = 8;
                }
            }else
            {
                m_pCAStateBuffer[ x + y * g_WindowWidth ].state = rand()%16;

            }

              
            
        }
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof( bd ) );

    bd.StructureByteStride = sizeof(CA_STATE);
    bd.ByteWidth = sizeof(CA_STATE) * g_WindowWidth*g_WindowHeight;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    bd.BindFlags |= D3D11_BIND_UNORDERED_ACCESS; 
    bd.MiscFlags |= D3D11_RESOURCE_MISC_SHARED;     // For grins try removing this flag 
                                                    // and watch the OpenCL GPU path tank 
                                                    // because of copies it doesn't need to make
                                                    // this flag prevents that copy and forces sharingfggggggcgcgcg
    bd.Usage = D3D11_USAGE_DEFAULT;
    D3D11_SUBRESOURCE_DATA InitialSRData;
    ZeroMemory( &InitialSRData, sizeof( InitialSRData ) );
    InitialSRData.pSysMem = m_pCAStateBuffer;
    g_pd3dDevice->CreateBuffer( &bd, &InitialSRData, &m_pCABuffer0 ) ;
	g_pd3dDevice->CreateBuffer( &bd, &InitialSRData, &m_pCABuffer1 );


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescSRV;
    ZeroMemory( &viewDescSRV, sizeof( viewDescSRV ) );
    viewDescSRV.Format = DXGI_FORMAT_UNKNOWN;
    viewDescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    viewDescSRV.Buffer.FirstElement = 0;
    viewDescSRV.Buffer.NumElements =  g_WindowWidth*g_WindowHeight;

    // create the shader resource view of the buffer with this description
    g_pd3dDevice->CreateShaderResourceView( m_pCABuffer0, &viewDescSRV, &m_pCABuffer0SRV ) ;
    g_pd3dDevice->CreateShaderResourceView( m_pCABuffer1, &viewDescSRV, &m_pCABuffer1SRV ) ;

    D3D11_UNORDERED_ACCESS_VIEW_DESC viewDescUAV;
    ZeroMemory( &viewDescUAV, sizeof( viewDescUAV ) );
    viewDescUAV.Format = DXGI_FORMAT_UNKNOWN;
    viewDescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    viewDescUAV.Buffer.FirstElement = 0;
    viewDescUAV.Buffer.NumElements = g_WindowWidth*g_WindowHeight;

    // create the unordered access view of the buffer with this description
    g_pd3dDevice->CreateUnorderedAccessView( m_pCABuffer0, &viewDescUAV, &m_pCABuffer0UAV);
    g_pd3dDevice->CreateUnorderedAccessView( m_pCABuffer1, &viewDescUAV, &m_pCABuffer1UAV);

}
void CreatePixelShader()
{

    HRESULT hr;
    ID3DBlob* pBlob = NULL;         // used to store the compiled pixel shader
    ID3DBlob* pErrorBlob = NULL;    // used to store any compilation errors

    hr = D3DX11CompileFromFile(
        //L"src/PixelShader.psh",     // use the code in this file
        pPSFileLoc,
        NULL,                       // don't use additional defines
        NULL,                       // don't use additional includes
        "CAPS",					    // compile this function
        "ps_5_0",                   // use pixel shader 5.0
        NULL,                       // no compile flags
        NULL,                       // no effect flags
        NULL,                       // don't use a thread pump
        &pBlob,                     // store the compiled shader here
        &pErrorBlob,                // store any errors here
        NULL );                     // no thread pump is used, so no asynchronous HRESULT is needed

    if( pErrorBlob )
        OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );


    // if the pixel shader was compiled successfully, create it on the GPU
    hr = g_pd3dDevice->CreatePixelShader(
        pBlob->GetBufferPointer(),  // use the pixel shader that was compiled here
        pBlob->GetBufferSize(),     // with this size
        NULL,                       // don't use any dynamic linkage
        &m_pPixelShader );          // store the reference to the pixel shader here
	
}
void CreatePixelShaderResources()
{

	HRESULT hr = S_OK;
    pscbi = new ConstantBufferItem();
    pscbi->pBff = NULL;
    ZeroMemory( &pscbi->cbDesc, sizeof(D3D11_BUFFER_DESC) );
    pscbi->cbDesc.ByteWidth = sizeof( CB_Colors );                 
    pscbi->cbDesc.Usage = D3D11_USAGE_DYNAMIC;                     
    pscbi->cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;          
    pscbi->cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;         
    pscbi->cbDesc.MiscFlags = 0;                                   
    hr = g_pd3dDevice->CreateBuffer( &pscbi->cbDesc, NULL, &pscbi->pBff );
    wcscpy_s( pscbi->constantBufferName, L"LightEQ" );

	hr = S_OK;
    pscbi2 = new ConstantBufferItem();
    pscbi2->pBff = NULL;
    ZeroMemory( &pscbi2->cbDesc, sizeof(D3D11_BUFFER_DESC) );
    pscbi2->cbDesc.ByteWidth = sizeof( CB_Colors );                 
    pscbi2->cbDesc.Usage = D3D11_USAGE_DYNAMIC;                     
    pscbi2->cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;          
    pscbi2->cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;         
    pscbi2->cbDesc.MiscFlags = 0;                                   
    hr = g_pd3dDevice->CreateBuffer( &pscbi2->cbDesc, NULL, &pscbi2->pBff );
    wcscpy_s( pscbi2->constantBufferName, L"WindowDim" );

    ssi = new SamplerStateItem();
    ssi->samplerState = NULL;
    ZeroMemory( &ssi->samplerDesc, sizeof( D3D11_SAMPLER_DESC ) );
    ssi->samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    ssi->samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    ssi->samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    ssi->samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    ssi->samplerDesc.MipLODBias = 0.0f;
    ssi->samplerDesc.MaxAnisotropy = 16;
    ssi->samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    
    ssi->samplerDesc.MinLOD = 0.0f;
    ssi->samplerDesc.MaxLOD = FLT_MAX;
    g_pd3dDevice->CreateSamplerState( &ssi->samplerDesc, &ssi->samplerState );
    wcscpy_s( ssi->samplerName, L"simpleSampler" );
    
    g_WindowDim.width = g_WindowWidth;
    g_WindowDim.height = g_WindowHeight;
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    g_pd3dImmediateContext->Map( pscbi2->pBff, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

    // copy the color info data to the constant buffer
    memcpy( MappedResource.pData, &g_WindowDim, sizeof( g_WindowDim ) );

    
    g_pd3dImmediateContext->Unmap( pscbi2->pBff, 0 );


}
HRESULT CreateComputeShader()
{
    HRESULT hr = S_OK;

    ID3DBlob* pBlob = NULL;         // used to store the compiled compute shader
    ID3DBlob* pErrorBlob = NULL;    // used to store any compilation errors
   
    // compile the compute shader from the file specified
    hr = D3DX11CompileFromFile(
        //C:\Projects\OpenCL\OCLHarness\src\ComputeShaderRootTest.csh
        //L"src/ComputeShaderRootTest.csh",   // use the code in this file
        pCSFileLoc,
        NULL,                       // don't use additional defines
        NULL,                       // don't use additional includes
        "CellularAutomataHS",       // compile this function
        "cs_5_0",                   // use compute shader 5.0
        NULL,                       // no compile flags
        NULL,                       // no effect flags
        NULL,                       // don't use a thread pump
        &pBlob,                     // store the compiled shader here
        &pErrorBlob,                // store any errors here
        NULL );                     // no thread pump is used, so no asynchronous HRESULT is needed

    // if there were any errors, display them
    if( pErrorBlob )
        OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );

    if( FAILED(hr) )
        return hr;

    // if the compute shader was compiled successfully, create it on the GPU
    g_pd3dDevice->CreateComputeShader(
        pBlob->GetBufferPointer(),  // use the compute shader that was compiled here
        pBlob->GetBufferSize(),     // with this size
        NULL,                       // don't use any dynamic linkage
        &m_pComputeShader );      // store the reference to the compute shader here


    return hr;

}

void CreateComputeShaderResources()
{

	HRESULT hr = S_OK;
    cscbi = new ConstantBufferItem();
    cscbi->pBff = NULL;
    ZeroMemory( &cscbi->cbDesc, sizeof(D3D11_BUFFER_DESC) );
    cscbi->cbDesc.ByteWidth = sizeof( CB_Colors );                 
    cscbi->cbDesc.Usage = D3D11_USAGE_DYNAMIC;                     
    cscbi->cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;          
    cscbi->cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;         
    cscbi->cbDesc.MiscFlags = 0;                                   
    hr = g_pd3dDevice->CreateBuffer( &cscbi->cbDesc, NULL, &cscbi->pBff );
    wcscpy_s( cscbi->constantBufferName, L"PSImageUpdate" );

}

void CreateScreenVertexBuffer()
{

	// create screen quad
    float svQuad[8] = {
        -1.0f, 1.0f,
        1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
        };

    // create a vertex buffer with the screen quad coordinates
    D3D11_BUFFER_DESC vbDesc =
    {
        sizeof(svQuad),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0,
        0
    };
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = svQuad;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateBuffer( &vbDesc, &InitData, &m_pVertexBuffer );


    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;

    // compile the vertex shader
    D3DX11CompileFromFile(
        //L"src/VertexShader.vsh",    // use the code in this file
        pVSFileLoc,
        NULL,                       // don't use additional defines
        NULL,                       // don't use additional includes
        "VS",                       // compile this function
        "vs_5_0",                   // use vertex shader 5.0
        NULL,                       // no compile flags
        NULL,                       // no effect flags
        NULL,                       // don't use a thread pump
        &pBlob,                     // compile the shader here
        &pErrorBlob,                // flush any errors here
        NULL );                     // no thread pump used, so no asynchronous HRESULT is needed

	g_pd3dDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pVertexShader );
    D3D11_INPUT_ELEMENT_DESC layout[] =
    { { "SV_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }; 
    g_pd3dDevice->CreateInputLayout( layout, 1, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pVertexLayout );

    pBlob->Release();
	if( pErrorBlob)
    pErrorBlob->Release();



}
HRESULT SetColorCB()
{
    HRESULT hr = S_OK;
	for(int i=0; i<16; i++)
	{
		//g_Colors.colors[i].red = float(rand()%256)/255;
		//g_Colors.colors[i].green = float(rand()%256)/255;
		//g_Colors.colors[i].blue = float(rand()%256)/255;
		//g_Colors.colors[i].alpha = 1.0f;

// 0 	0.494117647     0.0784313753     0.635294139       1.00000000 
// 1 	0.941176474     0.227450982      0.400000006       1.00000000 
// 2    0.996078432     0.690196097      0.666666687       1.00000000 
// 3    0.0745098069    0.580392182      0.819607854       1.00000000 
// 4    1.00000000      0.000000000      0.000000000       1.00000000 
// 5    0.117647059     0.905882359      0.811764717       1.00000000 
// 6    0.482352942     0.721568644      0.952941179       1.00000000 
// 7    0.384313732     0.925490201      0.796078444       1.00000000 
// 8    0.176470593     0.325490206      0.486274511       1.00000000 
// 9    0.545098066     0.458823532      0.894117653       1.00000000 
// 10	0.788235307     0.525490224      0.443137258       1.00000000 
// 11	0.121568628     0.901960790      0.423529416       1.00000000 
// 12	0.000000000     1.00000000       0.000000000       1.00000000 
// 13	0.988235295     0.403921574      0.211764708       1.00000000 
// 14	0.352941185     0.411764711      0.513725519       1.00000000 
// 15	0.141176477     0.152941182      0.235294119       1.00000000 	

		
		if( i == 0 )
        {
		    g_Colors.colors[i].red   = float(126)/255;
		    g_Colors.colors[i].green = float(20)/255;
		    g_Colors.colors[i].blue  = float(162)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 1 )
        {
    		g_Colors.colors[i].red   = float(240)/255;
    		g_Colors.colors[i].green = float(58)/255;
    		g_Colors.colors[i].blue  = float(102)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 2 )
        {
		    g_Colors.colors[i].red   = float(254)/255;
		    g_Colors.colors[i].green = float(176)/255;
		    g_Colors.colors[i].blue  = float(170)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 3 )
        {
    		g_Colors.colors[i].red   = float(19)/255;
    		g_Colors.colors[i].green = float(148)/255;
    		g_Colors.colors[i].blue  = float(209)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 4 )
        {
		    g_Colors.colors[i].red   = float(255)/255;
		    g_Colors.colors[i].green = float(0)/255;
		    g_Colors.colors[i].blue  = float(0)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 5 )
        {
    		g_Colors.colors[i].red   = float(30)/255;
    		g_Colors.colors[i].green = float(231)/255;
    		g_Colors.colors[i].blue  = float(207)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 6 )
        {
		    g_Colors.colors[i].red   = float(123)/255;
		    g_Colors.colors[i].green = float(184)/255;
		    g_Colors.colors[i].blue  = float(243)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 7 )
        {
    		g_Colors.colors[i].red   = float(98)/255;
    		g_Colors.colors[i].green = float(236)/255;
    		g_Colors.colors[i].blue  = float(203)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 8 )
        {
		    g_Colors.colors[i].red   = float(45)/255;
		    g_Colors.colors[i].green = float(83)/255;
		    g_Colors.colors[i].blue  = float(124)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 9 )
        {
    		g_Colors.colors[i].red   = float(139)/255;
    		g_Colors.colors[i].green = float(117)/255;
    		g_Colors.colors[i].blue  = float(228)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 10 )
        {
		    g_Colors.colors[i].red   = float(201)/255;
		    g_Colors.colors[i].green = float(134)/255;
		    g_Colors.colors[i].blue  = float(113)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 11 )
        {
    		g_Colors.colors[i].red   = float(31)/255;
    		g_Colors.colors[i].green = float(230)/255;
    		g_Colors.colors[i].blue  = float(108)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 12 )
        {
    		g_Colors.colors[i].red   = float(0)/255;
    		g_Colors.colors[i].green = float(255)/255;
    		g_Colors.colors[i].blue  = float(0)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 13 )
        {
		    g_Colors.colors[i].red   = float(252)/255;
		    g_Colors.colors[i].green = float(103)/255;
		    g_Colors.colors[i].blue  = float(54)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 14 )
        {
		    g_Colors.colors[i].red   = float(90)/255;
		    g_Colors.colors[i].green = float(105)/255;
		    g_Colors.colors[i].blue  = float(131)/255;
		    g_Colors.colors[i].alpha = 1.0f;        
        }
        if( i == 15 )
        {
    		g_Colors.colors[i].red   = float(36)/255;
    		g_Colors.colors[i].green = float(39)/255;
    		g_Colors.colors[i].blue  = float(60)/255;
    		g_Colors.colors[i].alpha = 1.0f;        
        }

	}
    
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    g_pd3dImmediateContext->Map( pscbi->pBff, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

    // copy the color info data to the constant buffer
    memcpy( MappedResource.pData, &g_Colors, sizeof( g_Colors ) );

    
    g_pd3dImmediateContext->Unmap( pscbi->pBff, 0 );

    return hr;
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{


    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = NULL;//hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_OCL_HARNESS );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );	
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"OCLHarness";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_SMALL );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;


   hInst = hInstance; 
   RECT rc = { 0, 0, g_WindowWidth, g_WindowHeight };

    if( g_WindowedMode)
	{
       AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
       g_hWnd = CreateWindowEx(
           NULL,
           L"OCLHarness", 
           L"CellularAutomata", 
           WS_OVERLAPPEDWINDOW, 
           0,
           0,
           rc.right - rc.left,
           rc.bottom - rc.top,
           NULL, 
           NULL, 
           hInstance, 
           NULL);
	} else {

       g_hWnd = CreateWindowEx(
           NULL,
           L"OCLHarness", 
           L"CellularAutomata", 
           WS_EX_TOPMOST | WS_POPUP, 
           0,
           0,
           rc.right - rc.left,
           rc.bottom - rc.top,
           NULL, 
           NULL, 
           hInstance, 
           NULL);
	}



   if (!g_hWnd)
   {
      return FALSE;
   }
   ShowWindow( g_hWnd, SW_NORMAL );    
   UpdateWindow(g_hWnd);


    return S_OK;

}

HRESULT InitDXDevice()
{
    HRESULT hr = S_OK;;



    //EvtOCLComplete= CreateEvent(NULL, FALSE, FALSE, NULL);
    //EvtDXComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    //EvtGPUComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    //SetEvent(EvtOCLComplete);
    //SetEvent(EvtDXComplete );
    //SetEvent(EvtGPUComplete );
    //WaitForSingleObject(EvtOCLComplete, INFINITE);
    //WaitForSingleObject(EvtDXComplete, INFINITE);
    //WaitForSingleObject(EvtGPUComplete, INFINITE);

    hr = CreateSwapChain();

    while (! g_pd3dDevice ) {}; // brief pause sometimes needed to let the system catchup.
	CreateRenderTarget();
 
	CreateScreenVertexBuffer();

	CreateDXSharedBufferResources();

    CreatePixelShader();
    CreatePixelShaderResources();
    CreateComputeShader();
    CreateComputeShaderResources();
	SetColorCB();
    
    return S_OK;
}

char* OCLFileRead(const TCHAR* fileName)
{
	FILE* pFile;
    _wfopen_s( &pFile, fileName, L"rb" );
	if(!pFile)
	{
		printf("Error opening file: %s\n",fileName);
		abort();
	}
	
	// get size of file, in chars
	fseek( pFile, 0, SEEK_END );
	long fileLen = ftell( pFile );

	char* codeString = (char*)malloc(fileLen+1);
	if(!codeString)
	{
		printf("Error allocating buffer\n");
		fclose(pFile);
		abort();
	}

	// read into string
	rewind(pFile);
	fread( codeString, 1, fileLen, pFile );

	codeString[fileLen] = '\0';

	fclose(pFile);
	return codeString;

}

void OCLHarness_Init()
{
	// Initialize opencl environment
	cl_context_properties cps[] =
	{ 
        CL_CONTEXT_D3D11_DEVICE_KHR, 
        (intptr_t)g_pd3dDevice, 
        0 
    };

	//... push to util files
	cl_uint							numPlatforms;
	cl_uint							numDevices;

	char*							platformName; 
	cl_device_type					deviceType[2]= { CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_CPU };
    int                             numberOfDeviceTypes = 2;
	cl_command_queue_properties		cmdQueueProps;
	cl_context_properties*			contextProps;
	//cl_device_id					deviceID;
	cl_platform_id					platformID = NULL;

	//cl_context						deviceContext;
	char*							sPlatformName;
	char*							sPlatformExtensions;
	char*							sDeviceExtensions = "\0";
	char*							sDeviceName;
	cl_bool							bImageSupport;
	cl_int							err;
	CLKernelItem*					kernelItem;

    myDim = new WORK_DIMENSIONS[1];
	platformName  = "Intel(R) OpenCL";//"NVIDIA CUDA";
	contextProps  = cps;
	cmdQueueProps = CL_QUEUE_PROFILING_ENABLE;


	clGetPlatformIDs( 0, NULL, &numPlatforms );
	// Create platforms
	cl_platform_id*	pPlatformIDs = (cl_platform_id*)calloc( sizeof(cl_platform_id), numPlatforms );
	clGetPlatformIDs( numPlatforms, pPlatformIDs, NULL );
	const int MAX_DEVICES = 2; //guessing there are no more than 16 devices per platform
    cl_device_id cpuDevices[MAX_DEVICES] = { 0, 0 };
	cl_device_id gpuDevices[MAX_DEVICES] = { 0, 0 };
	for( cl_uint i=0; i<numPlatforms; i++ )
	{
		char* sPlatname;
		size_t nameSize;
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_NAME, 0, NULL, &nameSize );

		sPlatname = new char[nameSize];
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_NAME, nameSize, sPlatname, NULL );
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_NAME, 0, NULL, &nameSize );
		sPlatformName = new char[nameSize];
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_NAME, nameSize, sPlatformName, NULL );
		size_t extensionsSize;
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &extensionsSize );
		sPlatformExtensions = new char[extensionsSize];
		err = clGetPlatformInfo( pPlatformIDs[i], CL_PLATFORM_EXTENSIONS, extensionsSize, sPlatformExtensions, NULL );
		
		cl_context_properties ctxProps[] = 
		{
			CL_CONTEXT_PLATFORM, 
			(cl_context_properties)pPlatformIDs[i],
			 CL_CONTEXT_D3D11_DEVICE_KHR, 
			(intptr_t)g_pd3dDevice, 
			0
		};
		
		err = clGetDeviceIDs(pPlatformIDs[i], deviceType[0], MAX_DEVICES, gpuDevices, &numDevices);
		for( UINT nd = 0; nd < numDevices; nd++ )
		{
			kernelItem = new CLKernelItem[1];
			strcpy_s( kernelItem->platformName, sPlatname );
			kernelItem->deviceType = gpu;
			kernelItem->deviceID = gpuDevices[nd];
			kernelItem->context  = clCreateContextFromType( ctxProps, deviceType[0], NULL, NULL, &err );
			kernelItem->cmdQueue = clCreateCommandQueue(kernelItem->context, gpuDevices[nd], cmdQueueProps, &err);
			err = clGetDeviceInfo( gpuDevices[nd], CL_DEVICE_NAME, 0, NULL, &nameSize );
			sDeviceName = new char[nameSize];
			err = clGetDeviceInfo( gpuDevices[nd], CL_DEVICE_NAME, nameSize, sDeviceName, NULL );
			strcpy_s( kernelItem->deviceName, sDeviceName );
			err = clGetDeviceInfo( gpuDevices[nd], CL_DEVICE_IMAGE_SUPPORT, sizeof( bImageSupport ), &bImageSupport, NULL );
			err = clGetDeviceInfo( gpuDevices[nd], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionsSize );
			sDeviceExtensions = new char[extensionsSize];
			err = clGetDeviceInfo( gpuDevices[nd], CL_DEVICE_EXTENSIONS, extensionsSize, sDeviceExtensions, NULL );

			if(strstr(sDeviceExtensions,"cl_khr_d3d11_sharing"))
			{
				bool found = true;
				g_kernelItemList.push_back(kernelItem);
			}else
			{
				delete kernelItem;
			}

            
            delete sDeviceName;
            delete sDeviceExtensions;
		}


		err = clGetDeviceIDs(pPlatformIDs[i], deviceType[1], MAX_DEVICES, cpuDevices, &numDevices);

		for( UINT nd = 0; nd < numDevices; nd++ )
		{
			kernelItem = new CLKernelItem[1];
			strcpy_s( kernelItem->platformName, sPlatname );
			kernelItem->deviceType = cpu;
			kernelItem->deviceID = cpuDevices[nd];
			err = clGetDeviceIDs(pPlatformIDs[i], deviceType[1], MAX_DEVICES, cpuDevices, &numDevices);
			kernelItem->context  = clCreateContextFromType( ctxProps, deviceType[1], NULL, NULL, &err );
			kernelItem->cmdQueue = clCreateCommandQueue( kernelItem->context, cpuDevices[nd], cmdQueueProps, &err);
			err = clGetDeviceInfo( cpuDevices[nd], CL_DEVICE_NAME, 0, NULL, &nameSize );
			sDeviceName = new char[nameSize];
			err = clGetDeviceInfo( cpuDevices[nd], CL_DEVICE_NAME, nameSize, sDeviceName, NULL );
			strcpy_s( kernelItem->deviceName, sDeviceName );					
			err = clGetDeviceInfo( cpuDevices[nd], CL_DEVICE_IMAGE_SUPPORT, sizeof( bImageSupport ), &bImageSupport, NULL );
			err = clGetDeviceInfo( cpuDevices[nd], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionsSize );
			sDeviceExtensions = new char[extensionsSize];
			err = clGetDeviceInfo( cpuDevices[nd], CL_DEVICE_EXTENSIONS, extensionsSize, sDeviceExtensions, NULL );

			if(strstr(sDeviceExtensions,"cl_khr_d3d11_sharing"))
			{
				g_kernelItemList.push_back(kernelItem);
			}else
			{
				delete kernelItem;

			}

           
            delete sDeviceName;
            delete sDeviceExtensions;

		}
        
        delete sPlatname;
        delete sPlatformExtensions;
        delete sPlatformName;
        break;
	}

    delete pPlatformIDs;
    
	if(!g_kernelItemList.size())
	{
		DX_CS = true;
	}


	// EXTENSIONS
	// OCL D3D11 interop functions
	clGetDeviceIDsFromD3D11KHRFunc = (clGetDeviceIDsFromD3D11KHRFuncType)clGetExtensionFunctionAddressForPlatform(platformID,"clGetDeviceIDsFromD3D11KHR");
	clCreateFromD3D11BufferKHRFunc = (clCreateFromD3D11BufferKHRFuncType)clGetExtensionFunctionAddressForPlatform(platformID,"clCreateFromD3D11BufferKHR");
	clCreateFromD3D11Texture2DKHRFunc = (clCreateFromD3D11Texture2DKHRFuncType)clGetExtensionFunctionAddressForPlatform(platformID,"clCreateFromD3D11Texture2DKHR");
	clCreateFromD3D11Texture3DKHRFunc = (clCreateFromD3D11Texture3DKHRFuncType)clGetExtensionFunctionAddressForPlatform(platformID,"clCreateFromD3D11Texture3DKHR");
	clEnqueueAcquireD3D11ObjectsKHRFunc = (clEnqueueAcquireD3D11ObjectsKHRFuncType) clGetExtensionFunctionAddressForPlatform(platformID,"clEnqueueAcquireD3D11ObjectsKHR");
	clEnqueueReleaseD3D11ObjectsKHRFunc = (clEnqueueReleaseD3D11ObjectsKHRFuncType) clGetExtensionFunctionAddressForPlatform(platformID,"clEnqueueReleaseD3D11ObjectsKHR");


	char * cs = OCLFileRead( pCLFileLoc);
    const char* codeString = cs;
	
	size_t kils = g_kernelItemList.size();
	for ( UINT ki = 0; ki < kils; ki++ )
	{
						
			g_kernelItemList[ki]->program = clCreateProgramWithSource( g_kernelItemList[ki]->context, 
												 1, 
												 &codeString, 
												 NULL, 
												 &err);
            
		    if( g_kernelItemList[ki]->deviceType == gpu )
		    {
                WideCharToMultiByte(CP_ACP,0, pCLGPUBuildRule,-1,g_kernelItemList[ki]->buildRule,256,0,0);
            }else
            {
                WideCharToMultiByte(CP_ACP,0, pCLCPUBuildRule,-1,g_kernelItemList[ki]->buildRule,256,0,0);
            }

			err = clBuildProgram(g_kernelItemList[ki]->program, 1, &g_kernelItemList[ki]->deviceID, g_kernelItemList[ki]->buildRule, NULL, NULL);
			char  *build_info;
			size_t build_info_size=0;

			// get build log
			clGetProgramBuildInfo(	g_kernelItemList[ki]->program,
									g_kernelItemList[ki]->deviceID,
									CL_PROGRAM_BUILD_LOG,
									0,
									NULL,
									&build_info_size	);
				
			// print build log
			if(build_info_size>0)
			{
				build_info = new char[build_info_size];
				clGetProgramBuildInfo(	g_kernelItemList[ki]->program,
										g_kernelItemList[ki]->deviceID,
										CL_PROGRAM_BUILD_LOG,
										build_info_size,
										build_info,
										NULL	);
				printf("Device [id: %x] build log:\n%s\n", (int)g_kernelItemList[ki]->deviceID, build_info);
				delete[] build_info;  // Breakpoint Here to read compilation errors... highlight build_info .. look at err code and cross with cl.h status codes
			}
    
            WideCharToMultiByte(CP_ACP,0, pCLEntry,-1,g_kernelItemList[ki]->entryName,256,0,0);
			g_kernelItemList[ki]->kernel = clCreateKernel (g_kernelItemList[ki]->program, g_kernelItemList[ki]->entryName, &err);

		    if( g_kernelItemList[ki]->deviceType == gpu )
		    {

		    	// the recommendation is to use CL_MEM_USE_HOST_PTR on Intel GPU architectures because of advantages to L3 CacheSharing between the
		    	// CPU and GPU whereby you can avoid implicit copies of data.  In this program I did not see speed up or degradation by using it or not 
		    	// using it.  search also for D3D11_RESOURCE_MISC_SHARED for a needed optimization.
		    	m_cl_gpu_CABuffer0 = clCreateFromD3D11BufferKHRFunc(	g_kernelItemList[ki]->context,
		    														CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR,//CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		    														m_pCABuffer0,
		    														&err);
		    	
		    	m_cl_gpu_CABuffer1 = clCreateFromD3D11BufferKHRFunc(	g_kernelItemList[ki]->context,
		    														CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR,//CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_P TR,
		    														m_pCABuffer1,
		    														&err);
                m_cl_gpu_dimensions = clCreateBuffer( g_kernelItemList[ki]->context, CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR, sizeof(WORK_DIMENSIONS),NULL, &err);

		    }else 
		    {
		    	m_cl_cpu_CABuffer0 = clCreateFromD3D11BufferKHRFunc(	g_kernelItemList[ki]->context,
		    														CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR,//CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		    														m_pCABuffer0,
		    														&err);
    
		    	m_cl_cpu_CABuffer1 = clCreateFromD3D11BufferKHRFunc(	g_kernelItemList[ki]->context,
		    														CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR,//CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		    														m_pCABuffer1,
		    														&err);
                m_cl_cpu_dimensions = clCreateBuffer( g_kernelItemList[ki]->context, CL_MEM_READ_WRITE || CL_MEM_USE_HOST_PTR, sizeof(WORK_DIMENSIONS),NULL, &err);
		    }

            
	}

    free((void*) cs);
}

void UpdateComputeShaderCB()
{
    g_CSInfo.windowX = g_WindowWidth;
    g_CSInfo.windowY = g_WindowHeight;

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    g_pd3dImmediateContext->Map( cscbi->pBff, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

    // copy the color info data to the constant buffer
    memcpy( MappedResource.pData, &g_CSInfo, sizeof( g_CSInfo ) );

    
    g_pd3dImmediateContext->Unmap( cscbi->pBff, 0 );


}

LONG	WINAPI	CarriageRun(LPVOID llpparm)
{ 
// extra thread not used in this sample
	g_sbCarriageThreadRunning = TRUE;

    
	while(g_lvContinueRunningQ ) 
    {

       // WaitForSingleObject(EvtGPUComplete, INFINITE);

        //SetEvent(EvtGPUComplete);

   	}
    
	g_sbCarriageThreadRunning = FALSE;

    return 0;
}


LONG	WINAPI	OCLHarness()
{

	g_sbOCLHarnessThreadRunning = TRUE;

    // This version of the harness takes the last created gpu/cpu device and kernal to run.
    // This is a place holder until I get other resource managers implemented and integrated
    // For IVY there is only one gpu and one cpu device so by default it behaves as expected
    // future versions will have not just kernal items but complete kernal graph items that are 
    // sequenced.  Also because the init function is restricted to DX11 KHR extensions for this 
    // experiment it might not run on third party hardware if the same extensions are not enabled
	size_t kils = g_kernelItemList.size();
	for (UINT nd = 0; nd < kils; nd++)
	{
		if( g_kernelItemList[nd]->deviceType == gpu )
		{
			g_cl_gpu_cmdQueue = g_kernelItemList[nd]->cmdQueue;
			g_cl_gpu_kernelCA = g_kernelItemList[nd]->kernel;;
		}

		if( g_kernelItemList[nd]->deviceType == cpu )
		{
			g_cl_cpu_cmdQueue = g_kernelItemList[nd]->cmdQueue;
			g_cl_cpu_kernelCA = g_kernelItemList[nd]->kernel;;
		}	
	}

    CHPCounter   MyHPCounter;
    CSVLogger MyLogger;
    double dTimeElapsed = 0;
    MyLogger.addTag("FrameCount",Int_Type);
    MyLogger.addTag("FrameTime",Double_Type);
    MyLogger.addTag("groupSizeX",Int_Type);
    MyLogger.addTag("groupSizeY",Int_Type);
    MyLogger.addTag("groupNumX",Int_Type);
    MyLogger.addTag("groupNumY",Int_Type);
    MyLogger.addTag("worksetSizeX",Int_Type);
    MyLogger.addTag("worksetSizeY",Int_Type);


			
			

	cl_int err;
	while( g_lvContinueRunningQ > 0 )
	{ 
         static int FrameCounter = 0;
 
         MyHPCounter.StartCounter();

      
        //toggle to Direct Compute path 
        if(GetAsyncKeyState('D') & 0x8000)	    
        {
            DX_CS = true;
            OCL_GPU = false;
            OCL_CPU = false;
        }
        //toggle to OCL GPU path : Default at startup
        if(GetAsyncKeyState('G') & 0x8000)
        {
            DX_CS   = false;
            OCL_GPU = true;
            OCL_CPU = false;
        }
        //toggle to OCL CPU path
        if(GetAsyncKeyState('C') & 0x8000)
        {
            DX_CS = false;
            OCL_GPU = false;
            OCL_CPU = true;
       }

        static int count = 0;
		if (count > 125)

		{
            srand((UINT)time( NULL ));
			theTime = g_CSInfo.time = (UINT) rand() * 1000000000;
			//theTime = (UINT)time( NULL );
			resetImage = g_CSInfo.reset = 1;
        
			if(count > 127)
			{
				resetImage = g_CSInfo.reset = 0;
				count = 0;
			}
			
		}
        
        //Sleep(400);  //If you want to see the athestics of the Cellular Automata progression
        if(g_counterReset)
        {
            count++;
        }

        if( DX_CS)  
        {
            UpdateComputeShaderCB();
            g_pd3dImmediateContext->CSSetConstantBuffers(0,1,&cscbi->pBff);
            g_pd3dImmediateContext->CSSetShader( m_pComputeShader, NULL, 0 );
            g_pd3dImmediateContext->CSSetShaderResources( 0, 1, &m_pCABuffer0SRV );
            UINT UAVCounts = 0;          
            g_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &m_pCABuffer1UAV, &UAVCounts );
            UINT ThreadsX = g_GroupSizeX;
            UINT ThreadsY = g_GroupSizeY;
            g_DCGroupsX = g_DCGroupsX;
            g_DCGroupsY = g_DCGroupsY;
            //UINT ThreadsX = 2;
            //UINT ThreadsY = 256;
            //UINT GroupsX = 683;
            //UINT GroupsY = 3;

            g_pd3dImmediateContext->Dispatch( g_DCGroupsX, g_DCGroupsY, 1 );
            //g_pd3dImmediateContext->Dispatch( 10, 300, 1 );
            ID3D11UnorderedAccessView* UAViewsNULL[ 1 ] = { NULL };
            g_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, UAViewsNULL, NULL );
            ID3D11ShaderResourceView* SRViewsNULL[ 1 ] = { NULL };
            g_pd3dImmediateContext->CSSetShaderResources( 0, 1, SRViewsNULL );
            MyLogger.addFieldValue("groupSizeX",  &ThreadsX  );
            MyLogger.addFieldValue("groupSizeY",  &ThreadsY  );
            MyLogger.addFieldValue("groupNumX",   &g_DCGroupsX   );
            MyLogger.addFieldValue("groupNumY",   &g_DCGroupsY   );
            MyLogger.addFieldValue("worksetSizeX",&g_WindowWidth);
            MyLogger.addFieldValue("worksetSizeY",&g_WindowHeight);
        

        }else if(OCL_GPU)
        {
            clEnqueueAcquireD3D11ObjectsKHRFunc( g_cl_gpu_cmdQueue, 1, &m_cl_gpu_CABuffer0, 0, NULL, NULL );
		    clEnqueueAcquireD3D11ObjectsKHRFunc( g_cl_gpu_cmdQueue, 1, &m_cl_gpu_CABuffer1, 0, NULL, NULL );
		    clSetKernelArg(	g_cl_gpu_kernelCA, 0, sizeof(cl_mem),	(void *) &m_cl_gpu_CABuffer0 );
		    clSetKernelArg(	g_cl_gpu_kernelCA, 1, sizeof(cl_mem),	(void *) &m_cl_gpu_CABuffer1 );
            clSetKernelArg( g_cl_gpu_kernelCA, 2, sizeof(cl_uint),   &theTime );
            clSetKernelArg( g_cl_gpu_kernelCA, 3, sizeof(cl_uint),   &resetImage );
		    clSetKernelArg(	g_cl_gpu_kernelCA, 4, sizeof(cl_mem),	(void *) &m_cl_gpu_dimensions );
		    const size_t globalWorkSize[2] = { g_WindowWidth, g_WindowHeight };
            const size_t localWorkSize[2] = { g_GroupSizeX, g_GroupSizeY };
		    cl_event ev;                                 
		    err = clEnqueueNDRangeKernel(	g_cl_gpu_cmdQueue,				// valid device command queue
								    g_cl_gpu_kernelCA,				// compiled kernel
								    2,					// work dimensions
								    NULL,				// global work offset
								    globalWorkSize,		// global work size
			                        //localWorkSize,				// local work size
			                        NULL,				// local work size/*localWorkSize*/
								    0,					// number of events to wait for
								    NULL,				// list of events to wait for
								    &ev);				// this event
		    clWaitForEvents(1, &ev);

		    clEnqueueReleaseD3D11ObjectsKHRFunc( g_cl_gpu_cmdQueue, 1, &m_cl_gpu_CABuffer0, 0, NULL, NULL );
		    clEnqueueReleaseD3D11ObjectsKHRFunc( g_cl_gpu_cmdQueue, 1, &m_cl_gpu_CABuffer1, 0, NULL, NULL );
            clEnqueueReadBuffer(g_cl_gpu_cmdQueue, m_cl_gpu_dimensions, CL_TRUE, 0, sizeof(WORK_DIMENSIONS), myDim, 0, NULL, NULL);
            MyLogger.addFieldValue("groupSizeX",  &myDim->groupSizeX  );
            MyLogger.addFieldValue("groupSizeY",  &myDim->groupSizeY  );
            MyLogger.addFieldValue("groupNumX",   &myDim->groupNumX   );
            MyLogger.addFieldValue("groupNumY",   &myDim->groupNumY   );
            MyLogger.addFieldValue("worksetSizeX",&myDim->worksetSizeX);
            MyLogger.addFieldValue("worksetSizeY",&myDim->worksetSizeY);


        }else if( OCL_CPU )
        {
            clEnqueueAcquireD3D11ObjectsKHRFunc( g_cl_cpu_cmdQueue, 1, &m_cl_cpu_CABuffer0, 0, NULL, NULL );
		    clEnqueueAcquireD3D11ObjectsKHRFunc( g_cl_cpu_cmdQueue, 1, &m_cl_cpu_CABuffer1, 0, NULL, NULL );
		    clSetKernelArg(	g_cl_cpu_kernelCA, 0, sizeof(cl_mem),	(void *) &m_cl_cpu_CABuffer0 );
		    clSetKernelArg(	g_cl_cpu_kernelCA, 1, sizeof(cl_mem),	(void *) &m_cl_cpu_CABuffer1 );
            clSetKernelArg( g_cl_cpu_kernelCA, 2, sizeof(cl_uint),   &theTime );
            clSetKernelArg( g_cl_cpu_kernelCA, 3, sizeof(cl_uint),   &resetImage );
 		    clSetKernelArg(	g_cl_cpu_kernelCA, 4, sizeof(cl_mem),	(void *) &m_cl_cpu_dimensions );
		    const size_t globalWorkSize[2] = { g_WindowWidth, g_WindowHeight };
            const size_t localWorkSize[1] = { 32 };
		    cl_event ev;
		    err = clEnqueueNDRangeKernel(	g_cl_cpu_cmdQueue,				// valid device command queue
								    g_cl_cpu_kernelCA,				// compiled kernel
								    2,					// work dimensions
								    NULL,				// global work offset
								    globalWorkSize,		// global work size
                                    // Optimization note.  For Intel GPU's local work size is left Null and optimal
                                    // work group size is automagically computed for you.
			      /*localWorkSize*/ NULL,				// local work size 
								    0,					// number of events to wait for
								    NULL,				// list of events to wait for
								    &ev);				// this event
		    clWaitForEvents(1, &ev);
            clEnqueueReadBuffer(g_cl_cpu_cmdQueue, m_cl_cpu_dimensions, CL_TRUE, 0, sizeof(WORK_DIMENSIONS), myDim, 0, NULL, NULL);
		    clEnqueueReleaseD3D11ObjectsKHRFunc( g_cl_cpu_cmdQueue, 1, &m_cl_cpu_CABuffer0, 0, NULL, NULL );
		    clEnqueueReleaseD3D11ObjectsKHRFunc( g_cl_cpu_cmdQueue, 1, &m_cl_cpu_CABuffer1, 0, NULL, NULL );
            MyLogger.addFieldValue("groupSizeX",  &myDim->groupSizeX  );
            MyLogger.addFieldValue("groupSizeY",  &myDim->groupSizeY  );
            MyLogger.addFieldValue("groupNumX",   &myDim->groupNumX   );
            MyLogger.addFieldValue("groupNumY",   &myDim->groupNumY   );
            MyLogger.addFieldValue("worksetSizeX",&myDim->worksetSizeX);
            MyLogger.addFieldValue("worksetSizeY",&myDim->worksetSizeY);
        }

        

   
        
		g_pd3dImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );
        float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        g_pd3dImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
		g_pd3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		g_pd3dImmediateContext->RSSetState(rsi->rasterState );
		g_pd3dImmediateContext->OMSetBlendState( bsi->blendState, 0, 0xffffffff );

		UINT stride = 8;
		UINT offset = 0;
		g_pd3dImmediateContext->IASetInputLayout( m_pVertexLayout );
		g_pd3dImmediateContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

		g_pd3dImmediateContext->VSSetShader( m_pVertexShader, NULL, 0 );
		g_pd3dImmediateContext->GSSetShader( NULL, NULL, 0 );	
		g_pd3dImmediateContext->PSSetShader(m_pPixelShader, NULL, 0 );
		g_pd3dImmediateContext->PSSetShaderResources(0, 1, &m_pCABuffer1SRV);
		g_pd3dImmediateContext->PSSetConstantBuffers( 0, 1, &pscbi->pBff);
		g_pd3dImmediateContext->PSSetConstantBuffers( 1, 1, &pscbi2->pBff);


		g_pd3dImmediateContext->Draw( 4, 0 );

        
        
        ID3D11ShaderResourceView* aSRViewsNULL[ 1 ] = { NULL };
        g_pd3dImmediateContext->PSSetShaderResources( 0, 1, aSRViewsNULL );

        g_pSwapChain->Present(0,0);
         
		
		swap(m_cl_gpu_CABuffer0, m_cl_gpu_CABuffer1);
		swap(m_cl_cpu_CABuffer0, m_cl_cpu_CABuffer1);
 		swap(m_pCABuffer0SRV, m_pCABuffer1SRV);
        swap( m_pCABuffer0UAV, m_pCABuffer1UAV );
        
        FrameCounter++;
        MyHPCounter.StopCounter();
        dTimeElapsed = MyHPCounter.GetLastTimeInterval();
        MyLogger.addFieldValue("FrameCount",&FrameCounter);
        MyLogger.addFieldValue("FrameTime",&dTimeElapsed);


		
	}
	
    char* pOutputLoc = new char[1024];
    size_t csvStrLen;
    wcstombs_s(&csvStrLen, pOutputLoc, 1024, pCsvFileLoc, 1024);

    MyLogger.dumpData(pOutputLoc);
    
	delete pOutputLoc;
	g_sbOCLHarnessThreadRunning = FALSE;

	return	0;

}


void  CleanupDevice()
{
    size_t kils = g_kernelItemList.size();
    for( UINT loop = 0; loop < kils; loop++ )
    {
        delete g_kernelItemList[loop];
    }


    delete rsi;
    delete bsi;
    delete pscbi;
    delete pscbi2;
    delete cscbi;
    delete ssi;  

    delete m_pCAStateBuffer;

    delete pCSFileLoc      ;
    delete pPSFileLoc      ;
    delete pVSFileLoc      ;
    delete pCLFileLoc      ;
    delete pCsvFileLoc     ;
    delete pCLGPUBuildRule ;
    delete pCLCPUBuildRule ;
    delete pCLEntry        ;
    delete myDim           ;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    //_crtBreakAlloc = 229;
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    LoadIniFile(lpCmdLine);

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;


	InitDXDevice();
    //if(!DX_CS)  // I was curious if you can have OpenCL and Direct Compute paths and contexts coexisting.
    //{            // It seems to work just fine with no noted side effects
	  OCLHarness_Init();
    //}
	//  Create OCLHarness() main entry thread here:
	DWORD	dwOCLHarnessThreadID = 0;
	g_hOCLHarnessThreadHandle	=	
		CreateThread( NULL, 0,(LPTHREAD_START_ROUTINE)OCLHarness, NULL, 0, &dwOCLHarnessThreadID );
	//  Create CarrigeRun() extra thread here:
	//DWORD	dwCarriageThreadID = 0;
	//g_hCarriageThreadHandle	=	
		//CreateThread( NULL, 0,(LPTHREAD_START_ROUTINE)CarriageRun, NULL, 0, &dwCarriageThreadID );


    // Main message loop
    MSG msg = {0};

    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

    }
	// Window received a quit message - terminate the PXC thread:
	LONG	volatile lvMyDestVal = 0;
	LONG	lvOldValue = g_lvContinueRunningQ;

	while( lvOldValue != InterlockedCompareExchange(  &g_lvContinueRunningQ, lvMyDestVal, (LONG)1 ) )
		;
	
	//  Let the Entry and helper Thread() close on its own so that it can cleanup and gracefully exit
	//  then close the thread
	while( g_sbOCLHarnessThreadRunning );
	while( g_sbCarriageThreadRunning );
		
	CloseHandle( g_hOCLHarnessThreadHandle );
	CloseHandle( g_hCarriageThreadHandle );

	//Clean up and exit
    CleanupDevice();

	return (int) msg.wParam;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
 	case WM_KEYDOWN:						
		{
            if (wParam == VK_ESCAPE) //If the escape key was pressed
            {
			    PostQuitMessage(0);					
            }
            break;

		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
