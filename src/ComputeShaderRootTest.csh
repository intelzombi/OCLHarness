


#define THREADS_X 2        
#define THREADS_Y 256        
#define pi 3.14159265358979323846


cbuffer CB0
{
	uint time;
	uint reset;
	uint WindowX;
	uint WindowY;
};


struct CABufType
{
    int state;
};
StructuredBuffer<CABufType> caBuffOld;
RWStructuredBuffer<CABufType> caBuffNew;



float2 mccool_rand(float2 ij)
{
	const float4 a = float4( pi*pi*pi*pi, exp(4.0f), pow(13.0, pi/2.0f), sqrt(1997.0) );
	float4 result = float4(ij,ij);

	for(int i = 0; i < 3; i++ )
	{
		result.x = frac(dot(result, a));
		result.y = frac(dot(result, a));
		result.z = frac(dot(result, a));
		result.w = frac(dot(result, a));
	}

	return ( result.xy*256.0f);

}

//--------------------------------------------------------------------------------------
// CACS
// main entry point
//--------------------------------------------------------------------------------------

// run each thread group with dimensions THREADS_X x THREADS_Y
[numthreads(THREADS_X,THREADS_Y,1)]
void CellularAutomataHS( uint3 Gid : SV_GroupID, uint3 get_global_id : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{

	int maskDimX = 3;
	int maskDimY = 3;
	int mask[3][3] =
	{
		//{ 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 }
		// This is a scolling test.  You can adjust the bit mask to cause
		// scrolling of the buffer.  This is useful for testing the boundry 
		// edges of the buffer along with the thread and group dimensions
		// try different thread and group dimensions and you can discover
		// incompatible group / thread dimensions that show up on the boundries
		// when scrolling
		{ 0, 0, 1 }, { 0, 0, 0 }, { 0, 0, 0 }
	};
	
	int buffIdx;
	int strideLComp, strideRComp, heightTComp, heightBComp; 
	int buffX = get_global_id.x;
	int buffY = get_global_id.y;

	int Stride = WindowX;
	int Height = WindowY;

	int span = Stride * Height;

	buffIdx = buffY * Stride + buffX;

	if( buffY == 0 )
	{
	    heightTComp = -(Height-1);
	}else
	{
	    heightTComp = 1; 
	}
	if( buffY == (Height -1) )
	{
	    heightBComp = -(Height-1) ;
	}else
	{
	    heightBComp = 1;
	}
	if( buffX == 0 )
	{
	    strideLComp = Stride;
	}else
	{
	    strideLComp = 0;
	}
	if( buffX == Stride-1 )
	{
	    strideRComp = -Stride;
	}else
	{
	    strideRComp = 0;
	}
 
	int hComp[3] = { heightTComp, 1, heightBComp };
	int sComp[3] = { strideLComp, 0, strideRComp };

	int product = 1;
	if( reset > 0)
	{
		float2 workItemT = float2( buffX, buffY );
		workItemT /= time*0.2f;
		float2 randRet = mccool_rand(workItemT);
		product = floor(randRet.x*randRet.y );
		product %= 16;
	
	}else{
		for ( int h = 0; h < maskDimY; h++)
		{
			for (int s = 0; s < maskDimX; s++)
			{
	
				if( mask[s][h] != 0 )
				{
			
					//if( caBuffOld[ buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1 ) ].state != 0 ) 
					//{
						//product *= caBuffOld[ buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1) ].state;
						product *= caBuffOld[ (buffIdx) + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1) ].state;
					//}
				}
			}
		}
	}

	int nextState = product;
	//nextState++;
	//nextState %=16;

	caBuffNew[buffIdx].state = nextState;

}