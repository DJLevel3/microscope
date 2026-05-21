//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//
static const char *vsh = \
"#version 430\n"

"layout (location=0) in vec2 iv;"
"out vec2 p;"

"out gl_PerVertex"
"{"
    "vec4 gl_Position;"
"};"

"void main()"
"{"
    "gl_Position=vec4(iv,0,1);"
    "p=iv;"
"}";

static const char * fsh = \
"#version 430\n"

"layout (location=0) out vec4 c;"
"layout (binding=0) uniform sampler2D g;"
"layout (binding=1) uniform sampler1D s;"
"in vec2 p;"

"float d(in vec2 l,in vec2 p)"
"{"
    "vec2 D=(l-p)*180;"
    "return 0.003/(D.x*D.x+D.y*D.y+0.2);"
"}"

"float n(in vec2 v,in vec2 w,in vec2 p)"
"{"
    "vec2 D=w-v;"
    "float t=D.x*D.x+D.y*D.y,f=1;"
    "if(t==0)return d(v,p);"
    "t=dot(p-v,D)/t;"
    "if(t<0||t>1)f=0.4;"
    "return f*d(v+D*clamp(t,0,1),p);"
"}"

"void main()"
"{"
    "float v=0,f;"
    "vec2 l=texelFetch(s,0,0).rg,L,w;"
    "for(int i=1;i<4800;i++){"
        "L=l;"
        "l=texelFetch(s,i,0).rg;"
        "w=l-L;"
        "f=0.002/(w.x*w.x+w.y*w.y+0.002);"
        "v=v+i*0.003*n(l,L,p*0.55)*f;"
    "}"
    "c=clamp(vec4(v-1,v,v-1,1),0,1);"
    "if(abs(p.x)<0.91&&abs(p.y)<0.91)c=c*texture(g,p*1.1).r;"
"}";
