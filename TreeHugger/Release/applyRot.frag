#version 400
//
uniform sampler2D rinde;
uniform sampler2D blatt;
//
uniform bool texOnOff;
//
uniform vec3 cameraSpaceLightPos;
//
flat in int blattRinde;
in vec2 outTexCoord;
//
//light
in vec4 cameraSpaceVertexNormal;
in vec4 cameraSpacePosition;
//
out vec4 endColor;
//

float calcAttenuation (out vec3 lightDirection)
{
	float lightAttenuation = 0.01;
	vec3 lightDifference = cameraSpaceLightPos - cameraSpacePosition.xyz;
	float lightDistanceSqr = dot(lightDifference, lightDifference);
	lightDirection = lightDifference * inversesqrt(lightDistanceSqr);

	return (1.0 / (1.0 + lightAttenuation * sqrt(lightDistanceSqr)));
}
//

void main(void)
{
	if (texOnOff == true)
	{
		vec4 lightIntensity = vec4 (0.8, 0.8, 0.8, 1.0);
		vec4 ambientIntensity = vec4(0.3, 0.3, 0.3, 1.0);
		vec4 specularColor = vec4(0.1,0.1,0.1,1.0);
		float shininess = 100.0;

		vec3 lightDir = vec3(0.0);
		float atten = calcAttenuation(lightDir);

		vec4 attenIntensity = atten * lightIntensity;

		vec3 surfaceNormal = normalize (cameraSpaceVertexNormal).xyz;
		float cosAngleIncidence = dot(surfaceNormal, lightDir);
		cosAngleIncidence = clamp (cosAngleIncidence, 0.0, 1.0);

		vec3 viewDir = normalize(-cameraSpacePosition.xyz);
		vec3 halfAngle = normalize(lightDir * viewDir);

		float blinnTerm = dot (surfaceNormal, halfAngle);
		blinnTerm = clamp (blinnTerm, 0.0, 1.0);
		blinnTerm = cosAngleIncidence != 0.0 ? blinnTerm : 0.0;
		blinnTerm = pow(blinnTerm, shininess);

		vec4 texCol = vec4(0.0);
		if (blattRinde == 0)
		{
			texCol = texture2D(rinde, outTexCoord);
		}
		else
		{
			texCol = texture2D(blatt, outTexCoord);
		
			if(texCol.a < 0.15)
				discard;
		}

		endColor = (texCol * cosAngleIncidence) + 
				   (specularColor * attenIntensity * blinnTerm) + 
				   (texCol * ambientIntensity);
		//endColor = texCol;
	}
	else
	{
		endColor = cameraSpacePosition;
	}

}