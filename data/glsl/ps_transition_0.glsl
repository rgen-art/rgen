//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
uniform highp sampler2D u_t2Texture_0;
uniform highp sampler2D u_t2Texture_1;

uniform highp float u_fTime;
uniform highp float u_fDuration;
uniform highp float u_fProgress;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
varying highp vec4 v_v4Texcoord;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void main()
{
    lowp vec3 v3Src = texture2D(u_t2Texture_0, v_v4Texcoord.xy).rgb;
    lowp vec3 v3Dst = texture2D(u_t2Texture_1, v_v4Texcoord.xy).rgb;
    o_v4Color = vec4(mix(v3Src, v3Dst, u_fProgress), 1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

