



float2 mccool_rand(float2 ij)
{
	const float pi = 3.14159265358979323846f;
	const float4 a = (float4) ( pi*pi*pi*pi, exp(4.0f), pow(13.0f, pi/2.0f), sqrt(1997.0f) );
	
	float flr[1];
	float4 result = (float4)(ij,ij);
	for(int i = 0; i < 3; i++ )
	{	
		result.x = fract(dot(result, a), flr);
		result.y = fract(dot(result, a), flr);
		result.z = fract(dot(result, a), flr);
		result.w = fract(dot(result, a), flr);
	}

	return ( result.xy*256.0f);

}

//
// 
//
//
//
//     
///    //////////////////24x24//////////////////////////
//                                                      _____    
//     o t x x x x x x x x x x x x x x x x x x x x x s       |   buffIdx[0][0]=buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1)
//     v w x x x x x x x x x x x x x x x x x x x x x u	     |   p = buffIdx + ((Stride * -(Height-1)) * ( h-1 )) + ( Stride + s-1)
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |   
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |   
//     x x x x x x x a b c x x x x x x x x x x x x x x 	     |   
//     x x x x x x x d o e x x x x x x x x x x x x x x 	     |   buffIdx[8][5]==buffIdx[5*Stride + 8]        
//     x x x x x x x f g h x x x x x x x x x x x x x x 	     |   a= buffIdx + (Stride * (h-1)) + (s-1) = 103
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |	 buffIdx = 128
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |	 
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |   
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |----Height
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     x x x x x x x x x x x x x x x x x x x x x x x x 	     |
//     q r x x x x x x x x x x x x x x x x x x x x x p	     |
//     |											  |-------
//     |______________________________________________|
//                           |
//                         Stride
//
typedef struct 
{
    int state;
}CA_STATE;

typedef struct
{
    uint groupSizeX;
    uint groupSizeY;
    uint groupNumX;
    uint groupNumY;
    uint worksetSizeX;
    uint worksetSizeY;

}DIMENSIONS;

__kernel 
void CellularAutomataHS( 
	__global CA_STATE* caBuffOld,
	__global CA_STATE* caBuffNew,
	uint time,
	uint reset
	,__global DIMENSIONS* dim
	)
{

	



	uint buffIdx;
	int strideLComp, strideRComp, heightTComp, heightBComp; 
	int buffX = get_global_id(0);
	int buffY = get_global_id(1);

	int Stride = get_global_size(0);
	int Height = get_global_size(1);

	buffIdx = buffY * Stride + buffX;
	// only need to collect this info to return one time so no need to 
	// access it for every work item
	if( buffX == 0 && buffY==0 )
	{
		dim->groupSizeX	 =  get_local_size(0);
		dim->groupSizeY	 =  get_local_size(1);
		dim->groupNumX	 =  get_num_groups(0);
		dim->groupNumY	 =  get_num_groups(1);
		dim->worksetSizeX = get_global_size(0);
		dim->worksetSizeY = get_global_size(1);
	}


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
 
	uint hComp[3] = { heightTComp, 1, heightBComp };
	uint sComp[3] = { strideLComp, 0, strideRComp };

	uint maskDimX = 3;
	uint maskDimY = 3;
	uint mask[3][3] =
	{
		{ 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 }
	};

	int product = 1;
	if( reset > 0)
	{
		float2 workItemT = (float2)( buffX, buffY );
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
			
					if( caBuffOld[ buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1 ) ].state != 0 ) 
					{
						product *= caBuffOld[ buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1) ].state;
						//product = caBuffOld[ buffIdx + ((Stride * hComp[h]) * ( h-1 )) + ( sComp[s] + s-1) ];
					}
				}
			}
		}

//		unwrapped manually to see if there was an increase in performance.  the increase was negligable and not worth the hit on
//		kernel readability but I left it here
//		for future experimentation	
//		//if( mask[0][0] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideLComp + 0-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideLComp + 0-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideLComp + 0-1) ];
//			}
//		//}
//		//if( mask[0][1] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideLComp + 0-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideLComp + 0-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideLComp + 0-1) ];
//			}
//		//}
//		//if( mask[0][2] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideLComp + 0-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideLComp + 0-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideLComp + 0-1) ];
//			}
//		//}
//		//if( mask[1][0] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( 0 + 1-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( 0 + 1-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( 0 + 1-1) ];
//			}
//		//}
//		//if( mask[1][1] != 0 )
//		//{
//		//
//		//	if( caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( 0 + 1-1 ) ].state != 0 ) 
//		//	{
//		//		product *= caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( 0 + 1-1) ].state;
//		//		//product = caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( 0 + 1-1) ];
//		//	}
//		//}
//		//if( mask[1][2] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( 0 + 1-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( 0 + 1-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( 0 + 1-1) ];
//			}
//		//}
//		//if( mask[2][0] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideRComp + 2-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideRComp + 2-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightTComp) * ( 0-1 )) + ( strideRComp + 2-1) ];
//			}
//		//}
//		//if( mask[2][1] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideRComp + 2-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideRComp + 2-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * 1) * ( 1-1 )) + ( strideRComp + 2-1) ];
//			}
//		//}
//		//if( mask[2][2] != 0 )
//		//{
//		
//			if( caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideRComp + 2-1 ) ].state != 0 ) 
//			{
//				product *= caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideRComp + 2-1) ].state;
//				//product = caBuffOld[ buffIdx + ((Stride * heightBComp) * ( 2-1 )) + ( strideRComp + 2-1) ];
//			}
//		//}

	}

	int nextState = product;
	nextState++;
	nextState %=16;

	caBuffNew[buffIdx].state = nextState;
	
 }