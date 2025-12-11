#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Vignette
    vec2 pos = fragTexCoord - 0.5;
    float len = length(pos);
    float vignette = smoothstep(0.9, 0.2, len);
    
    // Color Grading (Slight blueish tint for cold cave) - less aggressive
    texelColor.r *= 0.9;
    texelColor.g *= 0.95;
    texelColor.b *= 1.05;

    // Mix vignette with original color to make it more "transparent" / less heavy
    texelColor.rgb = mix(texelColor.rgb, texelColor.rgb * vignette, 0.5);

    finalColor = texelColor * colDiffuse;
}
