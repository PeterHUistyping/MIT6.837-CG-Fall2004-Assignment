# assignment3
Changes: 
ON main.C 
    Variables are defined outside main()->decrease nums of args for functions

ON Object3D childs
    implement backshading (used to be on main.C)
    which helps OpenGL

On camera.C
    Let openGL be the last to consider,separating rendering from it.Thus, no need to recompute.


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