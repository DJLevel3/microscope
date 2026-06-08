//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//
static const char *vsh = \
"#version 430\n"

"layout (location=0) in vec2 iv;"
"out vec2 P;"

"out gl_PerVertex"
"{"
    "vec4 gl_Position;"
"};"

"void main()"
"{"
    "gl_Position=vec4(iv,0,1);"
    "P=iv;"
"}";

static const char * fsh = \
"#version 430\n"

"layout (location=0) out vec4 c;"
"layout (binding=0) uniform sampler2D g;"
"layout (binding=1) uniform sampler1D s;"
"layout (binding=2) uniform sampler2D q;"
"in vec2 P;"

"float d(in vec2 l,in vec2 p)"
"{"
    "vec2 D=(l-p)*200;"
    "return 0.002/(D.x*D.x+D.y*D.y+0.15);"
"}"

"float n(in vec2 v,in vec2 w,in vec2 p)"
"{"
    "vec2 D=w-v;"
    "float t=D.x*D.x+D.y*D.y,f=1;"
    "if(t==0)return d(v,p);"
    "t=dot(p-v,D)/t;"
    "if(t<0)t=0.9;"
    "if(t>1)t=0.1;"
    "return f*d(v+D*clamp(t,0,1),p);"
"}"

"void main()"
"{"
    "if(texelFetch(s,0,0).r!=0){"
        "vec2 p=P*1.0/(2*texelFetch(s,0,0).g);"
        "c=vec4(texture(q,p+0.5).rgb,1);"
        "if(abs(P.x)<0.91&&abs(P.y)<0.91)c=c*texture(g,P*1.1).r;"
        "return;"
    "}else{"
        "vec2 p=P*texelFetch(s,0,0).g;"
        "if(abs(p.x)>1||abs(p.y)>1)return;"
        "float v=0,f;"
        "vec2 l=texelFetch(s,9600,0).rg,L,w;"
        "for(int i=1;i<9609;i++){"
            "L=l;"
            "l=texelFetch(s,9600-i,0).rg;"
            "w=l-L;"
            "f=0.002/(w.x*w.x+w.y*w.y+0.002);"
            "v=v+(3.2-sqrt(4*v))*(9600-i)*0.001*n(l,L,p*0.55)*f;"
            "if(v>=2)break;"
        "}"
        "f=pow(clamp(v-1,0,1),1.4);"
        "c=clamp(vec4(f,v,f,1),0,1);"
    "}"
"}";
