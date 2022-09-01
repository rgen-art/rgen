//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
uniform highp vec4 u_v4Color;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
varying highp vec4 v_v4Texcoord;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void main()
{
    highp vec2 p = v_v4Texcoord.xy;
    highp float s = v_v4Texcoord.z;

    highp vec2 px = dFdx(p);
    highp vec2 py = dFdy(p);

    highp float fx = 2.0 * p.x * px.x - px.y;
    highp float fy = 2.0 * p.x * py.x - py.y;

    highp float sd = (p.x * p.x - p.y) / sqrt(fx * fx + fy * fy);
    highp float alpha =  s > 0.0 ? (0.5 - sd) : sd;

    if (alpha > 1.0)
    { o_v4Color = u_v4Color; }
    else if (alpha < 0.0)
    { o_v4Color = vec4(0.0, 0.0, 0.0, 0.0); }
    else
    { o_v4Color = vec4(u_v4Color.xyz, u_v4Color.w * alpha); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
