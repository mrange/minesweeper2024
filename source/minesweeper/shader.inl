// Generated with Shader Minifier 1.3.6 (https://github.com/laurentlb/Shader_Minifier/)
#ifndef SHADER_MINIFIER_IMPL
#ifndef SHADER_MINIFIER_HEADER
# define SHADER_MINIFIER_HEADER
# define VAR_fcol "v"
# define VAR_state "a"
#endif

#else // if SHADER_MINIFIER_IMPL

// D:\code\github\minesweeper2024\source\minesweeper\shader.frag
"#version 430\n"
 "const float y=1./8;"
 "out vec4 v;"
 "uniform vec4[146] a;"
 "const float s=.825/6,w=acos(-1),f=2*w,x=.825/6,m=s,l[12]=float[](5,12,0,17,14,19,16,14,15,18,19,19);"
 "vec2 d=vec2(.75,.075),r[7]=vec2[](vec2(0),vec2(.5,1),vec2(4,2),vec2(10,1),vec2(10,2),vec2(4,1),vec2(10,2));"
 "int t[20]=int[](125,80,79,87,114,55,63,81,127,119,123,62,45,94,47,43,44,251,41,0);"
 "vec3 n(float y)"
 "{"
   "return 1+sin(vec3(-4,3,1)/2+y);"
 "}"
 "float n(vec2 v,vec2 y)"
 "{"
   "float f=(y.x-y.y)/2;"
   "v.x=abs(v.x);"
   "return(v.x<f?"
     "abs(v.y):"
     "length(v-vec2(f,0)))-y.y;"
 "}"
 "vec3 n(vec2 v,vec3 y,vec3 f,float m,float x)"
 "{"
   "vec2 a=abs(v),i=v,s=v,l=sign(v);"
   "if(a.x>.6+d.y||a.y>1.1+d.y)"
     "return vec3(0);"
   "i.y-=1;"
   "float r=round(i.y);"
   "i.y-=r;"
   "s.y=a.y-.5;"
   "s=abs(s);"
   "vec3 w=(t[int(floor(x))]&1<<int(dot(vec2(1,-1)/sqrt(2),s)>0?"
     "3+(l.x+1)/2+l.y+1:"
     "-r))==0?"
     "f:"
     "y;"
   "return w*smoothstep(m,-m,min(n(i,d),n(a.yx-.5,d)));"
 "}"
 "void main()"
 "{"
   "vec2 s=a[0].xy,d=(2*gl_FragCoord.xy-s)/s.yy,f=abs(d),i=d,w=vec2(.75*x,x),z,c,t,o;"
   "float C=a[0].z/2,F=a[1].z,I=a[0].w,H=a[1].w,p=sqrt(2)/s.y,E=p/m,D=p/.05,B=p/(.25*m),e=sign(i.y),u;"
   "vec3 A=vec3(0),G=vec3(0,0,C),J=normalize(vec3(d,2));"
   "z=d/m-.5;"
   "i.x-=d.y/5-x*.75/2;"
   "i.y=abs(i.y)-.9;"
   "i/=w;"
   "t=round(i);"
   "c=round(z);"
   "i-=t;"
   "z-=c;"
   "o=z*z*z*z;"
   "i*=w;"
   "i.y*=e;"
   "c+=6;"
   "i/=.05;"
   "u=c.x+c.y*12+2;"
   "for(int K=1;K<9;++K)"
     "{"
       "float g=(-G.x+6*sqrt(K))/abs(J).x;"
       "vec3 L=G+J*g;"
       "vec2 M=L.yz*.02,N=round(M),O=M-N;"
       "if(fract(sin(dot((N+K+.5*sign(J.x)).xy,vec2(12.9898,58.233)))*13758.5453)<.5)"
         "O=vec2(O.y,-O.x);"
       "float P=smoothstep(-.7,1.,sin(L.z/9+C+K+sign(J.x))),Q=abs(min(length(O+.5)-.5,length(O-.5)-.5))-.025;"
       "A+=n(.05*g+C)*exp(-.003*g*g)*.0025/max(abs(Q),.003*P)*P;"
     "}"
   "if(t.y==0&&abs(t.x-.5)<6)"
     "{"
       "float K=t.x>0?"
         "mod((e>0?"
           "F:"
           "H)*pow(10,t.x-6),10):"
         "l[int(t.x+5+3*(1-e))];"
       "vec3 O=n(2.5+1.5*e+.4*i.y+(t.x<1?"
         "0:"
         "3));"
       "A+=n(i,O,O*.075,D,K);"
     "}"
   "if(max(f.x,f.y)<.825)"
     "{"
       "vec4 O=a[int(u)];"
       "float K=O.z,N=O.w,Q=pow(dot(o,o),1./8)-.45,M=smoothstep(N+1./2,N+1./8,I),L=smoothstep(K,K+y,I);"
       "vec3 g=tanh(A*8)/8;"
       "for(int P=0;P<2;++P)"
         "{"
           "float R=P==0?"
             "O.y:"
             "O.x,S=P==0?"
             "1-L:"
             "L,T=abs(length(z)-M/9);"
           "vec2 U=r[int(R)];"
           "for(float V=0;V<U.y;++V)"
             "T=min(abs(T-.1),T);"
           "vec3 V=(.2+n(2-R))*(U.x*.005/max(T,.003));"
           "if(R<1)"
             "{"
               "vec2 W=z/.25;"
               "W.x+=-W.y/8;"
               "vec3 X=n(R/2-W.y/2),Y=X/20;"
               "if(R<0)"
                 "g+=n(W,X,Y,B,-R);"
             "}"
           "else"
             " g=mix(g,V,S*smoothstep(E,-E,Q));"
         "}"
       "A=mix(A,g,smoothstep(E,-E,Q));"
       "A=mix(A,mix(n(3.+d.y)/3,vec3(1),M),smoothstep(E,-E,abs(Q)-1./80));"
     "}"
   "v=vec4(sqrt(tanh(A)),1);"
 "}",

#endif
