Texture2D EarthColor: register(t0);
sampler EarthColorSampler: register(s0);


struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: TEXCOORD1;
	float3 direction_to_camera: TEXCOORD2;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float4 earth_color = EarthColor.Sample(EarthColorSampler, 1.0 - input.texcoord);

	//AMBIENT LIGHT
	float ka = 1.2;
	float3 ia = float3(0.09, 0.082, 0.082); // white color
	ia *= earth_color.rgb;
	float3 ambient_light = ka * ia;

	//DIFFUSE LIGHT
	float kd = 0.7;
	float3 id = float3(1.0, 1.0, 1.0);
	id *= earth_color.rgb;

	float amount_diffuse_light = max(0.0, dot(m_light_direction.xyz, input.normal));

	float3 diffuse_light = kd * amount_diffuse_light * id;

	//SPECULAR LIGHT
	float ks = 1.0;
	float3 is = float3(1.0, 1.0, 1.0);
	float reflected_light = reflect(m_light_direction.xyz, input.normal);
	float shininess = 30.0;
	float amount_specular_light = pow(max(0.0, dot(reflected_light, input.direction_to_camera)), shininess);

	float3 specualr_light = ks * amount_specular_light * is;

	float3 final_light = ambient_light + diffuse_light + specualr_light;

	return float4(final_light, 1.0);
}