# assignment1
 debug: make sure *.C  clang++
 
 "-input", 
                "scene1_02.txt",   //input_file
                "-size", 
                "200", // width
                "200", // height
                "-output", 
                "output1_1.tga",  //output_file
                "-depth", 
                "9", //depth_min
                "10", //depth_max
                "depth1_1.tga"// depth_file


OrthographicCamera {  //camera //parser.getCamera()
    center 0 0 10 
    direction 0 0 -1
    up 0 1 0
    size 5
}

Lights {  //parser.getLight()
    numLights 1
    DirectionalLight {
        direction -0.5 -0.5 -1
        color 1 1 1
    }
}

Background {    //background_color  //parser.getBackgroundColor()
    color 0.2 0 0.6 
    ambientLight 0 0 0  //parser.getAmbientLight()
}
 

Materials {
    numMaterials 1
    Material { 
	diffuseColor 1 0 0 
    }
}

Group {  //parser.getGroup();
    numObjects 5//parser.getNumMaterials();
   
    MaterialIndex 0  //parser.getMaterial(i);
    Sphere {
	center 0 0 0 
        radius 1 
    }

    MaterialIndex 1
    Sphere {
	center 1 1 1 
        radius 0.75
    }
    Sphere {
	center -1 -1 1 
        radius 0.75
    }

    MaterialIndex 2
    Sphere {
	center -1 1 -1 
        radius 0.75
    }
    Sphere {
	center 1 -1 -1 
        radius 0.75 
    }
}




Scene description file grammar

file	::=	camera background lights materials group
camera	::=	orthographicCamera | perspectiveCamera
orthographicCamera	::=	OrthographicCamera { 
    center Vec3f
    direction Vec3f
    up Vec3f
    size float 
}
perspectiveCamera	::=	PerspectiveCamera { 
    center Vec3f
    direction Vec3f
    up Vec3f
    angle float 
}
background	::=	Background { 
    color Vec3f
    ambientLight Vec3f
}
lights	::=	Lights { 
    numLights int
    light ^ numLights
}
light	::=	directionalLight
directionalLight	::=	DirectionalLight { 
    direction Vec3f
    color Vec3f
}
materials	::=	Materials { 
    numMaterials int
    Material ^ numMaterials
}
Material	::=	Material { 
    diffuseColor Vec3f
}
group	::=	Group { 
    numObjects int
    (object3D | material object3D) ^ numObjects
}
object3D	::=	group | transform | sphere | plane | triangle | triangleMesh
transform	::=	Transform { 
    transformation*
    object3D
}
transformation	::=	Translate Vec3f   | 
Scale Vec3f   | 
XRotate float   | 
YRotate float   | 
ZRotate float   | 
Rotate { Vec3f float }   | 
Matrix { float ^ 16 }
sphere	::=	Sphere { 
    center Vec3f
    radius float
}
plane	::=	Plane { 
    normal Vec3f
    offset float
}
triangle	::=	Triangle { 
    vertex0 Vec3f
    vertex1 Vec3f
    vertex2 Vec3f
}
triangleMesh	::=	TriangleMesh { 
    obj_file string
}
material	::=	Material { 
    diffuse_color Vec3f
}
Vec3f	::=	float float float

How to read a grammar:

Start with the top level rule, in this case file. To expand a rule, substitute the appropriate definitions for each of the elements in bold. Some special notation:
|	means "OR"
^ n	means "exactly n elements"
*	means "zero or more elements"