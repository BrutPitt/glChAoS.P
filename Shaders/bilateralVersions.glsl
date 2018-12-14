float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma));
}

float normpdf3(in vec3 v, in float sigma)
{
	return 0.39894*exp(-0.5*dot(v,v)/(sigma*sigma));
}


vec4 bilateralSmartSmoothA()
{
    const float bsigma = threshold;
	const vec4 c = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);
		
	//declare stuff
	const float radius = 3.0*sigma-1.f;
//	float kernel[120];
	vec4 final_colour = vec4(0.0);

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
	//create the 1-D kernel
	float Z = 0.0;
	
/*
    int rad = int(radius);
    for (int i = 0; i <= rad; ++i)
        kernel[rad+i] = kernel[rad-i] = exp( -(i*i) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
*/

	vec4 cc;
	float factor;
	float bZ = 1.0/normpdf(1.0, bsigma);
	//read out the texels
	for (float i=-radius; i <= radius; i++)	{
		for (float j=-radius; j <= radius; j++) {
            
			cc =  texelFetch(origTexture, ivec2(gl_FragCoord.x+i,gl_FragCoord.y+j),0 );
            float fJ = exp( -(j*j) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
            float fI = exp( -(i*i) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
			factor = normpdf3(cc.rgb-c.rgb, bsigma)*fJ*fI;
			Z += factor;
			final_colour += factor*cc;

		}
	}
	
	return final_colour/Z;

}


vec4 bilateralSmartSmoothOK()
{
    const float bsigma = threshold;
	const float radius = float(round(3.0*sigma-1.f));
    const float radQ = radius * radius;

    const float invSigma = 1.f/sigma;
	const float invSigmaQx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
    const float invSigmaQx2PI = M_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
//	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(2*PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);

	float Zbuff = 0.0;
	vec4 accumBuff = vec4(0.0);

    vec2 d;
	for (d.x=-radius; d.x <= radius; d.x++)	{
        const float pt = sqrt(radQ-d.x*d.x);
		for (d.y=-pt; d.y <= pt; d.y++) {
            const float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2;

			const vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 );
            const vec4 dC = walkPx-centrPx;
			const float deltaFactor = exp( -dot(dC, dC) * invBSigmaSqx2) * invBSigmaxSqrt2PI * blurFactor; 
                                 
			Zbuff     += deltaFactor;
			accumBuff += deltaFactor*walkPx;
		}
	}
	return accumBuff/Zbuff;
}

vec4 bilateralSmartSmoothTex()
{
    const vec2 pxTex = invScrnSize;
    const float bsigma = threshold;
	const vec2 radius = invScrnSize* float(round(3.0*sigma-1.f));
    const float radQ = radius.y * radius.y;

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texture(origTexture,vTexCoord);

	vec4 Zbuff = vec4(0.0);
	vec4 accumBuff = vec4(0.0);

    vec2 d = vec2(0.0, radius.y);

    for(; d.y >= 0.0; d.y-=pxTex.y) {
			const vec4 walkPx1 =  texture(origTexture, vTexCoord+d);
            const vec4 walkPx2 =  texture(origTexture, vTexCoord-d);

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;

            const vec2 exponent = vec2(dot(dC1, dC1), dot(dC2, dC2)) * -invBSigmaSqx2;

            const float fXY = exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI ; 
			const vec2 factor = exp( exponent ) * invBSigmaxSqrt2PI * fXY ;
			Zbuff.xy += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2;
    }

	for(d.x=radius.x; d.x > 0.0; d.x-=pxTex.x)	{
		for(d.y=sqrt(radQ-d.x*d.x); d.y > 0.0; d.y-=pxTex.y) {
			const vec4 walkPx1 =  texture(origTexture, vTexCoord+d);
            const vec4 walkPx2 =  texture(origTexture, vec2(vTexCoord.x-d.x, vTexCoord.y+d.y));
            const vec4 walkPx3 =  texture(origTexture, vec2(vTexCoord.x+d.x, vTexCoord.y-d.y));
            const vec4 walkPx4 =  texture(origTexture, vTexCoord-d);

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;
            const vec4 dC3 = walkPx3-centrPx;
            const vec4 dC4 = walkPx4-centrPx;

            const vec4 exponent = vec4(dot(dC1, dC1), dot(dC2, dC2), dot(dC3, dC3), dot(dC4, dC4)) * -invBSigmaSqx2;

			const vec4 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff     += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2 + factor.z*walkPx3 + factor.w*walkPx4;
		}
	}

    //pixel [0,0] -> exponent are 0 -> factor = fXY = 1.0
	accumBuff += centrPx;

	return accumBuff/(Zbuff.x+Zbuff.y+Zbuff.z+Zbuff.w+1.0);
 }

vec4 bilateralSmartSmoothTex()
{
    const vec2 pxTex = invScrnSize;
    const float bsigma = threshold;
	const float radius = float(round(3.0*sigma-1.f));
    const float radQ = radius * radius;

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texture(origTexture,gl_FragCoord.xy*pxTex);

	vec4 Zbuff = vec4(0.0);
	vec4 accumBuff = vec4(0.0);

    vec2 d = vec2(0.0, radius);

    for(; d.y > 0.0; d.y--) {
			const vec4 walkPx1 =  texture(origTexture, (gl_FragCoord.xy+d)*pxTex);
            const vec4 walkPx2 =  texture(origTexture, (gl_FragCoord.xy-d)*pxTex);

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;

            const vec2 exponent = vec2(dot(dC1, dC1), dot(dC2, dC2)) * -invBSigmaSqx2;

            const float fXY = exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI ; 
			const vec2 factor = exp( exponent ) * invBSigmaxSqrt2PI * fXY ;
			Zbuff.xy += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2;
    }

	for(d.x=radius.x; d.x > 0.0; d.x--)	{
		for(d.y=sqrt(radQ-d.x*d.x); d.y > 0.0; d.y--) {
			const vec4 walkPx1 =  texture(origTexture, (gl_FragCoord.xy+d)*pxTex);
            const vec4 walkPx2 =  texture(origTexture, (gl_FragCoord.xy-d)*pxTex);
            const vec4 walkPx3 =  texture(origTexture, vec2(gl_FragCoord.x-d.y, gl_FragCoord.y+d.x)*pxTex);
            const vec4 walkPx4 =  texture(origTexture, vec2(gl_FragCoord.x+d.y, gl_FragCoord.y-d.x)*pxTex);

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;
            const vec4 dC3 = walkPx3-centrPx;
            const vec4 dC4 = walkPx4-centrPx;

            const vec4 exponent = vec4(dot(dC1, dC1), dot(dC2, dC2), dot(dC3, dC3), dot(dC4, dC4)) * -invBSigmaSqx2;

			const vec4 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff     += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2 + factor.z*walkPx3 + factor.w*walkPx4;
		}
	}

    //pixel [0,0] -> exponent are 0 -> factor = fXY = 1.0
	accumBuff += centrPx;

	return accumBuff/(Zbuff.x+Zbuff.y+Zbuff.z+Zbuff.w+1.0);
 }

vec4 bilateralSmartSmoothOK()
{
    const vec2 pxTex = invScrnSize;
    const float bsigma = threshold;
	const float radius = float(round(3.0*sigma-1.f));
    const float radQ = radius * radius;

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texture(origTexture,gl_FragCoord.xy*pxTex);

    //pixel [0,0] -> exponent are 0 -> factor = fXY = 1.0
	vec4 Zbuff = vec4(1.0, vec3(0.0));
	vec4 accumBuff = centrPx;

    vec2 d;
	for(d.x=radius.x; d.x > 0.0; d.x--)	{
		for(d.y=sqrt(radQ-d.x*d.x); d.y >= 0.0; d.y--) {
			const vec4 walkPx1 =  texture(origTexture, (gl_FragCoord.xy+d)*pxTex);
            const vec4 walkPx2 =  texture(origTexture, (gl_FragCoord.xy-d)*pxTex);
            const vec4 walkPx3 =  texture(origTexture, vec2(gl_FragCoord.x-d.y, gl_FragCoord.y+d.x)*pxTex);
            const vec4 walkPx4 =  texture(origTexture, vec2(gl_FragCoord.x+d.y, gl_FragCoord.y-d.x)*pxTex);

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;
            const vec4 dC3 = walkPx3-centrPx;
            const vec4 dC4 = walkPx4-centrPx;

            const vec4 exponent = vec4(dot(dC1, dC1), dot(dC2, dC2), dot(dC3, dC3), dot(dC4, dC4)) * -invBSigmaSqx2;

			const vec4 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff     += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2 + factor.z*walkPx3 + factor.w*walkPx4;
		}
	}

	return accumBuff/(Zbuff.x+Zbuff.y+Zbuff.z+Zbuff.w);
}

vec4 bilateralSmartSmoothOKMat4()
{
    const float bsigma = threshold;
	const float radius = float(round(3.0*sigma-1.f));

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);
    const mat4 mCenter = mat4(centrPx,centrPx,centrPx,centrPx);

	vec4 Zbuff = vec4(0.0);
	vec4 accumBuff = vec4(0.0);
/*
    vec2 d = vec2(radius, 0.0);

    for(; d.x > 0.0; d.x--) {
			const mat4 m = mat4( texelFetch(origTexture, ivec2(gl_FragCoord.xy+d   ),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy-d   ),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy+d.yx),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy-d.yx),0 ) );

            const mat4 mTmp = m - mCenter;
            const vec4 exponent = vec4(1.0) * matrixCompMult(mTmp, mTmp) * -invBSigmaSqx2;

			const vec4 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff     += factor;
			accumBuff += m * factor;
    }
*/
    vec2 d = vec2(0.f, radius);

    for(; d.y > 0.f; d.y--) {
			const vec4 walkPx1 =  texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 );
            const vec4 walkPx2 =  texelFetch(origTexture, ivec2(gl_FragCoord.xy-d),0 );

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;

            const vec2 exponent = vec2(dot(dC1, dC1), dot(dC2, dC2)) * -invBSigmaSqx2;

			const vec2 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;
			Zbuff.xy += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2;
    }

	for(d.x=radius; d.x > 0.0; d.x--)	{
		for(d.y=sqrt(radius*radius-d.x*d.x); d.y > 0.0; d.y--) {
			const mat4 m = mat4( texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x-d.x, gl_FragCoord.y+d.y),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x+d.x, gl_FragCoord.y-d.y),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy-d),0 ));

            const mat4 mTmp = m - mCenter;
            const vec4 exponent = vec4(1.0) * matrixCompMult(mTmp, mTmp) * -invBSigmaSqx2;
            //const vec4 exponent = vec4(dot(mDot[0], mDot[0]), dot(mDot[1], mDot[1]), dot(mDot[2], mDot[2]), dot(mDot[3], mDot[3])) * -invBSigmaSqx2;

			const vec4 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff     += factor;
			accumBuff += m * factor;
		}
	}

    //pixel [0,0] -> exponent are 0 -> factor = fXY = 1.0
	accumBuff += centrPx;
	return accumBuff/(Zbuff.x+Zbuff.y+Zbuff.z+Zbuff.w+1.0);
}

vec4 bilateralSmartSmoothOKMat8()
{
    const float bsigma = threshold;
	const float radius = float(round((3.0*sigma)*COS_PI_4 - 1.f));

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5f * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5f * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);
    const mat4 mCenter = mat4(centrPx,centrPx,centrPx,centrPx);

    //const vec4 uno = vec4(1.0);

	vec4 Zbuff = vec4(0.f);
	vec4 accumBuff = vec4(0.f);

    vec2 d = vec2(0.f, radius);

    for(; d.y > 0.f; d.y--) {
			const vec4 walkPx1 =  texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 );
            const vec4 walkPx2 =  texelFetch(origTexture, ivec2(gl_FragCoord.xy-d),0 );

            const vec4 dC1 = walkPx1-centrPx;
            const vec4 dC2 = walkPx2-centrPx;

            const vec2 exponent = vec2(dot(dC1, dC1), dot(dC2, dC2)) * -invBSigmaSqx2;

			const vec2 factor = exp( exponent ) * invBSigmaxSqrt2PI * 
                                exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

			Zbuff.xy += factor;
			accumBuff += factor.x*walkPx1 + factor.y*walkPx2;
    }

	for(d.x=radius; d.x > 0.f; d.x--)	{
		for(d.y=sqrt(radius*radius-d.x*d.x); d.y > 0.0; d.y--) {
			const mat4 m1 = mat4(texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x-d.x, gl_FragCoord.y+d.y),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x+d.x, gl_FragCoord.y-d.y),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy-d),0 ));

			const mat4 m2 = mat4(texelFetch(origTexture, ivec2(gl_FragCoord.xy+d.yx),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x-d.y, gl_FragCoord.y+d.x),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.x+d.y, gl_FragCoord.y-d.x),0 ),
                                 texelFetch(origTexture, ivec2(gl_FragCoord.xy-d.yx),0 ));

            const mat4 mTmp1 = m1 - mCenter;
            const mat4 mTmp2 = m2 - mCenter;
            const vec4 exponent1 = vec4(1.f) * matrixCompMult(mTmp1, mTmp1) * -invBSigmaSqx2;
            const vec4 exponent2 = vec4(1.f) * matrixCompMult(mTmp2, mTmp2) * -invBSigmaSqx2;
            //const vec4 exponent = vec4(dot(mDot[0], mDot[0]), dot(mDot[1], mDot[1]), dot(mDot[2], mDot[2]), dot(mDot[3], mDot[3])) * -invBSigmaSqx2;

            const float fX = exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI * invBSigmaxSqrt2PI;
			const vec4 factor1 = exp( exponent1 ) * fX;                                 
			const vec4 factor2 = exp( exponent2 ) * fX; 

			Zbuff     += factor1 + factor2;
			accumBuff += m1 * factor1 + m2 * factor2;
		}
	}

	const vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.xy),0);

    const vec4 dC = walkPx-centrPx;

    const float exponent = dot(dC, dC) * -invBSigmaSqx2;
	const float factor   = exp( exponent ) * invBSigmaxSqrt2PI * 
                           exp( -dot(d,d) * invSigmaSqx2 ) * invSigmaxSqrt2PI;

	accumBuff += factor*walkPx;

	return accumBuff/(Zbuff.x+Zbuff.y+Zbuff.z+Zbuff.w+factor);
}


vec4 bilateralSmartSmoothOK1()
{
    const float bsigma = threshold;
	const float radius = 3.0*sigma-1.f;

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);

	float Zbuff = 0.0;
	vec4 accumBuff = vec4(0.0);

	for (float i=-radius; i <= radius; i++)	{
        float fI = exp( -(i*i) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 

		for (float j=-radius; j <= radius; j++) {            
            float fJ = exp( -(j*j) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 

			const vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.x+i,gl_FragCoord.y+j),0 );

            const vec4 dC = walkPx-centrPx;
			const float factor = exp( -dot(dC, dC) * invBSigmaSqx2 ) * invBSigmaxSqrt2PI * fJ * fI;
			Zbuff += factor;
			accumBuff += factor*walkPx;
		}
	}
	return accumBuff/Zbuff;

}

vec4 bilateralSmartSmooth(const float reductFactor)
{
    const float smallSigma = sigma * reductFactor;
    const float bsigma = threshold;
	const float radius = 3.0*smallSigma-1.f;

    const float invSigma = 1.f/smallSigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);

	vec4 accumBuff = vec4(0.0);
	float Zbuff = 0.0;

	for (float i=-radius; i <= radius; i++)	{ 
        float fI = exp( -(i*i) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 

		for (float j=-radius; j <= radius; j++) {            
            float fJ = exp( -(j*j) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 

			const vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.x+i,gl_FragCoord.y+j),0 );

            const vec4 dC = walkPx-centrPx;
			const float factor = exp( -dot(dC, dC) * invBSigmaSqx2 ) * invBSigmaxSqrt2PI * fJ * fI;
			Zbuff += factor;
			accumBuff += factor*walkPx;
		}
	}
	return accumBuff/Zbuff;
}

/*
vec4 gPassModuled(sampler2D tex, vec2 direction) 
{
	//vec2 size = vec2(1024.0, 1024.0); //vec2( textureSize2D( glowTexture, int(level)));
	vec2 offset = wSize; 
	
	//compute the radius across the kernel
	float radius = 3.0*sigma-1.f;
	
	vec4 accumBuff = vec4(0.0);
    float accumAlpha = 0.0;
	
	//precompute factors used every iteration
	float invSigmaSqx2 = 1.f/(sigma * sigma * 2.0);
	float invSigmaxSqrt2PI = 1.f/(SQRT_OF_2PI * sigma);

    float bsigma = threshold*.1;
	float invBSigmaSqx2 = 1.f/(bsigma * bsigma * 2.0);
	float invBSigmaxSqrt2PI = 1.f/(SQRT_OF_2PI * bsigma);
    float depth = 0.0;

    vec4 oCol = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);

	// separable Gaussian
    for ( float r = -radius; r <= radius; r++) {
            float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI / sigma ;
            vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);

            vec4 newC = (c-oCol)  ;
            float dotP = dot(newC, newC)  * bsigma;

            float factor2 = exp( -dotP * invBSigmaSqx2) * invBSigmaxSqrt2PI *factor * factor/ bsigma ;
            //if(factor2<=0.0) factor2 = 1.0;
            depth += factor2;
            accumBuff += factor2 * c;
            
	}

    //return  vec4((accumBuff/(depth)).rgb, 1.0);
    return  (depth <= 0.0) ? oCol : accumBuff/depth;
}
*/
/*
vec4 g4PassModuled(sampler2D tex, vec2 direction) 
{
    const float bsigma = threshold;
	const float radius = 3.0*sigma-1.f;

    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
    const float invBSigma = 1.f/bsigma;
	const float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(PI) * sigma)

	const vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);

	vec4 accumBuff = vec4(0.0);
	float Zbuff = 0.0;

	// separable Gaussian
    for ( float r = -radius; r <= radius; r++) {
            float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI ;
            vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);
            vec4 t = texelFetch(pass1Texture, ivec2(gl_FragCoord.xy + r * direction), 0);

            float factor2;
            if(pass==1) {
                depth += factor;
                accumBuff += factor * c;
            }
            else {
                vec4 newC = (c - oCol) ;            
                float dotP =  dot(newC, newC);
                float factor2 = exp( -dotP * invBSigmaSqx2) * invBSigmaxSqrt2PI * factor ;
                depth += factor2;
                accumBuff += factor2 * c;
            }
            //if(factor2<=0.0) factor2 = 1.0;
            
	}

    //return  vec4((accumBuff/(depth)).rgb, 1.0);
    vec4 retCol = (depth <= 0.0) ? oCol : accumBuff/depth;

    //if(pass == 2) retCol = (retCol + tCol) * .5;
    return  retCol;
}

vec4 gPassIntensity(sampler2D tex, vec2 direction) 
{
	//vec2 offset = wSize; 
	
	//compute the radius across the kernel
	const float radius = 3.0*sigma-1.f;
	
    float Zbuff = 0.0;
	vec4 accumBuff = vec4(0.0);
	
	//precompute factors used every iteration
    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
	
	// separable Gaussian
    for ( float r = -radius; r <= radius; r++) {
        float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI;
        vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);
	    accumBuff += factor * c;
	}
		
	return accumBuff;
}
*/

vec4 gPassModuledA(sampler2D tex, vec2 direction, int pass) 
{
	//vec2 size = vec2(1024.0, 1024.0); //vec2( textureSize2D( glowTexture, int(level)));
	vec2 offset = wSize; 
	
	//compute the radius across the kernel
	float radius = 3.0*sigma-1.f;
	
	vec4 accumBuff = vec4(0.0);
    float accumAlpha = 0.0;
	
    const float invSigma = 1.f/sigma;
	const float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
	const float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)

    float bsigma = threshold;
	float invBSigmaSqx2 = 1.f/(bsigma * bsigma * 2.0);
	float invBSigmaxSqrt2PI = 1.f/(SQRT_OF_2PI * bsigma);

    float depth = 0.0;

    vec4 oCol = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);
    vec4 tCol = texelFetch(pass1Texture,ivec2(gl_FragCoord.xy),0);

    float dotP = dot(oCol.rgb, oCol.rgb);
    if(dotP<threshold) {
        for ( float r = -radius; r <= radius; r++) {
                vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);
                float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI  / sigma;
   
                accumBuff += factor * c;
            
	    }
    } else accumBuff = oCol; 

    //if(pass == 2) retCol = (retCol + tCol) * .5;
    return  accumBuff;
}

