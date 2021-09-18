Search.setIndex({docnames:["basic/basic_pipeline","basic/compile_shader","basic/constant_buffer","basic/create_device","basic/create_swapchain","basic/create_window","basic/input_assembler","basic/mip_map","basic/post_effect","basic/render_target","basic/shader_reflection","basic/texture","index","source","spec","three_d/depth_test","three_d/gltf","three_d/orbit_camera","three_d/scene"],envversion:{"sphinx.domains.c":2,"sphinx.domains.changeset":1,"sphinx.domains.citation":1,"sphinx.domains.cpp":4,"sphinx.domains.index":1,"sphinx.domains.javascript":2,"sphinx.domains.math":2,"sphinx.domains.python":3,"sphinx.domains.rst":2,"sphinx.domains.std":2,sphinx:56},filenames:["basic/basic_pipeline.md","basic/compile_shader.md","basic/constant_buffer.md","basic/create_device.md","basic/create_swapchain.md","basic/create_window.md","basic/input_assembler.md","basic/mip_map.md","basic/post_effect.md","basic/render_target.md","basic/shader_reflection.md","basic/texture.md","index.md","source.md","spec.md","three_d/depth_test.md","three_d/gltf.md","three_d/orbit_camera.md","three_d/scene.md"],objects:{},objnames:{},objtypes:{},terms:{"##":10,"#\\n":10,"#include":[1,2,3,4,5,9,10],"#pragma":17,"%s":10,"& h":17,"&_":9,"(%d":10,"(&":[2,4,9,10,17],"((":1,"()":[0,1,2,3,4,5,9,10,11,17],"(+":0,"(-":[0,11,17],"(\"":10,"(_":[9,17],"(adapter":3,"(ambient":12,"(banana":17,"(bool":17,"(buffer":2,"(camera":17,"(compiled":10,"(const":[0,1,2,3,4,9,10],"(d":[0,3,10,17],"(desc":[2,10],"(directx":17,"(dx":17,"(eledesc":10,"(element":[0,2,11],"(elements":10,"(featurelevels":3,"(float":[2,17],"(gs":[0,2,11],"(hinstance":5,"(hprevinstance":5,"(hr":[1,2,3,4,9,10],"(hwnd":[4,5],"(i":10,"(iid":4,"(int":17,"(j":10,"(lpcmdline":5,"(p":17,"(pdevice":10,"(pdxgidevice":4,"(point":[0,2,11],"(pos":17,"(ps":[6,12],"(righthanded":17,"(s":11,"(sigdesc":10,"(source":1,"(ss":1,"(std":1,"(t":17,"(texture":9,"(v":17,"(vblob":10,"(vdesc":10,"(viewports":9,"(vs":[6,12],"(x":14,"(xywh":2,"(y":[14,17],"({":10,"))":[1,2,3,4,9,10,17],"),":[1,9,10,17],");":[0,1,2,3,4,5,6,9,10,11,17],")\u3002":0,")]":[0,2,11],")h":9,")w":9,"* _":17,"++":[5,13],"++i":10,"++j":10,"+-":14,", &":[9,10],", clear":9,", compiled":10,", d":9,", dtype":3,", hwnd":4,", name":1,", nullptr":9,", source":1,", static":10,"-+":[0,14],"--":0,"->":[0,2,4,9,10],"-_":17,"-d":12,"-format":13,"-interfaces":12,"-reference":12,"-up":14,"-us":12,"-y":17,"..":[0,1,2,3,4,5,6,9,10,11,17],"._":17,".append":[0,2,11],".arraysize":9,".as":4,".back":10,".bindflags":[2,9],".boundresources":10,".buffercount":4,".bufferusage":4,".bytewidth":2,".color":[0,2],".com":[12,13],".componenttype":10,".constantbuffers":10,".count":[4,9],".cpp":[0,1,2,3,4,5,9,10],".cpuaccessflags":9,".create":5,".cullmode":14,".data":[1,10],".depthenable":9,".depthfunc":9,".depthwritemask":9,".dolly":17,".empty":10,".flags":4,".format":[4,9],".frontcounterclockwise":14,".get":[2,3,4,9],".getaddressof":4,".github":12,".h":[1,2,3,4,5,9,10,17],".height":9,".hlsl":[0,2,6,11],".inputparameters":10,".io":12,".mask":10,".microsoft":12,".miplevels":9,".mipslice":9,".miscflags":9,".name":10,".position":[0,2,6,11],".process":5,".push":10,".quality":[4,9],".rdbuf":1,".restartstrip":[0,2,11],".sampledesc":[4,9],".semanticindex":10,".semanticname":10,".shift":17,".size":[1,10],".startoffset":10,".stencilenable":9,".str":1,".swapeffect":4,".texture":9,".type":10,".usage":[2,9],".uv":11,".variables":10,".viewdimension":9,".w":2,".width":9,".windowed":4,".x":[2,17],".y":[2,17],".yaw":17,".z":[2,17],"/..":[0,1,2,3,4,5,6,9,10,11,17],"//":[1,2,3,4,5,9,10,14],"/assets":[0,2,6,11],"/banana":17,"/basic":0,"/client":[1,3],"/constant":2,"/createwindow":5,"/d":13,"/device":3,"/en":12,"/gorilla":[0,1,2,3,4,9,10],"/ia":6,"/imasaradx":12,"/lib":[0,1,2,3,4,9,10,17],"/main":5,"/orbit":17,"/ousttrue":13,"/pipeline":0,"/render":9,"/samples":5,"/shader":[1,10],"/swapchain":4,"/texture":11,"/win":12,"/window":5,"/windows":12,"0f":[9,10,17],"0x":10,"1f":17,"2d":[3,9,11],"2dms":9,"3d":[1,2,3,4,9,10,12,13,14],"3dblob":[1,10],"3dcompiler":1,"3x":[2,17],"4transform":17,"4x":17,"5f":17,"8a":4,"8g":4,"8r":4,"9f":17,": _":17,":/":[12,13],"::":[0,1,2,4,5,9,10,12,13,14,17],"<<":1,"<comptr":1,"<d":10,"<float":17,"<gs":[0,2,11],"<id":[0,1,2,3,4,9,10],"<idxgiadapter":[3,4],"<idxgidevice":4,"<idxgifactory":4,"<idxgiswapchain":4,"<int":17,"<t":[1,4,10],"<uint":10,"= _":17,"==":10,"=>":6,">(":[10,17],">+":14,">,":1,">:":17,">;":[1,3,4,10],">>":1,"[%d":10,"[]":[3,9,18],"\")":[5,10],"\",":[5,10],"\"/":1,"\\n":10,"\u3002\u3044\u3064\u3082\u306e":18,"\u3042\u308b":[0,2,3],"\u304b\u3064\u7dd1":0,"\u304b\u3064\u8d64":0,"\u304b\u3064\u9752":0,"\u304b\u3089":[0,4,9],"\u304c\u308f\u304b\u308b":0,"\u3053\u3068":[0,2,15],"\u3054\u3068":10,"\u3057\u3066":15,"\u3059\u308b":[0,2,10,13,14,15,17,18],"\u305b\u308b":14,"\u305d\u308c":14,"\u305f\u3044":14,"\u3067\u304d\u308b":[0,9,17],"\u3068\u3044\u3046":0,"\u306a\u3044":[0,4],"\u306a\u304f":[0,2],"\u306a\u3057":13,"\u306a\u3063":[0,9],"\u306a\u306e":0,"\u306a\u308b":[0,2,11],"\u306b\u5bfe\u5fdc":0,"\u306e\u3067":[0,9,14,17],"\u3082\u3057":0,"\u3088\u3046":[9,11],"\u308c\u308b":[0,3],"\u30a2\u30d7\u30ea\u30b1\u30fc\u30b7\u30e7\u30f3":3,"\u30a4\u30f3\u30d7\u30c3\u30c8\u30ec\u30a4\u30a2\u30a6\u30c8":12,"\u30aa\u30d7\u30b7\u30e7\u30f3":13,"\u30ab\u30e1\u30e9":17,"\u30ab\u30fc\u30bd\u30eb":2,"\u30ad\u30c3\u30af":0,"\u30af\u30ea\u30a2":9,"\u30b5\u30a4\u30ba":[2,10],"\u30b7\u30a7\u30fc\u30c0\u30fc":[0,12],"\u30b7\u30fc\u30f3":15,"\u30b9\u30c6\u30fc\u30b8":10,"\u30b9\u30ed\u30c3\u30c8":10,"\u30c0\u30df\u30fc":0,"\u30c7\u30d0\u30a4\u30b9":3,"\u30c7\u30d5\u30a9\u30eb\u30c8":12,"\u30c7\u30fc\u30bf":6,"\u30d0\u30a4\u30c8\u30a2\u30e9\u30a4\u30e1\u30f3\u30c8":2,"\u30d0\u30c3\u30d5\u30a1":[6,12,15],"\u30d3\u30e5\u30fc\u30dd\u30fc\u30c8":12,"\u30da\u30fc\u30b8":12,"\u30de\u30a6\u30b9":[2,17],"\u30e1\u30e2\u30ea\u30ec\u30a4\u30a2\u30a6\u30c8":12,"\u30e2\u30b8\u30e5\u30fc\u30eb":12,"\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc":17,"\u30ec\u30a4\u30a2\u30a6\u30c8":12,"\u30ed\u30fc\u30c0\u30fc":[16,18],"\u30ed\u30fc\u30c9":17,"\u4e09\u89d2\u5f62":[0,2,12],"\u4e2d\u5fc3":14,"\u4e88\u5b9a":[12,17],"\u4ed5\u69d8":14,"\u4ed5\u69d8\u6c7a\u3081":12,"\u4ee5\u4e0b":14,"\u4f4d\u7f6e":2,"\u4f5c\u3063":9,"\u4f5c\u308b":[3,4,18],"\u4f75\u7528":14,"\u4f7f\u3046":[0,6,11,17],"\u4f7f\u308f":4,"\u4f8b\u3048":2,"\u5099\u5fd8":12,"\u5165\u308b":0,"\u5165\u529b":0,"\u51fa\u529b":0,"\u539f\u70b9":[11,14],"\u53cd\u5bfe":0,"\u53cd\u6642":0,"\u53d6\u308a\u6271\u3044":14,"\u53f3\u624b":[14,17],"\u5408\u308f":14,"\u5468\u308a":0,"\u5834\u5408":3,"\u5909\u6570":12,"\u59cb\u52d5":0,"\u5c0e\u5165":18,"\u5de6\u4e0a":[11,14],"\u5ea7\u6a19":[2,12],"\u5f97\u3066":9,"\u5f97\u308b":10,"\u5fc5\u8981":[2,3,15],"\u5fd8\u308c":3,"\u63cf\u753b":2,"\u64cd\u4f5c":17,"\u6570\u5b66":17,"\u6700\u5c0f":0,"\u6728\u69cb":18,"\u683c\u7d0d":2,"\u691c\u7d22":12,"\u69cb\u6210":0,"\u6b21\u5143":0,"\u6ce8\u610f":2,"\u6d41\u308c\u308b":6,"\u6df1\u5ea6":15,"\u751f\u6210":10,"\u756a\u53f7":10,"\u7701\u7565":12,"\u7ba1\u7406":10,"\u7d22\u5f15":12,"\u7d42\u4e86":3,"\u7d4c\u7531":2,"\u81ea\u52d5":10,"\u8868\u793a":3,"\u88cf\u5411\u304d":0,"\u898b\u3048":0,"\u8a08\u56de\u308a":0,"\u8a18\u8ff0":13,"\u8a66\u3057":0,"\u9001\u308a\u8fbc\u3093":2,"\u9593\u63a5":0,"\u9762\u63cf":15,"\u9802\u70b9":12,"\uff12\u3064":14,"])":[3,9],"].":9,"]:":0,"]\\":10,"break":10,"byte":10,"case":10,"char":1,"class":[5,13],"const":[1,2,9,10,17],"else":[4,17],"false":[0,2,9,10,17],"float":[0,2,6,9,10,11,14,17],"for":[3,5,10],"if":[1,2,3,4,5,9,10,17],"in":1,"int":[5,9,17],"null":9,"public":17,"return":[0,1,2,3,4,5,6,9,10,11,17],"static":[10,17],"switch":10,"true":[0,2,4,9,10,14],"void":[0,2,9,11,17],"{ _":17,"{_":9,"{{":9,"{}":17,"|gs":12,"})":10,"};":[0,1,2,3,4,6,9,10,11,14,17],"}}":9,_aligned:10,_all:9,_allow:4,_alt:4,_args:4,_b:4,_back:[0,10,14],_base:1,_bgra:3,_bind:10,_buffer:[2,10],_camera:17,_case:13,_cast:[10,17],_chain:4,_changes:4,_component:10,_constant:[2,10],_d:9,_data:10,_debug:3,_default:[2,9],_depth:9,_desc:[0,4,9,10,14],_device:3,_dimension:9,_discard:4,_driver:3,_dsv:9,_effect:4,_element:10,_elements:10,_empty:0,_enable:1,_enter:4,_equal:9,_factory:4,_feature:3,_file:1,_flag:4,_flip:4,_float:10,_format:[4,9,10],_fullscreen:4,_h:17,_hardware:3,_height:17,_id:10,_in:6,_input:10,_layout:10,_less:9,_level:3,_limits:17,_list:9,_macro:1,_major:14,_mask:9,_matrix:17,_messages:5,_mode:4,_mwa:4,_name:5,_no:4,_output:[0,2,4,11],_parameter:[5,10],_per:10,_pitch:17,_point:1,_pointlist:0,_position:[0,2,6,11],_ppv:4,_projection:17,_r:10,_reflection:10,_render:4,_rtv:9,_s:9,_sampler:10,_screen:2,_sequential:4,_shader:1,_sit:10,_slots:10,_stencil:9,_strictness:1,_support:3,_swap:4,_swapchain:4,_switch:4,_target:[0,2,4,6,9,11],_texture:[9,10],_to:2,_topology:0,_type:[3,10],_unknown:10,_usage:4,_variable:10,_version:3,_vertex:10,_view:[1,9,13,17],_window:4,_write:9,_x:[2,17],_y:[2,17],adapter:3,ads:12,analize:10,animation:12,append:10,assert:[1,2],auto:[1,2,4,5,9,10,17],backbuffer:9,banana:17,basecolor:[14,18],basetexture:18,basic:12,binary:1,bind:[2,9],bool:[2,9,10,17],buffer:[1,10,12],calc:17,camera:12,cb:10,cbuffer:14,clang:13,clear:9,cleardepth:9,cleardepthstencilview:9,clearrendertargetview:9,cmake:13,color:[0,2],com:3,comparison:9,compile:1,compiled:10,compileshader:12,comptr:[0,1,2,4,9,10,12],console:3,constant:[10,12],constantbuffer:[2,12,14],context:[0,2,9],core:12,countof:9,create:[2,3,4,9,10],createbuffer:2,createdepthstencilview:9,createdevice:12,createdeviceandswapchain:4,createinputlayout:10,createrendertargetview:9,createswapchain:12,createswapchainforhwnd:4,createtexture:9,createwindow:12,cube:15,cull:[0,14],cullmode:0,d3d:[0,2,3,9,10,14],d3dcompile:1,d3dreflect:10,debug:3,define:1,depth:[9,12],depthbuffer:9,depthdesc:9,depthstencil:9,desc:[2,9,10],device:[2,3,4,9,10],devicecontext:[0,2,9],diffuse:12,dir:1,direct:12,directx:[2,14,17],directxmath:17,docs:12,dolly:17,draw:[0,12,15],dss:9,dssdesc:9,dsv:9,dsvdesc:9,dtype:3,dummyinput:[0,2,11],dx:17,dxgi:[4,9,10],dy:17,eledesc:10,element:[0,2,11],elements:10,endif:[3,4],entry:1,err:1,failed:[1,2,3,4,9,10],far:17,featurelevels:3,filesystem:[1,13],fit:17,flag:12,flags:[1,3],format:10,fovyrad:17,framecount:5,frontcounterclockwise:0,fstream:1,geometryshader:12,get:[4,10],getadapter:4,getbufferpointer:10,getbuffersize:10,getconstantbufferbyindex:10,getdesc:[9,10],getdxgiformat:10,getinputparameterdesc:10,getparent:4,getresourcebindingdesc:10,getvariablebyindex:10,github:13,gltf:[12,14,17,18],gorilla:[1,2,3,4,5,9,10],graphics:12,gs:[0,2,10,11],gsmain:[0,2,11],half:17,hardcoding:10,height:9,hinstance:5,hlsl:2,hprevinstance:5,hr:[1,2,3,4,9,10],hresult:[3,4],https:[12,13],hwnd:[4,5],iasetprimitivetopology:0,iasetvertexbuffers:0,id:[4,9,10,12],ifdef:3,ifstream:1,iid:10,include:[1,2,4,10,17],initialize:10,inout:[0,2,11],input:[0,2,6,10,11],inputassembler:12,inputlayout:10,inverse:17,ios:1,ispress:17,left:17,light:[14,18],limits:17,loop:5,lpcmdline:5,main:5,makewindowassociation:4,mask:10,mat:[2,18],material:[12,18],matrix:14,maxdepth:9,maxvertexcount:[0,2,11],mesh:[12,18],microsoft:[1,4,10,12],middle:17,min:17,mindepth:9,mipmap:12,model:[14,18],morphtarget:12,mousebinder:17,move:17,mvp:[14,18],name:[1,13],namespace:[1,2,3,4,9,10,17],ncmdshow:5,near:17,node:[12,18],normal:17,normalmap:12,normalmatrix:18,nullptr:[0,1,2,3,4],numeric:17,numfeaturelevels:3,object:14,omsetdepthstencilstate:9,omsetrendertargets:9,once:17,orbit:12,orbitcamera:17,other:13,output:[0,2,6,11],outputdebugprintfa:10,padding:2,path:1,pdxgiadapter:4,pdxgidevice:4,pdxgifactory:4,pipeline:12,pitch:17,pixel:2,pixelshader:6,point:0,pos:17,position:[0,2,6,11,17],posteffect:12,preflector:10,primitive:0,projection:[17,18],ps:[6,10],psmain:[0,2,6,11],pswapchain:4,pwstr:5,rasterizer:[0,14],rasterizerstate:12,read:1,rect:2,reflect:10,reflection:12,register:10,release:3,render:9,rendertarget:12,rendertargetview:9,renderterget:9,resize:17,ret:1,right:17,rotation:18,row:14,rs:14,rssetviewports:9,rtv:9,sahder:0,sample:11,sampler:[10,11],samplerstate:11,samples:13,scale:18,sd:4,sdk:3,sdkversion:3,set:9,setup:9,sfd:4,shader:12,shaderdesc:10,shaderreflection:10,shaderstage:10,shift:17,showwindow:5,sigdesc:10,signature:10,size:2,sizeof:3,skinning:12,snake:13,source:[1,12],space:12,span:13,specular:12,src:1,srv:[10,11],ss:1,sstream:1,std:[1,10,13,17],string:[1,13],stringstream:1,struct:[0,2,6,11,17],submesh:18,sv:[0,2,6,11],swapchain:9,tan:17,target:1,template:[1,3,4,10],test:12,texcoord:11,texture:[9,12,18],todo:14,topleftx:9,toplefty:9,tositeru:12,transform:18,translation:18,transpose:17,trianglestream:[0,2,11],tuple:1,type:10,typename:[1,3,4,10],types:17,uint:[1,2,3,5,9,10],unorm:[4,9],unreferenced:5,update:2,updatesubresource:2,updatewindow:5,uppercamel:13,usage:[2,9],using:[1,3,4,10],uv:[11,12],validfeaturelevel:3,variables:10,vbelement:10,vblob:10,vc:13,vdesc:10,vector:10,vertexshader:[6,10],view:[17,18],viewport:[2,9,12],viewports:9,vs:[6,10],vsmain:[0,2,6,11],wheel:17,width:9,winapi:5,window:5,windows:13,world:12,wrl:[1,4,10,12],wwinmain:5,xmconverttoradians:17,xmfloat:17,xmloadfloat:17,xmmatrixmultiply:17,xmmatrixperspectivefovrh:[14,17],xmmatrixrotationx:17,xmmatrixrotationy:17,xmmatrixtranslation:17,xmstorefloat:17,xmvector:17,xy:10,xywh:2,xyz:10,xyzw:10,yaw:17,yawpitch:17},titles:["Basic Pipeline","CompileShader","Constant Buffer","CreateDevice","CreateSwapChain","CreateWindow","InputAssembler","MipMap","PostEffect","RenderTarget","Shader Reflection","Texture","Root","Source","\u4ed5\u69d8\u6c7a\u3081","Depth Test","glTF","Orbit Camera","SceneGraph"],titleterms:{"(ps":18,"(vs":18,"3d":0,"::":3,"<t":3,"\u30a4\u30f3\u30d7\u30c3\u30c8\u30ec\u30a4\u30a2\u30a6\u30c8":10,"\u30b7\u30a7\u30fc\u30c0\u30fc":10,"\u30c7\u30d5\u30a9\u30eb\u30c8":0,"\u30d0\u30c3\u30d5\u30a1":0,"\u30d0\u30c3\u30d5\u30a1\u30fc":12,"\u30d1\u30a4\u30d7\u30e9\u30a4\u30f3":12,"\u30d3\u30e5\u30fc\u30dd\u30fc\u30c8":0,"\u30e1\u30e2\u30ea\u30ec\u30a4\u30a2\u30a6\u30c8":2,"\u30ec\u30a4\u30a2\u30a6\u30c8":6,"\u4e09\u6b21":12,"\u4e09\u89d2\u5f62":14,"\u4e88\u5b9a":14,"\u4ed5\u69d8\u6c7a\u3081":14,"\u5143\u5316":12,"\u53c2\u8003":12,"\u53d6\u308a\u6271\u3044":12,"\u5909\u6570":10,"\u5c0e\u5165":12,"\u5ea7\u6a19":0,"\u6e96\u5099":12,"\u7701\u7565":0,"\u9802\u70b9":[0,6],"|gs":18,and:12,animation:16,basic:0,buffer:[2,14],camera:17,compileshader:1,comptr:3,constant:[2,14],constantbuffer:[10,18],createdevice:3,createswapchain:4,createwindow:5,depth:15,draw:[14,18],flag:3,geometryshader:0,gltf:16,id:0,indices:12,inputassembler:6,level:14,lighting:12,material:16,mesh:16,microsoft:3,mipmap:7,morphtarget:16,node:16,orbit:17,pipeline:0,posteffect:8,projection:12,rasterizerstate:0,reflection:10,rendertarget:9,root:12,samplerslot:10,scenegraph:[12,18],shader:10,skinning:16,source:13,space:14,tables:12,test:15,texture:[11,16],textureslot:10,uv:14,viewport:14,world:[14,18],wrl:3}})