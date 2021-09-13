Search.setIndex({docnames:["basic/basic_pipeline","basic/compile_shader","basic/constant_buffer","basic/create_device","basic/create_swapchain","basic/create_window","basic/input_assembler","basic/render_target","basic/shader_reflection","basic/texture","index","source","three_d/depth_test","three_d/orbit_camera"],envversion:{"sphinx.domains.c":2,"sphinx.domains.changeset":1,"sphinx.domains.citation":1,"sphinx.domains.cpp":4,"sphinx.domains.index":1,"sphinx.domains.javascript":2,"sphinx.domains.math":2,"sphinx.domains.python":3,"sphinx.domains.rst":2,"sphinx.domains.std":2,sphinx:56},filenames:["basic/basic_pipeline.md","basic/compile_shader.md","basic/constant_buffer.md","basic/create_device.md","basic/create_swapchain.md","basic/create_window.md","basic/input_assembler.md","basic/render_target.md","basic/shader_reflection.md","basic/texture.md","index.md","source.md","three_d/depth_test.md","three_d/orbit_camera.md"],objects:{},objnames:{},objtypes:{},terms:{"##":8,"#\\n":8,"#include":[1,2,3,4,5,7,8],"#pragma":13,"%s":8,"& h":13,"&_":[2,7],"(%d":8,"(&":[2,4,7,8,13],"((":1,"()":[0,1,2,3,4,5,7,8,13],"(+":0,"(-":0,"(\"":8,"(_":[2,7,13],"(adapter":3,"(bool":13,"(camera":13,"(compiled":8,"(const":[0,1,2,3,4,7,8],"(d":[0,3,8,13],"(desc":8,"(directx":13,"(dx":13,"(eledesc":8,"(element":[0,2],"(featurelevels":3,"(float":[2,6,13],"(gorilla":13,"(gs":[0,2],"(hinstance":5,"(hprevinstance":5,"(hr":[1,2,3,4,7,8],"(hwnd":[4,5],"(i":8,"(iid":4,"(int":13,"(j":8,"(lpcmdline":5,"(pdevice":8,"(pdxgidevice":4,"(point":[0,2],"(ps":6,"(righthanded":13,"(sigdesc":8,"(slot":2,"(source":1,"(ss":1,"(std":1,"(texture":7,"(v":13,"(vblob":8,"(vdesc":8,"(viewports":7,"(vs":6,"(xywh":2,"(y":13,"({":8,"))":[1,2,3,4,7,8],"),":[1,6,7,8,13],");":[0,1,2,3,4,5,6,7,8,13],")]":[0,2],")h":7,")w":7,"* _":13,"++":[5,11],"++i":8,"++j":8,", &":[7,8],", clear":7,", compiled":8,", d":7,", dtype":3,", hwnd":4,", name":1,", nullptr":7,", source":1,", static":8,"-+":0,"--":0,"->":[0,2,4,7,8],"-d":10,"-format":11,"-interfaces":10,"-reference":10,"-us":10,"..":[0,1,2,3,4,5,6,7,8,13],".append":[0,2],".arraysize":7,".as":4,".back":8,".bindflags":[2,7],".boundresources":8,".buffercount":4,".bufferusage":4,".bytewidth":2,".color":[0,2],".com":[10,11],".componenttype":8,".constantbuffers":8,".count":[4,7],".cpp":[0,1,2,3,4,5,7,8],".cpuaccessflags":7,".create":5,".data":[1,8],".depthenable":7,".depthfunc":7,".depthwritemask":7,".dolly":13,".flags":4,".format":[4,7],".get":[2,3,4,7],".getaddressof":[2,4],".github":10,".h":[1,2,3,4,5,7,8,13],".height":7,".hlsl":[0,2,6],".inputparameters":8,".io":10,".mask":8,".microsoft":10,".miplevels":7,".mipslice":7,".miscflags":7,".name":8,".position":[0,2,6],".process":5,".push":8,".quality":[4,7],".rdbuf":1,".restartstrip":[0,2],".sampledesc":[4,7],".semanticindex":8,".semanticname":8,".shift":13,".size":[1,8],".startoffset":8,".stencilenable":7,".str":1,".swapeffect":4,".texture":7,".type":8,".usage":[2,7],".variables":8,".viewdimension":7,".w":2,".width":7,".windowed":4,".x":[2,13],".y":[2,13],".yaw":13,".z":[2,13],"/..":[0,1,2,3,4,5,6,7,8,13],"//":[1,2,3,4,5,7,8],"/assets":[0,2,6],"/basic":0,"/basicpipeline":0,"/camera":13,"/client":[1,3],"/compileshader":1,"/constant":2,"/constantbuffer":2,"/createdevice":3,"/createswapchain":4,"/createwindow":5,"/d":11,"/device":3,"/en":10,"/gorilla":[0,1,2,3,4,7,8,13],"/ia":6,"/imasaradx":10,"/main":5,"/orbit":13,"/ousttrue":11,"/pipeline":0,"/render":7,"/rendertarget":7,"/samples":[0,1,2,3,4,5,7,8,13],"/shader":[1,8],"/shaderreflection":8,"/swapchain":4,"/win":10,"/window":5,"/windows":10,"0f":[7,8,13],"0x":8,"1f":13,"2d":[3,7],"2dms":7,"3d":[0,1,2,3,4,7,8,11],"3dblob":[1,8],"3dcompiler":1,"4x":13,"8a":4,"8g":4,"8r":4,"9f":13,": _":13,":/":[10,11],"::":[0,1,2,3,4,5,7,8,13],"<<":1,"<comptr":1,"<d":8,"<float":13,"<gs":[0,2],"<id":[0,1,2,3,4,7,8],"<idxgiadapter":[3,4],"<idxgidevice":4,"<idxgifactory":4,"<idxgiswapchain":4,"<int":13,"<t":[1,3,4,8],"<uint":8,"= _":13,"==":8,"=>":6,">(":[8,13],">,":1,">:":13,">;":[1,3,4,8],">>":1,"[%d":8,"[]":[3,7],"\")":[5,8],"\",":[5,8],"\"/":1,"\\n":8,"\u3042\u308b":[0,3],"\u304b\u3064\u7dd1":0,"\u304b\u3064\u8d64":0,"\u304b\u3064\u9752":0,"\u304b\u3089":[0,4,7],"\u304c\u308f\u304b\u308b":0,"\u3053\u3068":[0,12],"\u3054\u3068":8,"\u3057\u3066":12,"\u3059\u308b":[0,2,8,11,12,13],"\u3067\u304d\u308b":[0,7,13],"\u3068\u3044\u3046":0,"\u306a\u3044":[0,4],"\u306a\u304f":0,"\u306a\u3057":11,"\u306a\u3063":7,"\u306a\u308b":0,"\u306b\u5bfe\u5fdc":0,"\u306e\u3067":[0,7,13],"\u3082\u3057":0,"\u3088\u3046":7,"\u308c\u308b":3,"\u30a2\u30d7\u30ea\u30b1\u30fc\u30b7\u30e7\u30f3":3,"\u30aa\u30d7\u30b7\u30e7\u30f3":11,"\u30ab\u30e1\u30e9":13,"\u30ab\u30fc\u30bd\u30eb":2,"\u30ad\u30c3\u30af":0,"\u30af\u30ea\u30a2":7,"\u30b5\u30a4\u30ba":[2,8],"\u30b7\u30a7\u30fc\u30c0\u30fc":[0,10],"\u30b7\u30fc\u30f3":12,"\u30b9\u30c6\u30fc\u30b8":8,"\u30b9\u30ed\u30c3\u30c8":8,"\u30c0\u30df\u30fc":0,"\u30c7\u30d0\u30a4\u30b9":3,"\u30c7\u30d5\u30a9\u30eb\u30c8":0,"\u30c7\u30fc\u30bf":6,"\u30d0\u30c3\u30d5\u30a1":[6,10,12],"\u30d3\u30e5\u30fc\u30dd\u30fc\u30c8":10,"\u30da\u30fc\u30b8":10,"\u30de\u30a6\u30b9":[2,13],"\u30e2\u30b8\u30e5\u30fc\u30eb":10,"\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc":13,"\u30ec\u30a4\u30a2\u30a6\u30c8":10,"\u30ed\u30fc\u30c9":13,"\u4e09\u89d2\u5f62":[0,2],"\u4e88\u5b9a":13,"\u4f4d\u7f6e":2,"\u4f5c\u3063":7,"\u4f5c\u308b":[3,4],"\u4f7f\u3046":[0,3,6,9,13],"\u4f7f\u308f":4,"\u5099\u5fd8":10,"\u5165\u308b":0,"\u5165\u529b":0,"\u51fa\u529b":0,"\u53f3\u624b":13,"\u56de\u308a":0,"\u5834\u5408":3,"\u5909\u6570":10,"\u59cb\u52d5":0,"\u5ea7\u6a19":[2,10],"\u5f97\u3066":7,"\u5f97\u308b":8,"\u5fc5\u8981":[3,12],"\u5fd8\u308c":3,"\u63cf\u753b":2,"\u64cd\u4f5c":13,"\u6570\u5b66":13,"\u6642\u8a08":0,"\u6700\u5c0f":0,"\u691c\u7d22":10,"\u69cb\u6210":0,"\u6b21\u5143":0,"\u6d41\u308c\u308b":6,"\u6df1\u5ea6":12,"\u756a\u53f7":8,"\u7701\u7565":10,"\u7b26\u53f7":0,"\u7ba1\u7406":8,"\u7d22\u5f15":10,"\u7d42\u4e86":3,"\u7d4c\u7531":2,"\u8868\u793a":3,"\u898b\u3048":0,"\u8a18\u8ff0":11,"\u8a66\u3057":0,"\u9001\u308a\u8fbc\u3093":2,"\u9006\u306b":0,"\u9593\u63a5":0,"\u9762\u63cf":12,"\u9802\u70b9":10,"])":[3,7],"].":7,"]:":0,"]\\":8,"break":8,"byte":8,"case":8,"char":1,"class":[5,11,13],"const":[1,2,7,8,13],"else":[4,13],"false":[0,2,7,8,13],"float":[0,2,6,7,8,13],"for":[3,5,8],"if":[1,2,3,4,5,7,8,13],"in":1,"int":[2,5,7,13],"null":7,"public":13,"return":[0,1,2,3,4,5,6,7,8,13],"static":[8,13],"switch":8,"true":[2,4,7,8],"void":[0,2,7,13],"{ _":13,"{_":7,"{{":7,"{}":13,"})":8,"};":[0,1,2,3,4,6,7,8,13],"}}":7,_aligned:8,_all:7,_allow:4,_alt:4,_args:4,_b:4,_back:[0,8],_base:1,_bgra:3,_bind:8,_buffer:[2,8],_camera:13,_case:11,_cast:[8,13],_chain:4,_changes:4,_component:8,_constant:[2,8],_d:7,_data:8,_debug:3,_default:[2,7],_depth:7,_desc:[0,4,7,8],_device:3,_dimension:7,_discard:4,_driver:3,_dsv:7,_effect:4,_element:8,_elements:8,_empty:0,_enable:1,_enter:4,_equal:7,_factory:4,_feature:3,_file:1,_flag:4,_flip:4,_float:8,_format:[4,7,8],_fullscreen:4,_gs:2,_h:13,_hardware:3,_id:8,_in:6,_input:8,_layout:8,_less:7,_level:3,_limits:13,_list:7,_macro:1,_major:6,_mask:7,_messages:5,_mode:4,_mwa:4,_name:5,_no:4,_output:[0,2,4],_parameter:[5,8],_per:8,_pitch:13,_point:1,_pointlist:0,_position:[0,2,6],_ppv:4,_projection:13,_ps:2,_r:8,_reflection:8,_render:4,_rtv:7,_s:7,_sampler:8,_screen:2,_sequential:4,_shader:1,_sit:8,_slots:8,_stencil:7,_strictness:1,_support:3,_swap:4,_swapchain:4,_switch:4,_target:[0,2,4,6,7],_texture:[7,8],_to:2,_topology:0,_type:[3,8],_unknown:8,_usage:4,_variable:8,_version:3,_vertex:8,_view:[1,7,13],_vs:2,_window:4,_write:7,_x:[2,13],_y:[2,13],adapter:3,analize:8,append:8,assert:[1,2],auto:[1,2,4,5,7,8,13],backbuffer:7,binary:1,bind:[2,7],bool:[2,7,8,13],buffer:[1,8,10],calc:13,camera:10,cb:8,clang:11,clear:7,cleardepth:7,cleardepthstencilview:7,clearrendertargetview:7,cmake:11,color:[0,2],com:3,comparison:7,compile:1,compiled:8,compileshader:10,comptr:[0,1,2,3,4,7,8],console:3,constant:[8,10],constantbuffer:[2,10],context:[0,2,7],core:10,countof:7,create:[2,3,4,7,8],createbuffer:2,createdepthstencilview:7,createdevice:10,createdeviceandswapchain:4,createinputlayout:8,createrendertargetview:7,createswapchain:10,createswapchainforhwnd:4,createtexture:7,createwindow:10,cube:12,cull:0,cullmode:0,d3d:[0,2,3,7,8],d3dcompile:1,d3dreflect:8,debug:3,define:1,depth:[7,10],depthbuffer:7,depthdesc:7,depthstencil:7,desc:[2,7,8],device:[2,3,4,7,8],devicecontext:[0,2,7],dir:1,direct:10,directx:13,directxmath:13,docs:10,dolly:13,draw:[0,12],dss:7,dssdesc:7,dsv:7,dsvdesc:7,dtype:3,dummyinput:[0,2],dx:13,dxgi:[4,7,8],dy:13,eledesc:8,element:[0,2],elements:8,endif:[3,4],entry:1,err:1,failed:[1,2,3,4,7,8],far:13,featurelevels:3,filesystem:1,flags:[1,3],format:8,fovyrad:13,framecount:5,frontcounterclockwise:0,fstream:1,geometryshader:10,get:[4,8],getadapter:4,getbufferpointer:8,getbuffersize:8,getconstantbufferbyindex:8,getdesc:[7,8],getdxgiformat:8,getinputparameterdesc:8,getparent:4,getresourcebindingdesc:8,getvariablebyindex:8,github:11,gltf:13,gorilla:[1,2,3,4,5,7,8,13],graphics:10,gs:[0,2,8],gsmain:[0,2],gssetconstantbuffers:2,hardcoding:8,height:7,hinstance:5,hprevinstance:5,hr:[1,2,3,4,7,8],hresult:[3,4],https:[10,11],hwnd:[4,5],iasetprimitivetopology:0,iasetvertexbuffers:0,id:[0,4,7],ifdef:3,ifstream:1,iid:8,include:[1,2,4,8,13],initialize:8,inout:[0,2],input:[0,2,6,8],inputassembler:10,inputlayout:8,ios:1,ispress:13,left:13,limits:13,loop:5,lpcmdline:5,main:5,makewindowassociation:4,mask:8,matrix:[6,13],maxdepth:7,maxvertexcount:[0,2],microsoft:[1,3,4,8],middle:13,min:13,mindepth:7,mousebinder:13,move:13,mul:6,mvp:6,name:[1,11],namespace:[1,2,3,4,7,8,13],ncmdshow:5,near:13,nullptr:[0,1,2,3,4],numeric:13,numfeaturelevels:3,omsetdepthstencilstate:7,omsetrendertargets:7,once:13,orbit:10,orbitcamera:13,other:11,output:[0,2,6],outputdebugprintfa:8,path:1,pdxgiadapter:4,pdxgidevice:4,pdxgifactory:4,pipeline:10,pitch:13,pixel:2,pixelshader:6,point:0,position:[0,2,6],preflector:8,primitive:0,projection:13,ps:[6,8],psmain:[0,2,6],pssetconstantbuffers:2,pswapchain:4,pwstr:5,rasterizer:0,rasterizerstate:0,read:1,rect:2,reflect:8,reflection:10,register:8,release:3,render:7,rendertarget:10,rendertargetview:7,renderterget:7,resize:13,ret:1,right:13,row:6,rssetviewports:7,rtv:7,sahder:0,sampler:[8,9],samplerslot:10,samples:11,sd:4,sdk:3,sdkversion:3,set:[2,7],setup:7,sfd:4,shader:10,shaderdesc:8,shaderreflection:8,shaderstage:8,shadervariables:8,shift:13,showwindow:5,sigdesc:8,signature:8,size:2,sizeof:3,slot:2,snake:11,source:1,src:1,srv:[8,9],ss:1,sstream:1,std:[1,8,13],string:1,stringstream:1,struct:[0,2,6,13],sv:[0,2,6],swapchain:7,tan:13,target:1,template:[1,3,4,8],test:10,texture:[7,10],textureslot:10,topleftx:7,toplefty:7,tositeru:10,trianglestream:[0,2],tuple:1,type:8,typename:[1,3,4,8],uint:[1,2,3,5,7,8],unorm:[4,7],unreferenced:5,update:2,updatesubresource:2,updatewindow:5,uppercamel:11,usage:[2,7],using:[1,3,4,8],validfeaturelevel:3,variables:8,vbelement:8,vblob:8,vc:11,vdesc:8,vector:8,vertexshader:[6,8],view:13,viewport:[2,7],viewports:7,vs:[6,8],vsmain:[0,2,6],vssetconstantbuffers:2,wheel:13,width:7,winapi:5,window:5,windows:11,wrl:[1,3,4,8],wwinmain:5,xmconverttoradians:13,xmfloat:13,xmloadfloat:13,xmmatrixmultiply:13,xmmatrixperspectivefovrh:13,xmmatrixrotationx:13,xmmatrixrotationy:13,xmmatrixtranslation:13,xmstorefloat:13,xy:8,xywh:2,xyz:8,xyzw:8,yaw:13,yawpitch:13},titles:["Basic Pipeline","CompileShader","Constant Buffer","CreateDevice","CreateSwapChain","CreateWindow","InputAssembler","RenderTarget","Shader Reflection","Texture","Root","Source","Depth Test","Orbit Camera"],titleterms:{"3d":10,"\u30b7\u30a7\u30fc\u30c0\u30fc":8,"\u30d0\u30c3\u30d5\u30a1":0,"\u30d3\u30e5\u30fc\u30dd\u30fc\u30c8":0,"\u30ec\u30a4\u30a2\u30a6\u30c8":6,"\u53c2\u8003":10,"\u5909\u6570":8,"\u5ea7\u6a19":0,"\u7701\u7565":0,"\u9802\u70b9":[0,6],and:10,basic:[0,10],buffer:2,camera:13,compileshader:1,constant:2,constantbuffer:8,createdevice:3,createswapchain:4,createwindow:5,depth:12,geometryshader:0,indices:10,inputassembler:6,mesh:10,orbit:13,pipeline:0,reflection:8,rendertarget:7,root:10,samplerslot:8,shader:8,source:[10,11],tables:10,test:12,texture:9,textureslot:8}})