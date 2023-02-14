float3 g_vCameraPos = (float3)0.f;

float3 g_vLightPos = (float3)0.f;
float3 g_vLightDir = (float3)0.f;
float g_fLightRange = 0.f;
float4 g_vLightDiffuse = (float4)0.f;
float4 g_vLightAmbient = (float4)0.f;
float4 g_vLightSpecular = (float4)0.f;
float2 g_fLightAngle = (float2)0.f;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_WorldTexture;
texture2D g_MaterialTexture;
textureCUBE g_CubeTexture;

RWTexture2D<float4> g_ShadeTexture;
RWTexture2D<float4> g_SpecularTexture;

sampler Sampler_Default = sampler_state {
	Filter = min_mag_mip_linear;
};
//-------------------------------------------------------------------------------------------------

bool g_bOnOff;
bool g_bOnOff2;
float g_fPI;
float g_fAlbedo;

float g_fRoughness;
float g_fMetalness;
float g_f0;

static const float Fdielectric = 0.04f;

float3 OrenNayar(float3 albedo, float3 normal, float3 viewDir, float3 lightDir, float roughness) {
	float sigma2 = roughness * roughness;
	float NdotL = saturate(dot(normal, lightDir));
	float NdotV = saturate(dot(normal, viewDir));

	float lambert = albedo / g_fPI;
	float A = 1.f - (sigma2 / (sigma2 + 0.33f)) * 0.5f;
	float B = 0.45f * sigma2 / (sigma2 + 0.09f);
	float alpha = max(acos(NdotL), acos(NdotV));
	float beta = min(acos(NdotL), acos(NdotV));
	float gamma = max(0.f, dot(viewDir - normal * NdotV, lightDir - normal * NdotL));

	//return lambert * (A + B * gamma * sin(alpha) * tan(beta));
	return max(0.f, NdotL) * (A + B * gamma * sin(alpha) * tan(beta));
}

float D_GGX(float a2, float NoH) {
	float d = (NoH * a2 - NoH) * NoH + 1.f;
	return a2 / (g_fPI * d * d);
}
float D_GGXaniso(float ax, float ay, float NoH, float3 H, float3 X, float3 Y) {
	float XoH = dot(X, H);
	float YoH = dot(Y, H);
	float d = XoH * XoH / (ax * ax) + YoH * YoH / (ay * ay) + NoH * NoH;
}

float3 F_Schlick(float3 f0, float VoH) {
	return f0 + (1.f - f0) * pow(1.f - VoH, 5.f);
}

float G_Schlick_UE4(float a2, float NoV, float NoL) {
	float a = a2 * a2;
	float Vis_SmithV = NoV * (NoV * (1.f - a) + a);
	float Vis_SmithL = NoL * (NoL * (1.f - a) + a);
	return 0.5f * rcp(Vis_SmithV + Vis_SmithL);
}

[numthreads(32, 32, 1)]
void CS_DIR(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	vector vDiffuse = g_DiffuseTexture.Load(int3(vPixel, 0.f));
	vector vWorldPos = g_WorldTexture.Load(int3(vPixel, 0.f));
	vector vView = normalize(vector(g_vCameraPos, 1.f) - vWorldPos);
	vector vNormal = normalize(vector(g_NormalTexture.Load(int3(vPixel, 0.f)).xyz * 2.f - 1.f, 0.f));
	float fRoughness = g_MaterialTexture.Load(int3(vPixel, 0.f)).x;
	float fRefract = g_MaterialTexture.Load(int3(vPixel, 0.f)).y;
	float fMetalness = g_MaterialTexture.Load(int3(vPixel, 0.f)).z;
	float fMetalness2 = g_MaterialTexture.Load(int3(vPixel, 0.f)).w;
	float3 f0 = lerp(Fdielectric, vDiffuse, fRefract);
	float3 f1 = lerp(Fdielectric, vDiffuse, fMetalness);
	float3 f2 = lerp(Fdielectric, vDiffuse, fMetalness2);

	float4 vShade;
	if (g_bOnOff)
		vShade = g_vLightDiffuse * (vector(OrenNayar(g_fAlbedo, vNormal, vView, normalize(-g_vLightDir), fRoughness).xyz, 1.f) + g_vLightAmbient);
	else
		vShade = g_vLightDiffuse * (saturate(dot(normalize(vector(g_vLightDir, 0.f)) * -1.f, vNormal)) + g_vLightAmbient);
	vShade.a = 1.f;

	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	vector vReflect = normalize(reflect(normalize(vector(g_vLightDir, 0.f)), vNormal));
	float4 vSpecular;
	if (g_bOnOff2) {
		float3 V = vView;
		float3 N = vNormal;
		float3 L = normalize(-g_vLightDir);
		float3 H = normalize(L + V);

		float alpha = fRefract * fRefract;
		float NoL = saturate(dot(N, L));
		float NoV = saturate(dot(N, V));
		float NoH = saturate(dot(N, H));
		float VoH = saturate(dot(V, H));
		float LoH = saturate(dot(L, H));

		float D = D_GGX(alpha, NoH);
		float3 F = F_Schlick(f0, LoH);
		float G = G_Schlick_UE4(alpha, NoV, NoL);

		float3 Specular = D * F * G / 4.f * NoL * NoV;
		vSpecular = g_vLightSpecular * vector(Specular, Specular.r);
		float3 F1 = F_Schlick(f2, VoH);
		float3 kd = lerp(float3(1, 1, 1) - F1, float3(0, 0, 0), fMetalness2);
		float3 kd2 = lerp(float3(0, 0, 0), float3(1, 1, 1), fMetalness);
		vShade = vShade * vector(kd, 1.f);

		vector vReflect1 = normalize(reflect(vLook, vNormal));
		float4 vReflect2 = g_CubeTexture.SampleLevel(Sampler_Default, vReflect1.xyz, 0);
		vShade += vReflect2 * vector(kd2, 1.f);
	}
	else
		vSpecular = vector(0.f,0.f,0.f,0.f) * g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 30.f);
	vSpecular.a = 1.f;

	g_ShadeTexture[vPixel.xy] += vShade;
	g_SpecularTexture[vPixel.xy] += saturate(vSpecular);
}
[numthreads(32, 32, 1)]
void CS_POINT(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float4 vShade = float4(0.f, 0.f, 0.f, 0.f);
	float4 vSpecular = float4(0.f, 0.f, 0.f, 0.f);

	vector vDiffuse = g_DiffuseTexture.Load(int3(vPixel, 0.f));
	vector vNormal = normalize(vector(g_NormalTexture.Load(int3(vPixel, 0.f)).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Load(int3(vPixel, 0.f));
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	vector vView = normalize(vector(g_vCameraPos, 1.f) - vWorldPos);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);

	float fRoughness = g_MaterialTexture.Load(int3(vPixel, 0.f)).x;
	float fRefract = g_MaterialTexture.Load(int3(vPixel, 0.f)).y;
	float fMetalness = g_MaterialTexture.Load(int3(vPixel, 0.f)).z;
	float3 f0 = lerp(Fdielectric, vDiffuse, fRefract);
	float3 f1 = lerp(Fdielectric, vDiffuse, fMetalness);

	if (g_bOnOff)
		vShade = g_vLightDiffuse * (vector(OrenNayar(g_fAlbedo, vNormal, vView, normalize(-vLightDir), fRoughness).xyz, 1.f) + g_vLightAmbient) * fAtt;
	else
		vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt;
	vShade.a = 1.f;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));

	if (g_bOnOff2) {
		float3 V = vView;
		float3 N = vNormal;
		float3 L = normalize(-vLightDir);
		float3 H = normalize(L + V);

		float alpha = fRefract * fRefract;
		float NoL = saturate(dot(N, L));
		float NoV = saturate(dot(N, V));
		float NoH = saturate(dot(N, H));
		float VoH = dot(V, H);

		float D = D_GGX(alpha, NoH);
		float3 F = F_Schlick(f0, VoH);
		float G = G_Schlick_UE4(alpha, NoV, NoL);

		float3 Specular = D * F * G / 4.f * NoL * NoV;
		vSpecular = g_vLightSpecular * vector(Specular, 1.f) * fAtt;

		float3 F1 = F_Schlick(f1, VoH);
		float3 kd = lerp(float3(1, 1, 1) - F1, float3(0, 0, 0), fMetalness);
		float3 kd2 = lerp(float3(0, 0, 0), float3(1, 1, 1)/* - F1*/, fMetalness);
		vShade = vShade * vector(kd, 1.f) * fAtt;

		vector vReflect1 = normalize(reflect(vLook, vNormal));
		float4 vReflect2 = g_CubeTexture.SampleLevel(Sampler_Default, vReflect1.xyz, 0);
		vShade += vReflect2 * vector(kd2, 1.f) * fAtt;
	}
	else
		vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 30.f) * fAtt;
	vSpecular.a = 1.f;

	g_ShadeTexture[vPixel.xy] += vShade;
	g_SpecularTexture[vPixel.xy] += saturate(vSpecular);
}
[numthreads(32, 32, 1)]
void CS_SPOT(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float4 vShade = float4(0.f, 0.f, 0.f, 0.f);
	float4 vSpecular = float4(0.f, 0.f, 0.f, 0.f);

	vector vDiffuse = g_DiffuseTexture.Load(int3(vPixel, 0.f));
	vector vNormal = normalize(vector(g_NormalTexture.Load(int3(vPixel, 0.f)).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Load(int3(vPixel, 0.f));
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	vector vView = normalize(vector(g_vCameraPos, 1.f) - vWorldPos);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);
	float fAngle = dot(normalize(vector(g_vLightDir, 0.f)), normalize(vLightDir));
	float fAtt2 = saturate((fAngle - g_fLightAngle.x) / (g_fLightAngle.y - g_fLightAngle.x));

	float fRoughness = g_MaterialTexture.Load(int3(vPixel, 0.f)).x;
	float fRefract = g_MaterialTexture.Load(int3(vPixel, 0.f)).y;
	float fMetalness = g_MaterialTexture.Load(int3(vPixel, 0.f)).z;
	float3 f0 = lerp(Fdielectric, vDiffuse, fRefract);
	float3 f1 = lerp(Fdielectric, vDiffuse, fMetalness);

	if (false)
		vShade = g_vLightDiffuse * (vector(OrenNayar(g_fAlbedo, vNormal, vView, normalize(-vLightDir), fRoughness).xyz, 1.f) + g_vLightAmbient) * fAtt * fAtt2;
	else
		vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt * fAtt2;
	vShade.a = 1.f;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));

	if (false) {
		float3 V = vView;
		float3 N = vNormal;
		float3 L = normalize(-vLightDir);
		float3 H = normalize(L + V);

		float alpha = fRefract * fRefract;
		float NoL = saturate(dot(N, L));
		float NoV = saturate(dot(N, V));
		float NoH = saturate(dot(N, H));
		float VoH = dot(V, H);

		//float ax, float ay, float NoH, float3 H, float3 X, float3 Y
		//float D = D_GGXaniso(alpha, NoH);
		float D = D_GGX(alpha, NoH);
		float3 F = F_Schlick(f0, VoH);
		float G = G_Schlick_UE4(alpha, NoV, NoL);

		float3 Specular = D * F * G / 4.f * NoL * NoV;
		vSpecular = g_vLightSpecular * vector(Specular, 1.f) * fAtt * fAtt2;

		float3 F1 = F_Schlick(f1, VoH);
		float3 kd = lerp(float3(1, 1, 1) - F1, float3(0, 0, 0), fMetalness);
		float3 kd2 = lerp(float3(0, 0, 0), float3(1, 1, 1)/* - F1*/, fMetalness);
		vShade = vShade * vector(kd, 1.f) * fAtt * fAtt2;

		vector vReflect1 = normalize(reflect(vLook, vNormal));
		float4 vReflect2 = g_CubeTexture.SampleLevel(Sampler_Default, vReflect1.xyz, 0);
		vShade += vReflect2 * vector(kd2, 1.f) * fAtt * fAtt2;
	}
	else
		vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 30.f) * fAtt * fAtt2;
	vSpecular.a = 1.f;

	g_ShadeTexture[vPixel.xy] += vShade;
	g_SpecularTexture[vPixel.xy] += saturate(vSpecular);
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Light {
	pass Pass_Dir {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DIR();
	}
	pass Pass_Point {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_POINT();
	}
	pass Pass_Spot {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_SPOT();
	}
}