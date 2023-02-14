Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

float g_fHue, g_fSaturation, g_fBrightness, g_fContrast;
bool g_bShow_HSBC;
//-------------------------------------------------------------------------------------------------
float3 applyHue(float3 aColor, float aHue) {
    float angle = radians(aHue);
    float3 k = float3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(angle);
    return aColor * cosAngle + cross(k, aColor) * sin(angle) + k * dot(k, aColor) * (1 - cosAngle);
}
[numthreads(32, 32, 1)]
void CS_HSBC(uint3 dispatchThreadID : SV_DispatchThreadID) {
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    if (!g_bShow_HSBC) {
        g_OutputTexture[vPixel] = g_InputTexture.Load(int3(vPixel, 0.f));
        return;
    }

    float4 vColor = g_InputTexture.Load(int3(vPixel, 0.f));

    //float hue = 360 * hsbc.r;
    //float saturation = hsbc.g * 2;
    //float brightness = hsbc.b * 2 - 1;
    //float contrast = hsbc.a * 2;

    vColor.rgb = applyHue(vColor.rgb, g_fHue);
    vColor.rgb = (vColor.rgb - 0.5f) * g_fContrast + 0.5f;
    vColor.rgb = vColor.rgb + g_fBrightness;
    float3 intensity = dot(vColor.rgb, float3(0.299, 0.587, 0.114));
    vColor.rgb = lerp(intensity, vColor.rgb, g_fSaturation);

    //return hsbcColor * startColor.a;
    g_OutputTexture[vPixel] = vColor;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_HSBC {
    pass Pass_HSBC {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_HSBC();
    }
}