// Vertex shader
#version 420

uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;
uniform mat4 matModel_IT;


in vec4 vCol;
in vec4 vPos;
in vec4 vNormal;
in vec2 vTextureCoords;

out vec4 gColour;
out vec4 gVertexWorldPos;	
out vec4 gVertexWorldNormal;
out vec2 gTextureCoords;

uniform bool bUseHeightMap;
uniform sampler2D heightMapSampler;
uniform float heightScale;
uniform vec2 UVOffset;

void main()
{
	vec4 vertexModelPosition = vPos;	

	vec2 UVFinal = vTextureCoords.st;
	
	if ( bUseHeightMap )
	{
		// Greyscale (black and white) heightmap image
		// vec3 heightRGB = texture( heightMapSampler, vTextureCoords.st ).rgb;
		// float height = (heightRGB.r + heightRGB.g + heightRGB.b) / 3.0f;

		UVFinal += UVOffset.yx;
		float height = texture( heightMapSampler, UVFinal.st ).r;
		
		// Adjust the height of the y axis from this "colour"
		// const float SCALEADJUST = 25.0f;
		// vertexModelPosition.y += ( height * SCALEADJUST );

		vertexModelPosition.y += ( height * heightScale );
	}

	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(vertexModelPosition.xyz, 1.0);
	
		
	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	gVertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	gVertexWorldNormal.xyz = normalize(gVertexWorldNormal.xyz);
	gVertexWorldNormal.w = 1.0f;

	gVertexWorldPos = matModel * vec4( vertexModelPosition.xyz, 1.0f);
	
	gColour = vCol;

	// Copy the UV coordinates unchanged (to the fragment shader)
	gTextureCoords = UVFinal;
}
