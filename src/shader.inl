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
    "gl_Position=vec4(iv,0.0,1.0);"
    "p=iv*18000.0;"
"}";

static const char * fsh = \
"#version 430\n"

"layout (location=0) uniform uint samp[4800];"
"layout (location=0) out vec4 co;"
"in vec2 p;"

"float db(in vec2 l, in vec2 p)"
"{"
    "vec2 df=(l-p)*0.006;"
    "return 0.02/(df.x*df.x+df.y*df.y+0.2);"
"}"

"vec2 dc(in uint v)"
"{"
    "return vec2(float(v&65535)-32768.0,float(v>>16)-32768.0);"
"}"

"void main()"
"{"
    "float v=0.0;"
    "vec2 l1=dc(samp[0]),l2,l3;"
    "int div=0;"
    "for (int i=1;i<4800;i++) {"
        "l3=l1;"
        "l1=dc(samp[i]);"
        "l2=(l3-l1);"
        "div=1+int(length(l2)/4096.0);"
        "l2=l2*(1.0/div);"
        "for (int j=0;j<div;j++)"
        "{"
            "v=v+i*0.0002*db(l1+j*l2,p)/sqrt(float(div));"
        "}"
    "}"
    "float vc=v-1.0;"
    "co=vec4(vc,v,vc,1.0);"
"}";
