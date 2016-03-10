
// textures
uniform sampler1D transferfunction;
uniform sampler3D vol_texture;
uniform sampler2D front_texture;
uniform sampler2D back_texture;
uniform float ambient;
uniform float iso_value;
uniform float step_size;

float base_distance = 0.0025f;
// parametes
uniform vec4 params;


float shininess = 50.0f;
void main()
{
	//debug output: a single color
	//gl_FragColor = params;


	//initalize accumulated color and opacity
	vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);

	float scalar;

	//normalized ray direction

	vec3 raydir = normalize(vec3(texture2D(back_texture, gl_TexCoord[0].st))
		- vec3(texture2D(front_texture, gl_TexCoord[0].st)));

	vec3 position = vec3(texture2D(front_texture, gl_TexCoord[0].st));
	int i;

	vec3 hit_pos;
	int flag = 0;

	//clip plane
	position = position + 200 * raydir * step_size;
	for (i = 0; i < 500; i++)
	{
		//data access to scalar value in 3D valume texture
		vec4 value = texture3D(vol_texture, position);
		scalar = value.a;
		
		if (scalar >= iso_value && !flag)
		{
			flag = 1;
			hit_pos = position;
		}

		//opacity correction
		scalar = 1 - pow((1 - scalar), step_size / base_distance);

		//apply transfer function
		vec4 src = texture1D(transferfunction, scalar);

		//front to back compositing
		
		dst = (1.0 - dst.a) * src + dst;

		//update ray position along ray direction
		position = position + raydir * step_size;

		//ray termination: test if outside volume
		//early ray termination
		if (dst.a > 0.95)
			break;
	}



	vec3 Ld = vec3(1.0f, 1.0f, 1.0f);
	vec3 La = vec3(0.4f, 0.4f, 0.4f);
	vec3 Ls = vec3(1.0f, 1.0f, 1.0f);
	vec3 Kd = vec3(0.9f, 0.5f, 0.3f);
	vec3 Ka = vec3(0.9f, 0.5f, 0.3f);
	vec3 Ks = vec3(0.8f, 0.8f, 0.8f);

	//shading
	vec3 pos = hit_pos;

	vec4 result = texture1D(transferfunction, texture3D(vol_texture, pos).a);

	float THRESHOLD = 0.05;
	float delta = 0.01;


	vec3 diffuse = vec3(0.0);
	vec3 spec = vec3(0.0);
	vec3 ambi;

	ambi = ambient * La * Ka;

	if (result.a > THRESHOLD)
	{
		vec3 norm;

		norm.x = texture3D(vol_texture, pos + half3(delta, 0.0, 0.0)).x - texture3D(vol_texture, pos - half3(delta, 0.0, 0.0)).x;
		norm.y = texture3D(vol_texture, pos + half3(0.0, delta, 0.0)).x - texture3D(vol_texture, pos - half3(0.0, delta, 0.0)).x;
		norm.z = texture3D(vol_texture, pos + half3(0.0, 0.0, delta)).x - texture3D(vol_texture, pos - half3(0.0, 0.0, delta)).x;

		norm = normalize(norm);


		float sDotN = max(dot(raydir, norm), 0.0);

		diffuse = sDotN * Ld * Kd;

		vec3 v = normalize(-pos.xyz);
		vec3 r = reflect(-raydir, norm);

		if (sDotN > 0.0)
			spec = Ls * Ks * pow(max(dot(r, v), 0.0), shininess);
	}

	gl_FragColor = (vec4(ambi + diffuse + spec, 1.0)) * dst;


}
