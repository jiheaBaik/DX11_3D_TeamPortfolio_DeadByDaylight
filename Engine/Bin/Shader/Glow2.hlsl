Texture2D g_InputTexture;
Texture2D g_InputDepthTexture;
RWTexture2D<float4> g_OutputTexture;

float2 g_fWinSize;
float3 g_vCameraPos;
float g_fTime;
matrix g_ViewMatrix;


const float MIN_DIST = 0.f;
const float MAX_DIST = 100.f;
const int MAX_STEPS = 64;
const float EPSILON = 1e-6;

//-------------------------------------------------------------------------------------------------
float3 rayDirection(float fieldOfView, float2 fragCoord) {
    float2 xy = fragCoord - g_fWinSize.xy / 2.f;
    float z = (0.5f * g_fWinSize.y) / tan(radians(fieldOfView) / 2.f);
    return normalize(float3(xy, -z));
}
float3x3 lookAt(in float3 _camera, in float3 _targetDir, in float3 _up) {
    float3 zaxis = normalize(_targetDir);
    float3 xaxis = normalize(cross(zaxis, _up));
    float3 yaxis = cross(xaxis, zaxis);
    return float3x3(xaxis, yaxis, -zaxis);
}
float3 rotate(float3 p, float4 q) {
    return 2.f * cross(q.xyz, p * q.w + cross(q.xyz, p)) + p;
}
float torusSDF(float3 p, float2 t) {
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}
float getSDF(float3 position) {
    float angle = g_fTime;
    float3 axis = normalize(float3(1.f, 1.f, 1.f));
    position = rotate(position, float4(axis * sin(-angle * 0.5f), cos(-angle * 0.5f)));
    return torusSDF(position, float2(1.f, 0.2f));
}
float getGlow(float dist, float radius, float intensity) {
    return pow(radius / max(dist, 1e-6), intensity);
}
float distanceToScene(float3 cameraPos, float3 rayDir, float start, float end, inout float glow, uint2 vPixel) {
    float depth = start;
    float dist;
    for (int i = 0; i < MAX_STEPS; i++) {
        dist = getSDF(g_InputDepthTexture.Load(int3(vPixel, 0)).x * 500.f);
        glow += getGlow(dist, 1e-3, 0.55);
        if (dist < EPSILON) {
            return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}
float3 ACESFilm(float3 x) {
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

[numthreads(32, 32, 1)]
void CS_GLOW2(uint3 dispatchThreadID : SV_DispatchThreadID) {
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    float3 rayDir = rayDirection(60.f, vPixel);

    float3 cameraPos = g_vCameraPos;
    float3 target = -normalize(cameraPos);
    float3 up = float3(0.f, 1.f, 0.f);
    float4x4 viewMatrix = g_ViewMatrix;

    rayDir = mul(viewMatrix, rayDir);

    float glow = 0.f;
    float dist = distanceToScene(cameraPos, rayDir, MIN_DIST, MAX_DIST, glow, vPixel);

    float3 glowColour = float3(0.2, 0.5, 1.0);
    float3 col = glow * glowColour;
    col = ACESFilm(col);
    col = pow(col, float3(0.4545, 0.4545, 0.4545));

    g_OutputTexture[vPixel] = vector(col, 1.f);
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Glow2 {
    pass Pass_Glow2 {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_GLOW2();
    }
}










